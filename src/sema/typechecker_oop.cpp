#include "typechecker.h"

#include "../parser/ast/expressions.h"
#include "../parser/ast/functions.h"
#include "../parser/ast/classes.h"
#include "../parser/ast/vardecl.h"
#include "../parser/ast/arraydeclare.h"
#include <iostream>



std::string TypeChecker::checkNewExpr(ASTNode* node) {
    auto* newExpr = static_cast<NewExprNode*>(node);
    auto classIt = classes.find(newExpr->className);
    if (classIt == classes.end()) {
        diag.report("ERROR384", newExpr->line, 1, "", newExpr->className);
        
        newExpr->resolvedType = "unknown";
        return newExpr->resolvedType;
    }

    std::vector<FunctionDefNode*> candidates = findConstructors(classIt->second);
    if (candidates.empty()) {
        if (!newExpr->arguments.empty()) {
            diag.report("ERROR385", newExpr->line, 1, "", newExpr->className + "' has no constructor accepting " + std::to_string(newExpr->arguments.size()));
            
        }
        for (auto& arg : newExpr->arguments) analyzeExpression(arg.get());
        newExpr->resolvedType = newExpr->className;
        return newExpr->resolvedType;
    }

    std::vector<std::string> argTypes;
    for (auto& arg : newExpr->arguments) argTypes.push_back(analyzeExpression(arg.get()));
    FunctionDefNode* ctor = resolveMethodOverload(candidates, argTypes, newExpr->className, newExpr->line);
    if (!ctor) {
        newExpr->resolvedType = "unknown";
        return newExpr->resolvedType;
    }
    newExpr->resolvedParamTypes.clear();
    for (auto& p : ctor->parameters) newExpr->resolvedParamTypes.push_back(p.first);
    newExpr->resolvedType = newExpr->className;
    return newExpr->resolvedType;
}

std::vector<FunctionDefNode*> TypeChecker::findConstructors(ClassDefNode* cls) {
    std::vector<FunctionDefNode*> found;
    if (cls->methods) {
        for (auto& m : cls->methods->methods) {
            auto* f = static_cast<FunctionDefNode*>(m.get());
            if (f->isConstructor) found.push_back(f);
        }
    }
    if (!found.empty()) return found;
    if (!cls->parentName.empty()) {
        auto it = classes.find(cls->parentName);
        if (it != classes.end()) return findConstructors(it->second);
    }
    return found;
}

std::string TypeChecker::checkSuperCall(ASTNode* node) {
    auto* call = static_cast<CallExprNode*>(node);

    if (currentClass.empty() || !classes.count(currentClass) || classes.at(currentClass)->parentName.empty()) {
        diag.report("ERROR386", call->line, 1, "", "");
        
        call->resolvedType = "unknown";
        return call->resolvedType;
    }
    std::string parentName = classes.at(currentClass)->parentName;

    if (call->callee == "super") {
        if (!inConstructor) {
            diag.report("ERROR387", call->line, 1, "", "");
            
            call->resolvedType = "unknown";
            return call->resolvedType;
        }
        std::vector<FunctionDefNode*> candidates = findConstructors(classes.at(parentName));
        if (candidates.empty()) {
            if (!call->arguments.empty()) {
                diag.report("ERROR388", call->line, 1, "", currentClass);
                
            }
            for (auto& arg : call->arguments) analyzeExpression(arg.get());
            call->resolvedParamTypes.clear();
            call->resolvedType = "void";
            return call->resolvedType;
        }
        std::vector<std::string> argTypes;
        for (auto& arg : call->arguments) argTypes.push_back(analyzeExpression(arg.get()));
        FunctionDefNode* ctor = resolveMethodOverload(candidates, argTypes, "super(...)", call->line);
        if (!ctor) { call->resolvedType = "unknown"; return call->resolvedType; }
        call->resolvedParamTypes.clear();
        for (auto& p : ctor->parameters) call->resolvedParamTypes.push_back(p.first);
        call->resolvedType = "void";
        return call->resolvedType;
    }

    std::string method = call->callee.substr(6);
    std::vector<FunctionDefNode*> candidates = getInheritedMethods(classes.at(parentName), method);
    if (candidates.empty()) {
        diag.report("ERROR389", call->line, 1, "", method + "' found in parent chain of '" + currentClass);
        
        call->resolvedType = "unknown";
        return call->resolvedType;
    }
    std::vector<std::string> argTypes;
    for (auto& arg : call->arguments) argTypes.push_back(analyzeExpression(arg.get()));
    FunctionDefNode* methodDef = resolveMethodOverload(candidates, argTypes, method, call->line);
    if (!methodDef) { call->resolvedType = "unknown"; return call->resolvedType; }
    if (methodDef->access == AccessSpecifier::PRIVATE) {
        diag.report("ERROR390", call->line, 1, "", method);
        
        call->resolvedType = "unknown";
        return call->resolvedType;
    }
    call->resolvedParamTypes.clear();
    for (auto& p : methodDef->parameters) call->resolvedParamTypes.push_back(p.first);
    call->resolvedType = methodDef->returnType.empty() ? "void" : methodDef->returnType;
    return call->resolvedType;
}

std::string TypeChecker::checkRefExpr(ASTNode* node) {
    auto* refNode = static_cast<RefExprNode*>(node);
    if (refNode->target->type != NodeType::IDENT && refNode->target->type != NodeType::INDEX_EXPR) {
        diag.report("ERROR391", refNode->line, 1, "", "");
        
        refNode->resolvedType = "unknown";
        return refNode->resolvedType;
    }
    std::string targetType = analyzeExpression(refNode->target.get());
    refNode->resolvedType = targetType;
    return refNode->resolvedType;
}

std::string TypeChecker::resolveFieldType(ClassDefNode* cls, const std::string& fieldName) {
    ASTNode* field = findField(cls, fieldName);
    if (!field) return "";
    if (field->type == NodeType::VAR_DECL) return static_cast<VarDeclNode*>(field)->varType;
    if (field->type == NodeType::ARRAY_DECL) return "array<" + static_cast<ArrayDeclNode*>(field)->elementType + ">";
    return "";
}

ASTNode* TypeChecker::findField(ClassDefNode* cls, const std::string& fieldName) {
    if (cls->attributes) {
        for (auto& attrNode : cls->attributes->attributes) {
            if (attrNode->type == NodeType::VAR_DECL) {
                auto* field = static_cast<VarDeclNode*>(attrNode.get());
                if (field->name == fieldName) return field;
            } else if (attrNode->type == NodeType::ARRAY_DECL) {
                auto* field = static_cast<ArrayDeclNode*>(attrNode.get());
                if (field->name == fieldName) return field;
            }
        }
    }
    if (!cls->parentName.empty()) {
        auto it = classes.find(cls->parentName);
        if (it != classes.end()) return findField(it->second, fieldName);
    }
    return nullptr;
}

std::vector<FunctionDefNode*> TypeChecker::findMethod(ClassDefNode* cls, const std::string& methodName) {
    std::vector<FunctionDefNode*> foundMethod;
    if (cls->methods) {
        for (auto& m : cls->methods->methods) {
            auto* f = static_cast<FunctionDefNode*>(m.get());
            if (f->isConstructor || f->isDestructor) continue;
            if (f->name == methodName) foundMethod.push_back(f);
        }
    }
    if(!foundMethod.empty()){
        return foundMethod;
    }
    if (!cls->parentName.empty()) {
        auto it = classes.find(cls->parentName);
        if (it != classes.end()) return findMethod(it->second, methodName);
    }
    return foundMethod;
}

bool TypeChecker::sameMethodSignature(FunctionDefNode* a,FunctionDefNode* b) {
    if (a==nullptr || b== nullptr) {
        return false;
    }
    if (a->name != b->name) {
        return false;
    }
    if (a->parameters.size() != b->parameters.size()) {
        return false;
    }
    for (size_t i=0; i < a->parameters.size(); ++i) {
        if (a->parameters[i].first != b->parameters[i].first) {
            return false;
        }
    }
    return true;
}

std::vector<FunctionDefNode*> TypeChecker::getInheritedMethods(ClassDefNode* cls,const std::string& methodName) {
    std::vector<FunctionDefNode*> result;
    if (cls == nullptr) {
        return result;
    }
    std::vector<FunctionDefNode*> currentMethods;
    if (cls->methods) {
        for (auto& m : cls->methods->methods) {
            auto* f = static_cast<FunctionDefNode*>(m.get());
            if (f->isConstructor || f->isDestructor) {
                continue;
            }
            if (f->name == methodName) {
                currentMethods.push_back(f);
                result.push_back(f);
            }
        }
    }
    if (cls->parentName.empty()) {
        return result;
    }
    auto parentIt = classes.find(cls->parentName);
    if (parentIt == classes.end()) {
        return result;
    }
    ClassDefNode* parent = parentIt->second;
    std::vector<FunctionDefNode*> parentMethods =getInheritedMethods(parent, methodName);
    for (auto* parentMethod : parentMethods) {
        bool overridden = false;
        for (auto* currentMethod : currentMethods) {
            if (sameMethodSignature(currentMethod, parentMethod)) {
                overridden = true;
                break;
            }
        }
        if (!overridden) {
            result.push_back(parentMethod);
        }
    }
    return result;
}
