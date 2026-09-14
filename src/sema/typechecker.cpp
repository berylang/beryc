#include "typechecker.h"

/*

    Bery Type Checker,
    
    it divides type checking into smaller parts, by spliting them based on their
    node type.

*/

#include "../parser/ast/expressions.h"
#include "../parser/ast/literals.h"
#include "../parser/ast/arraydeclare.h"
#include "../parser/ast/functions.h"
#include "../parser/ast/vardecl.h"
#include "../parser/ast/classes.h"
#include <iostream>
#include <unordered_set>


TypeChecker::TypeChecker(SymbolTable& symbolTable, std::unordered_map<std::string, std::vector<FunctionSignature>>& funcs,std::unordered_map<std::string, ClassDefNode*>& classesMap, std::string& currentClassRef, bool& inConstructorRef, DiagnosticEngine& diagref) 
    : symbolTable(symbolTable), functions(funcs), classes(classesMap), currentClass(currentClassRef), inConstructor(inConstructorRef), diag(diagref) {}

bool TypeChecker::typeMatchesLiteral(const std::string& type, NodeType litType) {
   if (type == "int"    && litType == NodeType::INT_LIT)     return true;
   if (type == "bigint" && litType == NodeType::INT_LIT)     return true;
   if (type == "float"  && litType == NodeType::DECIMAL_LIT) return true;
   if (type == "bool"   && litType == NodeType::BOOL_LIT)    return true;
   if (type == "double" && litType == NodeType::DECIMAL_LIT) return true;
   if (type == "char"   && litType == NodeType::CHAR_LIT)    return true;
   if (type == "string" && litType == NodeType::STRING_LIT)  return true;
   if (type == "string" && litType == NodeType::NULL_LIT)    return true;
   return false;
}

std::string TypeChecker::analyzeExpression(ASTNode* node) {
    switch (node->type) {
        case NodeType::BINARY_EXPR:     return checkBinaryExpr(node);
        case NodeType::UNARY_EXPR:      return checkUnaryExpr(node);
        case NodeType::TERNARY_EXPR:    return checkTernaryExpr(node);
        case NodeType::BETWEEN_EXPR:    return checkBetweenExpr(node);
        case NodeType::CALL_EXPR:       return checkCallExpr(node);
        case NodeType::INDEX_EXPR:      return checkIndexExpr(node);
        case NodeType::ASSIGNMENT_EXPR: return checkAssignmentExpr(node);
        case NodeType::CAST_EXPR:       return checkCastExpr(node);
        case NodeType::IDENT:           return checkIdentifier(node);
        case NodeType::NEW_EXPR:        return checkNewExpr(node);
        case NodeType::REF_EXPR:        return checkRefExpr(node);
        default:                        return checkLiteral(node);
    }
}





std::string TypeChecker::checkCastExpr(ASTNode* node) {
    auto* castNode = static_cast<CastExprNode*>(node);
    std::string srcType = analyzeExpression(castNode->expr.get());
    castNode->srcType = srcType; 

    auto isPrimitive = [](const std::string& t) {
        return t == "int" || t == "bigint" || t == "float" || t == "double" || t == "char" || t == "bool";
    };

    if (!isPrimitive(srcType) || !isPrimitive(castNode->targetType)) {
        diag.report("ERROR382", castNode->line, 1, "", srcType + "' to '" + castNode->targetType);
        
        castNode->resolvedType = "unknown";
        return castNode->resolvedType;
    }
    castNode->resolvedType = castNode->targetType;
    return castNode->resolvedType;
}

std::string TypeChecker::checkLiteral(ASTNode* node) {
    switch (node->type) {
        case NodeType::INT_LIT:     
            node->resolvedType = "int";
            return node->resolvedType;
        case NodeType::DECIMAL_LIT:     
            node->resolvedType = "float";
            return node->resolvedType;
        case NodeType::CHAR_LIT:     
            node->resolvedType = "char";
            return node->resolvedType;
        case NodeType::BOOL_LIT:     
            node->resolvedType = "bool";
            return node->resolvedType;
        case NodeType::STRING_LIT:     
            node->resolvedType = "string";
            return node->resolvedType;  
        case NodeType::NULL_LIT:     
            node->resolvedType = "null";
            return node->resolvedType;
        default:
            diag.report("ERROR383", node->line, 1, "", "");
            node->resolvedType = "unknown";
            return node->resolvedType;
    }
}
