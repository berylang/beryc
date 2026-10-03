#include "parser.h"
/*
    In Bery, each expression is made up of smaller expressions.

    the precedence of operators in Bery : (lowese to highest):
    01. ternary
    02. logical
    03. equality
    04. relational
    05. between
    06. bitwise
    07. shift
    08. additives
    09. multiplicative
    10. unary
    11. postfix
    12. primary

    the parsing flow goes from lowest to highest. and because 12th level (primary) level can have
    grouped expressions it should call the praseExpression() again, making it -
        Recursive Descent Parsing
*/

#include "ast/literals.h"
#include <stdexcept>
#include <iostream>
#include "ast/expressions.h"


std::unique_ptr<ASTNode> Parser::parseExpression(){
    auto expr = parseTernary();
    auto isassignmentOperator = [](TokenType t){
        return t==TokenType::TOKEN_EQUAL || t== TokenType::TOKEN_ADD_ASSIGN || t== TokenType::TOKEN_SUB_ASSIGN
        || t== TokenType::TOKEN_MUL_ASSIGN || t== TokenType::TOKEN_DIV_ASSIGN  || t== TokenType::TOKEN_DSTAR_ASSIGN
        || t== TokenType::TOKEN_MODULE_ASSIGN || t== TokenType::TOKEN_AND_ASSIGN || t== TokenType::TOKEN_OR_ASSIGN
        || t== TokenType::TOKEN_XOR_ASSIGN || t== TokenType::TOKEN_LSHIFT_ASSIGN || t== TokenType::TOKEN_RSHIFT_ASSIGN ;
    };
    if (isassignmentOperator(peek().type)) {
        SourceLoc start = expr->whole();
        Token optoken = advance();
        auto value = parseExpression();

        if (expr->type == NodeType::IDENT || expr->type == NodeType::INDEX_EXPR) {
            auto node = withLoc(std::make_unique<AssignmentExprNode>( std::move(expr), std::move(value), optoken.lexeme, optoken.line), optoken);
            spanFrom(node.get(), start, previous());
            return node;
        }
        diag.report("ERROR211", optoken.line, optoken.column, optoken.length);
        errors = true;
        throw ParseError();
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseTernary() {
    auto expr = parseLogicalOr();
    if (check(TokenType::TOKEN_QUESTION)) {
        SourceLoc start = expr->whole();
        Token opTok = advance();
        auto trueExpr = parseExpression();
        consume(TokenType::TOKEN_COLON, "ERROR212");
        auto falseExpr = parseTernary();
        auto node = withLoc(std::make_unique<TernaryExprNode>(std::move(expr), std::move(trueExpr), std::move(falseExpr), opTok.line), opTok);
        spanFrom(node.get(), start, previous());
        return node;
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseLogicalOr(){
    auto left = parseLogicalAnd();
    while(check(TokenType::TOKEN_OR)){
        SourceLoc start = left->whole();
        Token opTok = advance();
        std::string optr = opTok.lexeme;
        auto right = parseLogicalAnd();
        auto node = withLoc(std::make_unique<BinaryExprNode>(optr, std::move(left), std::move(right), opTok.line), opTok);
        spanFrom(node.get(), start, previous());
        left = std::move(node);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseLogicalAnd(){
    auto left = parseEquality();
    while(check(TokenType::TOKEN_AND)){
        SourceLoc start = left->whole();
        Token opTok = advance();
        std::string optr = opTok.lexeme;
        auto right = parseEquality();
        auto node = withLoc(std::make_unique<BinaryExprNode>(optr, std::move(left), std::move(right), opTok.line), opTok);
        spanFrom(node.get(), start, previous());
        left = std::move(node);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseEquality(){
    auto left = parseRelational();
    while(check(TokenType::TOKEN_EQUAL_EQUAL)|| check(TokenType::TOKEN_NOT_EQUAL)){
        SourceLoc start = left->whole();
        Token opTok = advance();
        std::string optr = opTok.lexeme;
        auto right = parseRelational();
        auto node = withLoc(std::make_unique<BinaryExprNode>(optr, std::move(left), std::move(right), opTok.line), opTok);
        spanFrom(node.get(), start, previous());
        left = std::move(node);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseRelational(){
    auto left =parseBetween();
   
    while(check(TokenType::TOKEN_GTHAN)|| check(TokenType::TOKEN_LTHAN)||check(TokenType::TOKEN_GTEQUAL)||check(TokenType::TOKEN_LTEQUAL)){
        SourceLoc start = left->whole();
        Token opTok = advance();
        std::string optr = opTok.lexeme;
        auto right = parseBetween();
        auto node = withLoc(std::make_unique<BinaryExprNode>(optr, std::move(left), std::move(right), opTok.line), opTok);
        spanFrom(node.get(), start, previous());
        left = std::move(node);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseBetween(){
    auto value = parseBitwise();
    if (check(TokenType::TOKEN_BETWEEN) || check(TokenType::TOKEN_NOT_BETWEEN)) {
        SourceLoc start = value->whole();
        bool isNegated = check(TokenType::TOKEN_NOT_BETWEEN);
        Token opTok = advance();
        auto lower = parseBitwise();
        consume(TokenType::TOKEN_COMMA, "ERROR213");
        auto upper = parseBitwise();

        auto node = withLoc(std::make_unique<BetweenExprNode>(std::move(value), std::move(lower), std::move(upper), isNegated, opTok.line), opTok);
        spanFrom(node.get(), start, previous());
        return node;
    }
    return value;
}

std::unique_ptr<ASTNode> Parser::parseBitwise(){
    auto left = parseShift();

    while(check(TokenType::TOKEN_AMPERSAND) || check(TokenType::TOKEN_CARET) || check(TokenType::TOKEN_PIPE)){
        SourceLoc start = left->whole();
        Token opTok = advance();
        std::string optr = opTok.lexeme;
        auto right = parseShift();
        auto node = withLoc(std::make_unique<BinaryExprNode>(optr, std::move(left), std::move(right), opTok.line), opTok);
        spanFrom(node.get(), start, previous());
        left = std::move(node);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseShift(){
    auto left = parseAdditive();

    while(check(TokenType::TOKEN_LSHIFT) || check(TokenType::TOKEN_RSHIFT)){
        SourceLoc start = left->whole();
        Token opTok = advance();
        std::string optr = opTok.lexeme;
        auto right = parseAdditive();
        auto node = withLoc(std::make_unique<BinaryExprNode>(optr, std::move(left), std::move(right), opTok.line), opTok);
        spanFrom(node.get(), start, previous());
        left = std::move(node);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseAdditive(){
    auto left = parseMultiplicative();
    
    while (check(TokenType::TOKEN_PLUS) || check(TokenType::TOKEN_MINUS)) {
        SourceLoc start = left->whole();
        Token opTok = advance();
        std::string optr = opTok.lexeme;
        auto right = parseMultiplicative();
        auto node = withLoc(std::make_unique<BinaryExprNode>(optr, std::move(left), std::move(right), opTok.line), opTok);
        spanFrom(node.get(), start, previous());
        left = std::move(node);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseMultiplicative(){
    auto left = parseUnary();

    while(check(TokenType::TOKEN_STAR) || check(TokenType::TOKEN_FSLASH)||
        check(TokenType::TOKEN_PERCENT)|| check(TokenType::TOKEN_POWER)){
        SourceLoc start = left->whole();
        Token opTok = advance();
        std::string optr = opTok.lexeme;
        auto right = parseUnary();
        auto node = withLoc(std::make_unique<BinaryExprNode>(optr, std::move(left), std::move(right), opTok.line), opTok);
        spanFrom(node.get(), start, previous());
        left = std::move(node);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseUnary(){
    if (check(TokenType::TOKEN_LPARAN) &&
        current + 1 < (int)tokens.size() && isTypeToken(tokens[current + 1].type) &&
        current + 2 < (int)tokens.size() && tokens[current + 2].type == TokenType::TOKEN_RPARAN) {

        Token lp = advance();
        Token typeToken = advance();
        Token rp = advance();
        auto expr = parseUnary();

        auto node = std::make_unique<CastExprNode>(typeToken.lexeme, std::move(expr), lp.line);
        node->line   = lp.line;               // anchor = the "(int)" part
        node->column = lp.column;
        node->length = rp.column + rp.length - lp.column;
        spanFrom(node.get(), SourceLoc{lp.line, lp.column, 1}, previous());
        return node;
    }

    if (check(TokenType::TOKEN_BANG) || check(TokenType::TOKEN_TILDE) ||
        check(TokenType::TOKEN_INC)  || check(TokenType::TOKEN_DEC)   ||
        check(TokenType::TOKEN_MINUS)|| check(TokenType::TOKEN_DELETE)) {

        Token opTok = advance();
        std::string optr = opTok.lexeme;
        auto operand = parsePostfix();

        auto node = withLoc(std::make_unique<UnaryExprNode>(optr, std::move(operand), opTok.line), opTok);
        spanFrom(node.get(), SourceLoc{opTok.line, opTok.column, 1}, previous());
        return node;
    }

    return parsePostfix();
}

std::unique_ptr<ASTNode> Parser::parsePostfix(){
    auto expr = parsePrimary();

    if (check(TokenType::TOKEN_INC) || check(TokenType::TOKEN_DEC)) {
        SourceLoc start = expr->whole();
        Token opTok = advance();
        std::string optr = "post" + opTok.lexeme;
        auto node = withLoc(std::make_unique<UnaryExprNode>(optr, std::move(expr), opTok.line), opTok);
        spanFrom(node.get(), start, previous());
        return node;
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::parsePrimary(){
    Token t = peek();
    if (t.type == TokenType::TOKEN_REF) {
        advance();
        auto target = parsePrimary();
        return withLoc(std::make_unique<RefExprNode>(std::move(target), t.line), t);
    }
    if (t.type == TokenType::TOKEN_NEW) {
        advance();
        Token className = consume(TokenType::TOKEN_IDENT, "ERROR214");
        consume(TokenType::TOKEN_LPARAN, "ERROR215");
        std::vector<std::unique_ptr<ASTNode>> arguments;
        if (!check(TokenType::TOKEN_RPARAN)) {
            do { arguments.push_back(parseExpression()); } while (check(TokenType::TOKEN_COMMA) && (advance(), true));
        }
        consume(TokenType::TOKEN_RPARAN, "ERROR216");
        return withLoc(std::make_unique<NewExprNode>(className.lexeme, std::move(arguments), className.line), className);
    }
    if (t.type == TokenType::TOKEN_IDENT || t.type == TokenType::TOKEN_SUPER) {
        advance();
        std::string fullName = t.lexeme;
        Token lastTok = t;
        while (check(TokenType::TOKEN_DOT)) {
            advance();
            Token nextIdent = consume(TokenType::TOKEN_IDENT, "ERROR217");
            fullName += "." + nextIdent.lexeme;
            lastTok = nextIdent;
        }
        Token nameTok = t;
        nameTok.lexeme = fullName;
        nameTok.length = (lastTok.line == t.line) ? lastTok.column + lastTok.length - t.column : t.length;
        if (check(TokenType::TOKEN_LPARAN)) return parseCallExpr(nameTok);

        if (check(TokenType::TOKEN_LBRACKET)) {
            std::vector<std::unique_ptr<ASTNode>> indices;
            while (check(TokenType::TOKEN_LBRACKET)) {
                advance();
                indices.push_back(parseExpression());
                consume(TokenType::TOKEN_RBRACKET, "ERROR218");
            }
            auto idxExpr = withLoc(std::make_unique<IndexExprNode>(fullName, std::move(indices), t.line), nameTok);

            while (check(TokenType::TOKEN_DOT)) {
                advance();
                Token member = consume(TokenType::TOKEN_IDENT, "ERROR217");
                idxExpr->memberChain.push_back(member.lexeme);
            }
            spanFrom(idxExpr.get(), SourceLoc{t.line, t.column, 1}, previous());
            return idxExpr;
        }
        return withLoc(std::make_unique<IdentNode>(fullName, "", t.line), nameTok);
    }
    if (t.type == TokenType::TOKEN_LPARAN) {
        advance();
        auto expression = parseExpression();
        consume(TokenType::TOKEN_RPARAN, "ERROR204");
        auto node = withLoc(std::make_unique<GroupedExprNode>(std::move(expression), t.line), t);
        spanFrom(node.get(), {t.line, t.column, 1}, previous());
        return node;
    }
    return parseLiteral();
}