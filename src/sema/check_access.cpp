#include "typechecker.h"

#include "../parser/ast/expressions.h"
#include "../parser/ast/arraydeclare.h"
#include "../parser/ast/vardecl.h"
#include "../parser/ast/literals.h"
#include "../parser/ast/classes.h"
#include <iostream>


std::string TypeChecker::checkIndexExpr(ASTNode* node) {
    auto* idxNode = static_cast<IndexExprNode*>(node);
    std::string arrType;
    int dimCount = 0;

    size_t dot = idxNode->name.find('.');
    if (dot != std::string::npos) {
        std::vector<std::string> parts = splitDots(idxNode->name);
        arrType = resolveChainType(parts, idxNode->loc());
        if (arrType == "unknown") {
            idxNode->resolvedType = "unknown";
            return idxNode->resolvedType;
        }
        std::vector<std::string> headParts(parts.begin(), parts.end() - 1);
        std::string classType = resolveChainType(headParts, idxNode->loc());
        auto classIt = classes.find(classType);
        if (classIt != classes.end()) {
            ASTNode* field = findField(classIt->second, parts.back());
            if (field && field->type == NodeType::ARRAY_DECL) {
                dimCount = (int)static_cast<ArrayDeclNode*>(field)->dimensions.size();
            } else {
                dimCount = 1;
            }
        } else {
            dimCount = 1;
        }
    } else {
        if (!symbolTable.exists(idxNode->name)) {
            diag.report("ERROR372", idxNode->line, idxNode->column, idxNode->length, idxNode->name);
            
            idxNode->resolvedType = "unknown";
            return idxNode->resolvedType;
        }
        Symbol& sym = symbolTable.get(idxNode->name);
        arrType = sym.type;
        dimCount = (int)sym.arrayDimensions.size();
    }

    if (arrType == "string") {
        for (auto& index : idxNode->indices) {
            std::string indexType = analyzeExpression(index.get());
            if (indexType != "int" && indexType != "bigint") {
                diag.report("ERROR373", idxNode->line, idxNode->column, idxNode->length, "");
                
                idxNode->resolvedType = "unknown";
                return idxNode->resolvedType;
            }
        }
        idxNode->resolvedType = "char";
        return idxNode->resolvedType;
    }

    if (!(arrType.size() > 6 && arrType.substr(0, 6) == "array<")) {
        diag.report("ERROR374", idxNode->line, idxNode->column, idxNode->length, idxNode->name);
        
        idxNode->resolvedType = "unknown";
        return idxNode->resolvedType;
    }

    if (idxNode->indices.size() > (size_t)dimCount && dimCount > 0) {
        diag.report("ERROR375", idxNode->line, idxNode->column, idxNode->length, idxNode->name);
        
        idxNode->resolvedType = "unknown";
        return idxNode->resolvedType;
    }

    for (auto& index : idxNode->indices) analyzeExpression(index.get());
    std::string elemType = arrType.substr(6, arrType.size() - 7);

    if (!idxNode->memberChain.empty()) {
        idxNode->resolvedType = resolveFieldChainFrom(elemType, idxNode->memberChain, idxNode->loc());
        return idxNode->resolvedType;
    }

    idxNode->resolvedType = elemType;
    return idxNode->resolvedType;
}
std::string TypeChecker::checkAssignmentExpr(ASTNode* node) {
    auto* assign = static_cast<AssignmentExprNode*>(node);
    std::string targetName = "";
    std::string targetType = analyzeExpression(assign->target.get());
    std::string valueType  = analyzeExpression(assign->value.get());

    if (targetType == "unknown") {
        assign->resolvedType = "unknown";
        return assign->resolvedType;
    }
    if (assign->op == "+=") {
        if (targetType != "int" && targetType != "float" && targetType != "double" && targetType != "bigint" && targetType != "string") {
            diag.report("ERROR376", assign->line, assign->column, assign->length, assign->op + "' on type '" + targetType);
            
        }
    }
    else if (assign->op != "=") {
        if (targetType != "int" && targetType != "float" && targetType != "double" && targetType != "bigint") {
            diag.report("ERROR376", assign->line, assign->column, assign->length, assign->op + "' on type '" + targetType);
            
        }
    }
    
    if (assign->target->type == NodeType::IDENT) {
        auto* ident = static_cast<IdentNode*>(assign->target.get());
        targetName = ident->name;
        size_t dot = ident->name.find('.');
        if (dot != std::string::npos) {
            std::vector<std::string> parts = splitDots(ident->name);
            std::vector<std::string> headParts(parts.begin(), parts.end() - 1);
            std::string fieldName = parts.back();
            std::string containerType = resolveChainType(headParts, assign->loc());
            if (containerType == "unknown") { assign->resolvedType = "unknown"; return assign->resolvedType; }
            auto classIt = classes.find(containerType);
            if (classIt == classes.end()) {
                diag.report("ERROR377", assign->line, assign->column, assign->length, headParts.back());
                
                assign->resolvedType = "unknown";
                return assign->resolvedType;
            }
            std::string fieldType = resolveFieldType(classIt->second, fieldName);
            if (fieldType.empty()) {
                diag.report("ERROR378", assign->line, assign->column, assign->length, std::vector<std::string>{containerType, fieldName});
                assign->resolvedType = "unknown";
                return assign->resolvedType;
            }
            ASTNode* field = findField(classIt->second, fieldName);
            if (!field) {
                diag.report("ERROR378", assign->line, assign->column, assign->length, std::vector<std::string>{containerType, fieldName});
                assign->resolvedType = "unknown";
                return assign->resolvedType;
            }

            AccessSpecifier acc = (field->type == NodeType::VAR_DECL) ? static_cast<VarDeclNode*>(field)->access: static_cast<ArrayDeclNode*>(field)->access;

            if (!checkMemberAccess(acc, containerType, fieldName, "field", assign->loc())) {
                assign->resolvedType = "unknown";
                return assign->resolvedType;
            }
            targetType = fieldType; 
        } else {



            if (!symbolTable.exists(ident->name)) {
                
                diag.report("ERROR003", ident->line, ident->column, ident->length, ident->name);
                
                ident->resolvedType = "unknown";
                return ident->resolvedType;
            }
            Symbol& s = symbolTable.get(ident->name);
            if (s.isConst) {
                diag.report("ERROR379", assign->line, assign->column, assign->length, ident->name);
                
                ident->resolvedType = "unknown";
                return ident->resolvedType;
            }
            s.isInitialized = true;
            targetType = s.type;
        }
    } else if (assign->target->type == NodeType::INDEX_EXPR) {
        auto* idxNode = static_cast<IndexExprNode*>(assign->target.get());
        size_t dot = idxNode->name.find('.');
        if (dot != std::string::npos) {
            std::vector<std::string> parts = splitDots(idxNode->name);
            std::string arrType = resolveChainType(parts, idxNode->loc());
            if (arrType == "unknown") {
                assign->resolvedType = "unknown";
                return assign->resolvedType;
            }
            targetName = idxNode->name;
        } else {
            if (!symbolTable.exists(idxNode->name)) {
                diag.report("ERROR372", idxNode->line, idxNode->column, idxNode->length, idxNode->name);
                
                assign->resolvedType = "unknown";
                return assign->resolvedType;
            }
            targetName = idxNode->name;
        }
    } else {
        diag.report("ERROR380", assign->line, assign->column, assign->length, "");
        
        assign->resolvedType = "unknown";
        return assign->resolvedType;
    }

    std::string exptype = valueType;
    
    if (exptype != "unknown" && exptype != targetType) {
        if (!(targetType == "float" && exptype == "int") &&  !(targetType == "double" && exptype == "int") &&
            !(targetType == "bigint" && exptype == "int") &&  !(targetType == "double" && exptype == "float")) {
            
            diag.report("ERROR381", assign->line, assign->column, assign->length, {targetName,targetType,exptype});
            
            assign->resolvedType = "unknown";
            return assign->resolvedType;
        }
    }
    assign->resolvedType = targetType;
    return assign->resolvedType;
}


std::string TypeChecker::checkIdentifier(ASTNode* node) {
    auto* ident = static_cast<IdentNode*>(node);
    size_t dot = ident->name.find('.');
    bool isModuleGlobalVar = (dot != std::string::npos) && symbolTable.exists(ident->name);
    if (dot != std::string::npos && !isModuleGlobalVar) {
        std::vector<std::string> parts = splitDots(ident->name);
        if (parts.back() == "len") {
            std::vector<std::string> headParts(parts.begin(), parts.end() - 1);
            std::string headType = resolveChainType(headParts, ident->loc());
            if (headType == "unknown") { 
                node->resolvedType = "unknown"; 
                return node->resolvedType; 
            
            }
            if (headType == "string" ||(headType.size() > 6 && headType.substr(0, 6) == "array<")) {
                ident->resolvedType = "int";
                return ident->resolvedType;
            }
        }
        ident->resolvedType = resolveChainType(parts, ident->loc());
        return ident->resolvedType;
    }
    if(!symbolTable.exists(ident->name)){
        diag.report("ERROR003", ident->line, ident->column, ident->length, ident->name);
        
        node->resolvedType = "unknown";
        return node->resolvedType;
    }
    ident->resolvedType = symbolTable.get(ident->name).type;
    return ident->resolvedType;
}


bool TypeChecker::checkMemberAccess(AccessSpecifier access, const std::string& className, const std::string& memberName, const std::string& type, SourceLoc loc) {
    if (access == AccessSpecifier::PUBLIC) return true;
    if (currentClass == className) return true;
    if (access == AccessSpecifier::PROTECTED && !currentClass.empty()) {
        std::string cur = currentClass;
        while (!cur.empty()) {
            if (cur == className) return true;
            auto it = classes.find(cur);
            cur = (it != classes.end()) ? it->second->parentName : "";
        }
    }
    std::string levelName = (access == AccessSpecifier::PRIVATE) ? "private" : "protected";
    diag.report("ERROR398", loc.line, loc.column, loc.length, levelName + " " + type + " '" + memberName + "' of class '" + className);
    return false;
}

std::string TypeChecker::resolveChainType(const std::vector<std::string>& parts, SourceLoc loc){
    std::string currentType;
    if (parts[0] == "super") {
        if (currentClass.empty() || !classes.count(currentClass) || classes.at(currentClass)->parentName.empty()) {
            diag.report("ERROR386", loc.line, loc.column, loc.length, "");
            return "unknown";
        }
        currentType = classes.at(currentClass)->parentName;
    } else {
        if (!symbolTable.exists(parts[0])) {
            diag.report("ERROR399", loc.line, loc.column, loc.length, parts[0]);
            return "unknown";
        }
        currentType = symbolTable.get(parts[0]).type;
    }
    std::vector<std::string> rest(parts.begin() + 1, parts.end());
    return resolveFieldChainFrom(currentType, rest, loc);
}


std::string TypeChecker::resolveFieldChainFrom(std::string currentType, const std::vector<std::string>& parts, SourceLoc loc) {
    for (size_t i = 0; i < parts.size(); ++i) {
        auto classIt = classes.find(currentType);
        if (classIt == classes.end()) {
            diag.report("ERROR400", loc.line, loc.column, loc.length, std::vector<std::string>{currentType, parts[i]});
            return "unknown";
        }
        ASTNode* field = findField(classIt->second, parts[i]);
        if (!field) {
            diag.report("ERROR378", loc.line, loc.column, loc.length, std::vector<std::string>{currentType, parts[i]});
            return "unknown";
        }
        AccessSpecifier acc = (field->type == NodeType::VAR_DECL)
            ? static_cast<VarDeclNode*>(field)->access
            : static_cast<ArrayDeclNode*>(field)->access;

        if (!checkMemberAccess(acc, currentType, parts[i], "field", loc)) return "unknown";

        if (field->type == NodeType::VAR_DECL) currentType = static_cast<VarDeclNode*>(field)->varType;
        else currentType = "array<" + static_cast<ArrayDeclNode*>(field)->elementType + ">";
    }
    return currentType;
}