
#include "typechecker.h"

#include "../parser/ast/expressions.h"



std::string TypeChecker::checkTernaryExpr(ASTNode* node) {
    auto* tern = static_cast<TernaryExprNode*>(node);
    std::string condType = analyzeExpression(tern->condition.get());
    if (condType != "bool") {
        diag.report("ERROR355", tern->line, 1, "", condType);
        
        tern->resolvedType = "unknown";
        return tern->resolvedType;
    }

    std::string tType = analyzeExpression(tern->trueExpr.get());
    std::string fType = analyzeExpression(tern->falseExpr.get());

    std::string resolved = resolveNumericPromotion(tType, fType);
    if (resolved.empty()) {
        diag.report("ERROR356", tern->line, 1, "", tType + "' vs '" + fType);
        
        tern->resolvedType = "unknown";
        return tern->resolvedType;
    }

    tern->resolvedType = resolved;
    return tern->resolvedType;
}



std::string TypeChecker::checkBetweenExpr(ASTNode* node) {
    auto* between = static_cast<BetweenExprNode*>(node);
    std::string valueType = analyzeExpression(between->value.get());
    std::string lowerType = analyzeExpression(between->lower.get());
    std::string upperType = analyzeExpression(between->upper.get());

    auto validType = [](const std::string& type){
        return type == "int" || type == "bigint" || type == "float" || type == "double" || type == "char";
    };

    if(!validType(valueType) || !validType(lowerType) || !validType(upperType)){
        diag.report("ERROR365", between->line, 1, "", "");
        
        between->resolvedType = "unknown";
        return between->resolvedType;
    }

    std::string dominentType = "int";
    if (valueType == "double" || lowerType=="double" || upperType=="double") dominentType= "double";
    else if (valueType == "float" || lowerType=="float" || upperType=="float") dominentType = "float";
    else if (valueType == "bigint" || lowerType=="bigint" || upperType=="bigint") dominentType = "bigint";

    between->resolvedType = dominentType;
    return between->resolvedType;
}