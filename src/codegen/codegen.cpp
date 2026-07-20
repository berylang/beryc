#include "codegen.h"

/*
   
   Bery IR Code Generator,

   it tranverse the AST and emits LLVM IR text directly into an output stream.
   it operates in three passes:
      @global pass - emtting global variables, arrays, functions signatures,extern declarations,
      @functions pass - emits each func body as an LLVM functions.
      @run{} pass - emits @main() which calls 'bery_runtime_startup()', runs the run block statement, and then calss 'bery_runtime_shutdown()'

*/

#include "../parser/ast/programnode.h"
#include "../parser/ast/vardecl.h"
#include "../parser/ast/literals.h"
#include "../parser/ast/arraydeclare.h"
#include "../parser/ast/expressions.h"
#include "../parser/ast/blocknode.h"
#include "../parser/ast/functions.h"
#include "../sema/symboltable.h"
#include <fstream>
#include <iomanip>
#include <functional>

CodeGen::CodeGen(ASTNode* root, SymbolTable& symTable)
   : root(root), symTable(symTable){}

void CodeGen::generate(const std::string& outputPath) {
    auto* program = static_cast<ProgramNode*>(root);
    
    std::ostringstream globalsOut;
    globalsOut << "declare double @llvm.pow.f64(double, double)\n";
    globalsOut << "declare void @bery_runtime_startup()\n";
    globalsOut << "declare void @bery_runtime_shutdown()\n";

    for (auto& node : program->globals) {
        if (node->type == NodeType::FUNC_DEF) {
            auto* func = static_cast<FunctionDefNode*>(node.get());
            CodeGenFunctionSignature sig;
            sig.returnType = func->returnType;
            for (auto& p : func->parameters) sig.paramTypes.push_back(p.first);
            functions[func->name] = sig;
            
            genFuncDef(node.get(), globalsOut);
        }
        else if (node->type == NodeType::EXTERN_DECL) {
            auto* ext = static_cast<ExternDeclNode*>(node.get());
            CodeGenFunctionSignature sig;
            sig.returnType = ext->returnType;

            std::string decl = "declare " + llvmType(ext->returnType) + " @" + ext->name + "(";
            for (size_t i = 0; i < ext->parameters.size(); ++i) {
                sig.paramTypes.push_back(ext->parameters[i].first);
                decl += llvmType(ext->parameters[i].first);
                if (i + 1 < ext->parameters.size()) decl += ", ";
            }
            decl += ")";

            functions[ext->name] = sig;
            globalsOut << decl << "\n";
        }else if (node->type == NodeType::CLASS_DEF) {
            genClassDecl(node.get());
        }
    }

    for (auto& node : program->globals) {
        if (node->type == NodeType::VAR_DECL) {
            auto* decl = static_cast<VarDeclNode*>(node.get());
            std::string lt = llvmType(decl->varType);
            std::string initVal = extractConstant(decl->value.get());
            
            symTable.get(decl->name).llvmRegister = "@" + decl->name;
            symTable.get(decl->name).llvmAllocType = lt;             
            llvm.__emitGlobalVar(decl->name, lt, initVal, globalsOut);
        }
        else if (node->type == NodeType::ENUM_DECL) {
            auto* enumDecl = static_cast<EnumDeclNode*>(node.get());
            int currentValue = 0;
            
            for (const auto& val : enumDecl->values) {
                std::string mangledName = enumDecl->name + "." + val;
                std::string globalName = "@" + mangledName;
                std::string lt = "i32";
                
                symTable.get(mangledName).llvmRegister = globalName;
                symTable.get(mangledName).llvmAllocType = lt;
                llvm.__emitGlobalVar(mangledName, lt, std::to_string(currentValue++), globalsOut);
            }
        }
        else if (node->type == NodeType::ARRAY_DECL) {
            auto* decl = static_cast<ArrayDeclNode*>(node.get());
            if (decl->dimensions.size() == 1 && decl->dimensions[0] == -1) {
                llvm.__declareExtern("declare i8* @bery_array_new(i64)", "bery_array_new");
                std::string memReg = "@" + decl->name + "_slot";
                symTable.get(decl->name).llvmRegister = memReg;
                symTable.get(decl->name).llvmAllocType = "i8*";
                llvm.__emitGlobalVar(decl->name + "_slot", "i8*", "null", globalsOut);
                continue;
            }
            symTable.get(decl->name).llvmRegister = "@" + decl->name;
            std::string lt = llvmType(decl->elementType);
            
            int totalSize = 1;
            for (size_t i = 0; i < decl->dimensions.size(); ++i) {
                totalSize *= decl->dimensions[i];
            }
            std::string arrType = lt;
            for (int i = decl->dimensions.size() - 1; i >= 0; --i) {
                arrType = "[" + std::to_string(decl->dimensions[i]) + " x " + arrType + "]";
            }
            symTable.get(decl->name).llvmAllocType = arrType;
            std::string initVal;
            if (decl->initializers.empty()) {
                initVal = "zeroinitializer";
            } else {
                std::function<std::string(int, int)> buildNestedInit = 
                [&](int dimIndex, int flatOffset) -> std::string {
                    if (dimIndex == decl->dimensions.size() - 1) {
                        std::string res = "[";
                        int size = decl->dimensions[dimIndex];
                        for (int i = 0; i < size; ++i) {
                            int idx = flatOffset + i;
                            std::string val = (idx < (int)decl->initializers.size()) ? extractConstant(decl->initializers[idx].get()) : "0";
                            res += lt + " " + val;
                            if (i + 1 < size) res += ", ";
                        }
                        res += "]";
                        return res;
                    }

                    std::string res = "[";
                    int size = decl->dimensions[dimIndex];
                    int childFlatSize = 1;
                    for (size_t i = dimIndex + 1; i < decl->dimensions.size(); ++i) {
                        childFlatSize *= decl->dimensions[i];
                    }
                    std::string childType = lt;
                    for (int i = decl->dimensions.size() - 1; i > dimIndex; --i) {
                        childType = "[" + std::to_string(decl->dimensions[i]) + " x " + childType + "]";
                    }

                    for (int i = 0; i < size; ++i) {
                        res += childType + " " + buildNestedInit(dimIndex + 1, flatOffset + (i * childFlatSize));
                        if (i + 1 < size) res += ", ";
                    }
                    res += "]";
                    return res;
                };

                initVal = buildNestedInit(0, 0);
            }
            llvm.__emitGlobalVar(decl->name, arrType, initVal, globalsOut);
        }
    }

    std::ostringstream body;
    llvm.__emitFunctionHeader("i32", "main", {}, body);
    body << "    call void @bery_runtime_startup()\n";
    for (auto& clPair : classLayouts) {
        ClassLayout& cl = clPair.second;
        int nameLen = (int)cl.name.length() + 1;
        std::string destructorArg = "i8* null";
        if (cl.hasDestructor) {
            std::string dtorReg = llvm.__emitDestructorBitcast(cl.llvmStructType, cl.name, body);
            destructorArg = "i8* " + dtorReg;
        }
        llvm.__emitTypeRegisterCall(cl.name, nameLen, (long long)cl.instanceSize, destructorArg, body);
    }
    pushGCScope();
    for (auto& node : program->globals) {
        if (node->type == NodeType::ARRAY_DECL) {
            auto* decl = static_cast<ArrayDeclNode*>(node.get());
            if (decl->dimensions.size() == 1 && decl->dimensions[0] == -1) {
                std::string arrReg = llvm.__emitCall("i8*", "bery_array_new", {{"i64", "4"}}, body);
                llvm.__emitStore("i8*", arrReg, "@" + decl->name + "_slot", body);
            }
        }
    }

    if (program->runBlock) {
        symTable.pushScope();
        for (auto& node : program->runBlock->statements) {
            genStatement(node.get(), body);
        }
        symTable.popScope();
    }
    int rootsInMain = popGCScope();
    emitGCPops(rootsInMain, body);
    llvm.__emitBr("main_end", body);
    llvm.__emitLabel("main_end", body);
    llvm.__emitReturn("i32", "0", body);
    llvm.__emitFunctionFooter(body);

    std::ofstream out(outputPath);
    out << llvm.__STRUCTURE_declares.str();
    out << globalsOut.str();
    out << llvm.__BRE_declares.str();
    out << llvm.__GLOBAL_STRINGS.str() << "\n";
    out << body.str();
}
