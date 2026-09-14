#include "typechecker.h"

#include "../parser/ast/expressions.h"
#include "../parser/ast/vardecl.h"
#include "../parser/ast/classes.h"
#include <iostream>

std::string TypeChecker::resolveNumericPromotion(const std::string& lType, const std::string& rType) {
    if (lType == rType) return lType;
    if ((lType == "bigint" && rType == "int")    || (lType == "int"    && rType == "bigint")) return "bigint";
    if ((lType == "float"  && rType == "int")    || (lType == "int"    && rType == "float"))  return "float";
    if ((lType == "bigint" && rType == "float")  || (lType == "float"  && rType == "bigint")) return "float";
    if ((lType == "bigint" && rType == "double") || (lType == "double" && rType == "bigint")) return "double";
    if ((lType == "int"    && rType == "double") || (lType == "double" && rType == "int"))    return "double";
    if ((lType == "float"  && rType == "double") || (lType == "double" && rType == "float"))  return "double";
    return "";
}


std::string TypeChecker::checkBinaryExpr(ASTNode* node) {
    auto* binary = static_cast<BinaryExprNode*>(node);
    std::string lType = analyzeExpression(binary->left.get());
    std::string rType = analyzeExpression(binary->right.get());

    if (binary->optr == "+") {
        if (lType == "string" || rType == "string") {
            if((lType != "string" && lType != "int" && lType != "bigint" && lType != "float" && lType != "double" && lType != "char" && lType != "bool") 
                ||  (rType != "string" && rType != "int" && rType != "bigint" && rType != "float" && rType != "double" && rType != "char" && rType != "bool")){
                diag.report("ERROR348", binary->line, 1, "", "");
                
                binary->resolvedType = "unknown";
                return binary->resolvedType; 
            }
            if(lType != "string"){
                auto cast = std::make_unique<CastExprNode>("string",std::move(binary->left), binary->line);
                cast->srcType = lType;
                binary->left = std::move(cast);

            }
            if(rType != "string"){
                auto cast = std::make_unique<CastExprNode>("string",std::move(binary->right), binary->line);
                cast->srcType = rType;
                binary->right = std::move(cast);

            }
            binary->resolvedType = "string";
            return binary->resolvedType;
        }
        
    }
    if (lType == "string" && rType == "string") {
        if (binary->optr == "==" || binary->optr == "!=") {
            binary->resolvedType = "bool";
            return binary->resolvedType;
        }
    }

    if (binary->optr == "&&" || binary->optr == "||") {
        if (lType != "bool" || rType != "bool") {
            diag.report("ERROR349", binary->line, 1, "", binary->optr + "' cannot be used on type '" + lType + "' and '" + rType);
            
            binary->resolvedType = "unknown";
            return binary->resolvedType;
        }
        binary->resolvedType = "bool";
        return binary->resolvedType;
    }

    std::string resolved = resolveNumericPromotion(lType, rType);
    if (resolved.empty()) {
        diag.report("ERROR350", binary->line, 1, "", lType + "' and '" + rType);
        
        binary->resolvedType = "unknown";
        return binary->resolvedType;
    }

    if (binary->optr == "==" || binary->optr == "!=" ||
        binary->optr == ">"  || binary->optr == ">=" ||
        binary->optr == "<"  || binary->optr == "<=") {
        if (binary->optr != "==" && binary->optr != "!=") {
            if (lType == "string" || lType == "bool" || rType == "string" || rType == "bool") {
                diag.report("ERROR351", binary->line, 1, "", binary->optr + "' cannot be used on type '" + lType + "' and '" + rType);
                
                binary->resolvedType = "unknown";
                return binary->resolvedType;
            }
        }
        binary->resolvedType = "bool";
        return binary->resolvedType;
    }

    if (binary->optr == "<<" || binary->optr == ">>") {
        if (rType != "int" && rType != "bigint") {
            diag.report("ERROR352", binary->line, 1, "", "");
            
            binary->resolvedType = "unknown";
            return binary->resolvedType;
        }
        if (resolved != "int" && resolved != "bigint") {
            diag.report("ERROR353", binary->line, 1, "", "");
            
            binary->resolvedType = "unknown";
            return binary->resolvedType;
        }
        binary->resolvedType = resolved;
        return binary->resolvedType;
    }

    if (binary->optr == "&" || binary->optr == "^" || binary->optr == "|") {
        if ((lType != "int" && lType != "bigint") || (rType != "int" && rType != "bigint")) {
            diag.report("ERROR354", binary->line, 1, "", "");
            
            binary->resolvedType = "unknown";
            return binary->resolvedType;
        }
        binary->resolvedType = resolved;
        return binary->resolvedType;
    }
    binary->resolvedType = resolved;
    return binary->resolvedType;
}

