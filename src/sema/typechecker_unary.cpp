
#include "typechecker.h"

#include "../parser/ast/expressions.h"
#include "../parser/ast/literals.h"
#include "../parser/ast/arraydeclare.h"

std::string TypeChecker::checkUnaryExpr(ASTNode* node) {
    auto* unary = static_cast<UnaryExprNode*>(node);

    if(unary->optr == "delete"){

        std::string objectType;

        if (unary->operand->type == NodeType::CALL_EXPR) {
            objectType = checkCallExpr(unary->operand.get());
        }
        else if (unary->operand->type == NodeType::INDEX_EXPR) {
            objectType = checkIndexExpr(unary->operand.get());
        }
        else if (unary->operand->type == NodeType::IDENT) {
            auto* ident = static_cast<IdentNode*>(unary->operand.get());
            size_t dot = ident->name.find('.');

            if(dot != std::string::npos){
                std::vector<std::string> parts = splitDots(ident->name);
                std::vector<std::string> headParts(parts.begin(), parts.end() - 1);
                std::string containerType = resolveChainType(headParts, unary->loc());

                if(containerType == "unknown")return unary->resolvedType = "unknown";

                auto classIt = classes.find(containerType);
                if(classIt == classes.end()) diag.report("ERROR357", unary->line, unary->column, unary->length, "");

                ASTNode* field = findField(classIt->second, parts.back());
                if(!field) diag.report("ERROR358", unary->line, unary->column, unary->length, "");
                AccessSpecifier access = (field->type == NodeType::VAR_DECL)? static_cast<VarDeclNode*>(field)->access : static_cast<ArrayDeclNode*>(field)->access;
                if(!checkMemberAccess(access, containerType, parts.back(), "field", unary->loc())) return unary->resolvedType = "unknown";

                if(field->type == NodeType::VAR_DECL && static_cast<VarDeclNode*>(field)->isConst) diag.report("ERROR359", unary->line, unary->column, unary->length, "");
                objectType = resolveChainType(parts, unary->loc());
                if(objectType == "unknown") return unary->resolvedType = "unknown";
            }
            else{
                if(!symbolTable.exists(ident->name)) diag.report("ERROR360", unary->line, unary->column, unary->length, "");
                Symbol& symbol = symbolTable.get(ident->name);
                if(symbol.isConst) diag.report("ERROR361", unary->line, unary->column, unary->length, "");

                objectType = symbol.type;
            }
        }
        else {
            diag.report("ERROR362", unary->line, unary->column, unary->length, "");
            
            unary->resolvedType = "unknown";
            return unary->resolvedType;
        }

        if(!classes.count(objectType))
            diag.report("ERROR363", unary->line, unary->column, unary->length, "");

        unary->resolvedType = "";
        return unary->resolvedType;
    }
    std::string optype = analyzeExpression(unary->operand.get());
    if(unary->optr=="++"||unary->optr=="--"||unary->optr=="post++"||unary->optr=="post--"){
        if(unary->operand->type != NodeType::IDENT && unary->operand->type != NodeType::INDEX_EXPR){
            diag.report("ERROR364", unary->line, unary->column, unary->length, "");
            
            unary->resolvedType = "unknown";
            return unary->resolvedType;
        }
    }
    if(unary->optr == "!"){
        unary->resolvedType = "bool";
        return unary->resolvedType;
    }
    unary->resolvedType = optype;
    return unary->resolvedType;
}
