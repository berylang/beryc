#include "../codegen.h"
#include "../../parser/ast/vardecl.h"
#include "../../parser/ast/arraydeclare.h"
#include "../../parser/ast/classes.h"
#include "../../parser/ast/functions.h"
#include "../../parser/ast/classes.h"
#include "../../parser/ast/expressions.h"
#include "../../sema/symboltable.h"

void CodeGen::genClassDecl(ASTNode* node) {
    auto* cls = static_cast<ClassDefNode*>(node);

    ClassLayout layout;
    layout.name = cls->name;
    layout.llvmStructType = llvm.__structTypeName(cls->name);

    std::vector<std::string> fieldTypes;
    if (cls->attributes) {
        for (size_t i = 0; i < cls->attributes->attributes.size(); ++i) {
            auto* field = static_cast<VarDeclNode*>(cls->attributes->attributes[i].get());
            std::string lt = llvmType(field->varType);
            layout.fields.push_back({field->varType, field->name});
            layout.fieldIndex[field->name] = (int)i;
            layout.fieldInitializers.push_back(field->value.get());
            fieldTypes.push_back(lt);
        }
    }
    llvm.__emitStructType(layout.llvmStructType, fieldTypes);

    layout.instanceSize = 0;
    for (auto& field : layout.fields) {
        layout.instanceSize += (size_t)llvm.__alignOf(llvmType(field.first));
    }
    if (layout.instanceSize == 0) {
        layout.instanceSize = 1;
    }

    llvm.__emitClassNameGlobal(cls->name);
    if (cls->methods) {
        for (auto& m : cls->methods->methods) {
            auto* f = static_cast<FunctionDefNode*>(m.get());
            if (f->isConstructor) layout.hasConstructor = true;
            if (f->isDestructor) layout.hasDestructor = true;
        }
    }
    classLayouts[cls->name] = layout;
    if (cls->methods) {
        for (auto& m : cls->methods->methods) {
            auto* func = static_cast<FunctionDefNode*>(m.get());
            std::string mangledName = func->isConstructor ? (cls->name + "$ctor") : func->isDestructor ? (cls->name + "$dtor") : (cls->name + "_" + func->name);

            CodeGenFunctionSignature sig;
            sig.returnType = func->returnType;
            sig.paramTypes.push_back(cls->name + "*");
            for (auto& p : func->parameters) sig.paramTypes.push_back(p.first);
            functions[mangledName] = sig;

            std::ostringstream methodOut;
            std::string retLT = (func->returnType == "void" || func->returnType.empty())
                                 ? "void" : llvmType(func->returnType);
            currentFuncReturn = func->returnType;

            std::vector<std::pair<std::string, std::string>> params;
            params.push_back({layout.llvmStructType + "*", "%self_arg"});
            for (auto& p : func->parameters)
                params.push_back({llvmType(p.first), "%" + p.second + "_arg"});
            llvm.__emitFunctionHeader(retLT, mangledName, params, methodOut);

            symTable.pushScope();
            pushGCScope();
            currentClassName = cls->name;
            currentSelfRef = cls->attributes->selfRef;

            std::string selfReg = "%self_" + std::to_string(llvm.__uniqueId());
            methodOut << "    " << selfReg << " = alloca " << layout.llvmStructType << "*\n";
            llvm.__emitStore(layout.llvmStructType + "*", "%self_arg", selfReg, methodOut);

            Symbol selfSym;
            selfSym.type = cls->name;
            selfSym.isConst = false;
            selfSym.isInitialized = true;
            selfSym.llvmRegister = selfReg;
            selfSym.llvmAllocType = layout.llvmStructType + "*";
            selfSym.line = cls->line;
            symTable.add(cls->attributes->selfRef, selfSym);

            std::string loadedSelf = llvm.__emitLoad(layout.llvmStructType + "*", selfReg, methodOut);

            for (auto& field : layout.fields) {
                auto& beryT = field.first;
                auto& fieldName = field.second;
                std::string lt = llvmType(beryT);
                int idx = layout.fieldIndex[fieldName];
                std::string gepReg = "%" + fieldName + "_" + std::to_string(llvm.__uniqueId());
                methodOut << "    " << gepReg << " = getelementptr inbounds "
                          << layout.llvmStructType << ", " << layout.llvmStructType << "* "
                          << loadedSelf << ", i32 0, i32 " << idx << "\n";

                Symbol fieldSym;
                fieldSym.type = beryT;
                fieldSym.isConst = false;
                fieldSym.isInitialized = true;
                fieldSym.llvmRegister = gepReg;
                fieldSym.llvmAllocType = lt;
                fieldSym.line = cls->line;
                symTable.add(fieldName, fieldSym);
            }

            for (auto& p : func->parameters) {
                std::string pLT = llvmType(p.first);
                std::string pReg = "%" + p.second + "_" + std::to_string(llvm.__uniqueId());

                Symbol paramSym;
                paramSym.type = p.first;
                paramSym.isConst = false;
                paramSym.isInitialized = true;
                paramSym.llvmRegister = pReg;
                paramSym.llvmAllocType = pLT;
                paramSym.line = func->line;
                symTable.add(p.second, paramSym);

                methodOut << "    " << pReg << " = alloca " << pLT << "\n";
                llvm.__emitStore(pLT, "%" + p.second + "_arg", pReg, methodOut);
            }

            for (auto& stmt : func->body->statements)
                genStatement(stmt.get(), methodOut);

            int roots = popGCScope();
            emitGCPops(roots, methodOut);
            symTable.popScope();
            currentClassName = "";
            currentSelfRef = "";

            llvm.__emitDefaultReturn(retLT, methodOut);
            llvm.__emitFunctionFooter(methodOut);
            llvm.__GLOBAL_STRINGS << methodOut.str();
            currentFuncReturn = "";
        }
    }

    classLayouts[cls->name] = std::move(layout);
}


void CodeGen::genVarDecl(ASTNode* node, std::ostream& out) {
    auto* decl = static_cast<VarDeclNode*>(node);
    std::string lt = llvmType(decl->varType);
    std::string memReg = "%" + decl->name + "_" + std::to_string(llvm.__uniqueId());
    Symbol sym;
    sym.symbolType = SymbolType::VARIABLE;
    sym.type = decl->varType;
    sym.isConst = decl->isConst;
    sym.isInitialized = decl->value != nullptr;
    sym.line = decl->line;
    sym.llvmRegister = memReg;
    sym.llvmAllocType = lt;
    symTable.add(decl->name, sym);
    out << "    " << memReg << " = alloca " << lt << "\n";
    if (decl->varType == "string" || classLayouts.count(decl->varType)) {
        emitGCPush(memReg, lt, out);
    }
    if (!decl->value) return;
    std::string valReg = genExpression(decl->value.get(), decl->varType, out);
    llvm.__emitStore(lt, valReg, memReg, out);
}

void CodeGen::genArrayDecl(ASTNode* node, std::ostream& out) {
    auto* decl = static_cast<ArrayDeclNode*>(node);
    if (decl->dimensions.size() == 1 && decl->dimensions[0] == -1) {
        std::string memReg = "%" + decl->name + "_" + std::to_string(llvm.__uniqueId());
        Symbol sym;
        sym.symbolType = SymbolType::VARIABLE;
        sym.type = "array<" + decl->elementType + ">";
        sym.isInitialized = true;
        sym.line = decl->line;
        sym.llvmRegister = memReg;
        sym.llvmAllocType = "i8*";
        symTable.add(decl->name, sym);
        out << "    " << memReg << " = alloca i8*\n";
        if (decl->valueExpr) {
            std::string valReg = genExpression(decl->valueExpr.get(), sym.type, out);
            llvm.__emitStore("i8*", valReg, memReg, out);
        } else {
            llvm.__declareExtern("declare i8* @bery_array_new(i64)", "bery_array_new");
            std::string arrReg = llvm.__emitCall("i8*", "bery_array_new", {{"i64", "4"}}, out);
            llvm.__emitStore("i8*", arrReg, memReg, out);
        }
        emitGCPush(memReg, "i8*", out);
        return;
    }
    std::string lt = llvmType(decl->elementType);

    std::string arrType = lt;
    for (int i = decl->dimensions.size() - 1; i >= 0; --i) {
        arrType = "[" + std::to_string(decl->dimensions[i]) + " x " + arrType + "]";
    }
    
    std::string memReg = "%" + decl->name + "_" + std::to_string(llvm.__uniqueId());
    std::string typeSig = decl->elementType;
    for (size_t i = 0; i < decl->dimensions.size(); ++i) typeSig += "[]";
    
    Symbol sym;
    sym.symbolType = SymbolType::VARIABLE;
    sym.type = typeSig;
    sym.isInitialized = !decl->initializers.empty();
    sym.line = decl->line;
    sym.llvmRegister = memReg;
    sym.llvmAllocType = arrType;
    sym.arrayDimensions = decl->dimensions;
    sym.arraySize = 1;
    for (int d : decl->dimensions) sym.arraySize *= d;
    symTable.add(decl->name, sym);

    out << "    " << memReg << " = alloca " << arrType << "\n";
    if (decl->initializers.empty()) return;
    std::string flatPtr = llvm.__emitBitcast(arrType + "*", memReg, lt + "*", out);


    for (size_t i = 0; i < decl->initializers.size(); ++i) {
        std::string valReg = genExpression(decl->initializers[i].get(), decl->elementType, out);
        std::string ptrReg = llvm.__emitGEP(lt, flatPtr, {"i32 " + std::to_string(i)}, false, out);
        llvm.__emitStore(lt, valReg, ptrReg, out);
    }
}

void CodeGen::genFuncDef(ASTNode* node, std::ostream& out) {
    auto* func = static_cast<FunctionDefNode*>(node);
    std::string retLT = (func->returnType == "void") ? "void" : llvmType(func->returnType);
    currentFuncReturn = func->returnType;

    std::vector<std::pair<std::string, std::string>> params;
    for (auto& p : func->parameters) {
        params.push_back({llvmType(p.first), "%" + p.second + "_arg"});
    }
    llvm.__emitFunctionHeader(retLT, func->name, params, out);

    symTable.pushScope();
    pushGCScope();
    for (auto& param : func->parameters) {
        std::string pType = llvmType(param.first);
        std::string pName = param.second;
        std::string memReg = "%" + pName + "_" + std::to_string(llvm.__uniqueId());

        Symbol sym;
        sym.symbolType = SymbolType::VARIABLE;
        sym.type = param.first;
        sym.isInitialized = true;
        sym.line = func->line;
        sym.llvmRegister = memReg;
        sym.llvmAllocType = pType;
        symTable.add(pName, sym);

        out << "    " << memReg << " = alloca " << pType << "\n";
        llvm.__emitStore(pType, "%" + pName + "_arg", memReg, out);
        bool isHeapTracked = param.first == "string" || classLayouts.count(param.first) || (param.first.size() > 6 && param.first.substr(0, 6) == "array<");
        if (isHeapTracked) { emitGCPush(memReg, pType, out); }
    }

    for (auto& stmt : func->body->statements) genStatement(stmt.get(), out);
    int roots = popGCScope();
    emitGCPops(roots, out);
    symTable.popScope();
    bool endsWithReturn = false;
    if (!func->body->statements.empty() &&
        func->body->statements.back()->type == NodeType::RETURN_STMT) {
        endsWithReturn = true;
    }

    if (!endsWithReturn) {
        llvm.__emitReturn(retLT, retLT == "void" ? "" : "0", out);
    }
    llvm.__emitDefaultReturn(retLT, out);
    llvm.__emitFunctionFooter(out);
    currentFuncReturn = "";
}


void CodeGen::genReturnStmt(ASTNode* node, std::ostream& out) {
    auto* retNode = static_cast<ReturnStmtNode*>(node);
    
    std::string valReg;
    if (retNode->value) {valReg = genExpression(retNode->value.get(), currentFuncReturn, out); }
    int totalRoots = 0;
    std::stack<int> tempStack = gcRootScopeStack;
    while (!tempStack.empty()) {
        totalRoots += tempStack.top();
        tempStack.pop();
    }
    emitGCPops(totalRoots, out);
    llvm.__emitReturn(llvmType(currentFuncReturn), retNode->value ? valReg : "", out);
    llvm.__emitLabel("dead_code_" + std::to_string(llvm.__uniqueId()), out);
}