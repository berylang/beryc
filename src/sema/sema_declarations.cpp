#include "sema.h"

/*

    Semantic Analyzer, Declarations,

    this file analyze - variable declarations, array declarations, function declarations, enum declarations


*/

#include "../parser/ast/vardecl.h"
#include "../parser/ast/arraydeclare.h"
#include "../parser/ast/functions.h"
#include "../parser/ast/classes.h"
#include <iostream>
#include <unordered_set>

const std::unordered_set<std::string> PRIMITIVE_TYPES = { "int", "bigint", "bool", "float", "double", "char", "string"};

bool SemanticAnalyzer::isImplicityConversionCheck(const std::string& fromType, const std::string& toType) {
    return (toType == "float"  && fromType == "int")|| (toType == "double" && fromType == "int")||
    (toType == "double" && fromType == "float") || (toType == "bigint" && fromType == "int");
}


void SemanticAnalyzer::analyzeVarDecl(ASTNode* node) {
    auto* decl = static_cast<VarDeclNode*>(node);
    if (!isKnownType(decl->varType)) {
        diag.report("ERROR315", decl->line, 1, "", decl->varType);
        return;
    }
    if (symbolTable.existsInCurrentScope(decl->name)) {
        diag.report("ERROR316", decl->line, 1, "", decl->name);
        return;
    }
    if (decl->isConst && !decl->value) {
        diag.report("ERROR317", decl->line, 1, "", decl->name);
        return;
    }
    if (decl->value) {
        std::string exprtype = typeChecker.analyzeExpression(decl->value.get());
        if (exprtype != "unknown" && exprtype != decl->varType) {
            if (exprtype == "null") {
                if (decl->varType != "string") {
                    diag.report("ERROR318", decl->line, 1, "", decl->varType);
                    return;
                }
            }
            else if (!isImplicityConversionCheck(exprtype, decl->varType)) {
                diag.report("ERROR319", decl->line, 1, "", decl->name + std::string(". Expected '") + decl->varType + "', got '" + exprtype);
                return;
            }
        }
    }
    symbolTable.addVariable(decl->name, decl->varType, decl->isConst, decl->value != nullptr, decl->line);
}


bool SemanticAnalyzer::isKnownType(const std::string& t) {
    if (PRIMITIVE_TYPES.count(t)) 
        return true;
    if (classes.count(t)) 
        return true;
    if (t.size() > 6 && t.substr(0, 6) == "array<" && t.back() == '>') {
        std::string inner = t.substr(6, t.size() - 7);
        return PRIMITIVE_TYPES.count(inner) > 0 || classes.count(inner) > 0;
    }
    return false;
}

void SemanticAnalyzer::analyzeArrayDecl(ASTNode* node) {
    auto* decl = static_cast<ArrayDeclNode*>(node);
    if (!isKnownType(decl->elementType)) {
        diag.report("ERROR320", decl->line, 1, "", decl->elementType);
         return;
    }
    if (symbolTable.existsInCurrentScope(decl->name)) {
        diag.report("ERROR321", decl->line, 1, "", decl->name);
         return;
    }
    std::string arrayType = "array<" + decl->elementType + ">";
    bool isDynamic = decl->dimensions.size() == 1 && decl->dimensions[0] == -1;

    if (isDynamic) {
        if (decl->valueExpr) {
            std::string exprType = typeChecker.analyzeExpression(decl->valueExpr.get());
            if (exprType != "unknown" && exprType != arrayType) {
                diag.report("ERROR322", decl->line, 1, "", decl->name + "'. Expected '" + arrayType + "', got '" + exprType);
                
            }
            symbolTable.addVariable(decl->name, arrayType, decl->isConst, true, decl->line, decl->dimensions);
            return;
        }
        if (decl->initializers.empty()) {
            symbolTable.addVariable(decl->name, arrayType, decl->isConst, false, decl->line, decl->dimensions);
            return;
        }
        for (auto& initVal : decl->initializers) {
            std::string exprType = typeChecker.analyzeExpression(initVal.get());
            if (exprType != "unknown" && exprType != decl->elementType) {
                if (!(decl->elementType == "float" && exprType == "int") &&
                    !(decl->elementType == "double" && exprType == "int")) {
                    diag.report("ERROR323", decl->line, 1, "", "");
                     return;
                }
            }
        }
        std::vector<int> dims = { (int)decl->initializers.size() };
        symbolTable.addVariable(decl->name, arrayType, decl->isConst, true, decl->line, dims);
        return;
    }

    int totalSize = 1;
    int inferredDim = -1;
    for (size_t i = 0; i < decl->dimensions.size(); ++i) {
        if (decl->dimensions[i] < 0) {
            if (i != 0) {
                diag.report("ERROR324", decl->line, 1, "", "");
                 return;
            }
            inferredDim = i;
        } else if (decl->dimensions[i] == 0) {
            diag.report("ERROR325", decl->line, 1, "", "");
             return;
        } else {
            totalSize *= decl->dimensions[i];
        }
    }
    if (inferredDim != -1) {
        if (decl->initializers.empty()) {
            diag.report("ERROR326", decl->line, 1, "", "");
             return;
        }
        if (decl->initializers.size() % totalSize != 0) {
            diag.report("ERROR327", decl->line, 1, "", "");
             return;
        }
        decl->dimensions[0] = decl->initializers.size() / totalSize;
        totalSize *= decl->dimensions[0];
    }

    if (!decl->initializers.empty() && decl->initializers.size() > (size_t)totalSize) {
        diag.report("ERROR328", decl->line, 1, "", "");
         return;
    }

    for (auto& initVal : decl->initializers) {
        std::string exprType = typeChecker.analyzeExpression(initVal.get());
        if (exprType != "unknown" && exprType != decl->elementType) {
            if (!(decl->elementType == "float" && exprType == "int") &&
                !(decl->elementType == "double" && exprType == "int")) {
                diag.report("ERROR323", decl->line, 1, "", "");
                 return;
            }
        }
    }

    symbolTable.addVariable(decl->name, arrayType, decl->isConst, !decl->initializers.empty(), decl->line, decl->dimensions);
}
void SemanticAnalyzer::analyzeFuncDef(ASTNode* node) {
    auto* func = static_cast<FunctionDefNode*>(node);

    for (auto& param : func->parameters) {
        if (!isKnownType(param.first)) {
            diag.report("ERROR329", func->line, 1, "", param.first + "' in function '" + func->name);
        }
    }
    if (func->returnType != "void" && !func->returnType.empty() && !isKnownType(func->returnType)) {
        diag.report("ERROR330", func->line, 1, "", func->returnType + "' in function '" + func->name);
    }

    currentFunctionReturnType = (func->returnType == "void") ? "" : func->returnType;
    functionDepth++;
    symbolTable.pushScope();
    for (auto& param : func->parameters) {
        std::vector<int> dims = (param.first.size() > 6 && param.first.substr(0,6) == "array<") ? std::vector<int>{-1} : std::vector<int>{};
        symbolTable.addVariable(param.second, param.first, false, true, func->line, dims);
    }
    
    for (auto& statement : func->body->statements) 
        analyzeNode(statement.get());
    
    symbolTable.popScope();
    functionDepth--;
    currentFunctionReturnType = "";
}

void SemanticAnalyzer::analyzeReturnStmt(ASTNode* node) {
    auto* ret = static_cast<ReturnStmtNode*>(node);
    if (functionDepth <= 0) {
        diag.report("ERROR331", ret->line, 1, "", "");
        return;
    }

    if (currentFunctionReturnType.empty()) {
        if (ret->value) {
            diag.report("ERROR403", ret->line, 1, "", "");
        }
        return;
    }

    if (!ret->value) {
        diag.report("ERROR332", ret->line, 1, "", currentFunctionReturnType);
        return;
    }

    std::string valType = typeChecker.analyzeExpression(ret->value.get());
    if (valType != "unknown" && valType != currentFunctionReturnType && !isImplicityConversionCheck(valType, currentFunctionReturnType)) {
        diag.report("ERROR333", ret->line, 1, "", currentFunctionReturnType + "', got '" + valType);
    }
}
void SemanticAnalyzer::analyzeEnumDecl(ASTNode* node) {
    auto* enumDecl = static_cast<EnumDeclNode*>(node);
    for (const auto& val : enumDecl->values) {
        std::string mangledName = enumDecl->name + "." + val; 
        if (symbolTable.existsInCurrentScope(mangledName)) {
            diag.report("ERROR334", enumDecl->line, 1, "", mangledName);
            
        } else {
            symbolTable.addVariable(mangledName, "int", true, true, enumDecl->line);
        }
    }
}

void SemanticAnalyzer::analyzeClassDecl(ASTNode* node) {
    auto* cls = static_cast<ClassDefNode*>(node);
    if (!cls->parentName.empty()) {
        if (cls->parentName == cls->name) {
            diag.report("ERROR335", cls->line, 1, "", cls->name);
            
        } else if (!classes.count(cls->parentName)) {
            diag.report("ERROR336", cls->line, 1, "", cls->parentName + "' for class '" + cls->name);
            
        } else {
            std::unordered_set<std::string> visited;
            std::string cur = cls->parentName;
            while (!cur.empty()) {
                if (cur == cls->name) {
                    diag.report("ERROR337", cls->line, 1, "", cls->name);
                    
                    break;
                }
                if (visited.count(cur)) break; 
                visited.insert(cur);
                auto it = classes.find(cur);
                cur = (it != classes.end()) ? it->second->parentName : "";
            }
        }
    }
    std::unordered_set<std::string> seen;
    if (cls->attributes) {
        for (auto& attr : cls->attributes->attributes) {
            std::string fieldName, fieldType;
            
            if (attr->type == NodeType::VAR_DECL) {
                auto* field = static_cast<VarDeclNode*>(attr.get());
                fieldName =field->name;
                fieldType =field->varType;
                
                if (field->value) {
                    std::string exprType = typeChecker.analyzeExpression(field->value.get());
                    if (exprType != "unknown" && exprType != fieldType) {
                        if (exprType == "null") {
                            if (fieldType!="string" && !classes.count(fieldType)) {
                                diag.report("ERROR338", field->line, 1, "", fieldName);
                            }
                        } else if (!(fieldType == "float" && exprType == "int") &&!(fieldType == "double" && exprType == "int") &&
                            !(fieldType == "bigint" && exprType == "int") && !(fieldType == "double" && exprType == "float") &&
                            !(fieldType == "float" && exprType == "double")) {
                            diag.report("ERROR339", field->line, 1, "", fieldName + "'. Expected '" + fieldType + "', got '" + exprType);
                            
                        }
                    }
                }
            } else if (attr->type == NodeType::ARRAY_DECL) {
                auto* field = static_cast<ArrayDeclNode*>(attr.get());
                fieldName = field->name;
                fieldType = "array<" + field->elementType + ">";
                
                if (!isKnownType(field->elementType)) {
                    diag.report("ERROR320", field->line, 1, "", field->elementType);
                    
                }
            } else {continue;}

            if (seen.count(fieldName)) {
                diag.report("ERROR340", attr->line, 1, "", fieldName + "' in class '" + cls->name);
                
            }
            seen.insert(fieldName);
        }}

    if (cls->methods) {
        int destructorCount = 0;
        for (auto& m : cls->methods->methods) {
            auto* f = static_cast<FunctionDefNode*>(m.get());
            if (f->isDestructor) {
                destructorCount++;
                if (destructorCount > 1) {
                    diag.report("ERROR341", f->line, 1, "", cls->name);
                    
                }
                if (!f->parameters.empty()) {
                    diag.report("ERROR342", f->line, 1, "", cls->name);
                    
                }
            }
        }

        currentClassContext = cls->name;
        for (auto& m : cls->methods->methods) {
            auto* func = static_cast<FunctionDefNode*>(m.get());
            for (auto& p : func->parameters) {
                if (!isKnownType(p.first)) {
                    diag.report("ERROR343", func->line, 1, "", p.first + "' in method '" + func->name);
                    
                }
            }
            if (!func->isConstructor && !func->isDestructor && func->returnType != "void" && !func->returnType.empty() && !isKnownType(func->returnType)) {
                diag.report("ERROR344", func->line, 1, "", func->returnType + "' in method '" + func->name);
            }

            FunctionSignature sig;
            sig.returnType = func->returnType;
            for (auto& p : func->parameters) sig.parameterTypes.push_back(p.first);
            std::string methodName = cls->name +"."+func->name;
            std::vector<FunctionSignature>& moverload = functions[methodName];
            if(!cls->parentName.empty()){
                std::string parentMethodName = cls->parentName + "." + func->name;
                auto parentFunc = functions.find(parentMethodName);
                if(parentFunc != functions.end()){
                    for(const auto& parentFuncSig : parentFunc->second){
                        if(parentFuncSig.parameterTypes == sig.parameterTypes){
                            if(parentFuncSig.returnType != sig.returnType){
                                diag.report("ERROR345", func->line, 1, "", func->name);
                                
                            }
                            break;
                        }
                    }
                }
            }
            bool dupeoverload = false;
             for(auto& existing : moverload){
                if(existing.parameterTypes == sig.parameterTypes){
                    dupeoverload = true;
                    break; 
                }
            } 
            if(dupeoverload && !func->isDestructor){
                diag.report("ERROR346", func->line, 1, "", func->name);
                
                    
            }
            moverload.push_back(sig);
            currentFunctionReturnType = (func->returnType == "void") ? "" : func->returnType;
            functionDepth++;
            currentFunctionIsConstructor = func->isConstructor;
            symbolTable.pushScope();
            if (cls->attributes) {
                symbolTable.addVariable(cls->attributes->selfRef, cls->name, false, true, cls->line);
                for (auto& attr : cls->attributes->attributes) {
                    if (attr->type == NodeType::VAR_DECL){
                        auto* field = static_cast<VarDeclNode*>(attr.get());
                        symbolTable.addVariable(field->name, field->varType, false, true, field->line);
                    } else if (attr->type == NodeType::ARRAY_DECL){
                        auto* field = static_cast<ArrayDeclNode*>(attr.get());
                        std::string arrType = "array<" + field->elementType + ">";
                        symbolTable.addVariable(field->name, arrType, false, true, field->line, field->dimensions);
                    }
                }
            }

            for (auto& p : func->parameters) {
                std::vector<int> dims = (p.first.size() > 6 && p.first.substr(0,6) == "array<") ? std::vector<int>{-1} : std::vector<int>{};
                symbolTable.addVariable(p.second, p.first, false, true, func->line, dims);
            }

            for (auto& statement : func->body->statements)
                analyzeNode(statement.get());

            symbolTable.popScope();
            functionDepth--;
            currentFunctionReturnType = "";
            currentFunctionIsConstructor = false;
        }
        currentClassContext = "";
    }
}