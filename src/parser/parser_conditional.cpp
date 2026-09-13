#include "parser.h"

/*

    Parsing conditional statements

    This file include if-else_if-else statements, and switch-case blocks
    normal syntax.

*/

#include "ast/controlflow.h"
#include <iostream>


std::unique_ptr<ASTNode> Parser::parseIfStmt() {
    advance();
    int line = previous().line;

    consume(TokenType::TOKEN_LPARAN, "ERROR206","if");
    auto condition = parseExpression();
    consume(TokenType::TOKEN_RPARAN, "ERROR204","condition");
    consume(TokenType::TOKEN_LBRACE, "ERROR205","if body");
    auto ifBranch = parseBlock();    
    std::unique_ptr<ASTNode> elseBranch = nullptr;
    if(check(TokenType::TOKEN_ELSE)){
        advance();
        if(check(TokenType::TOKEN_IF)){
            elseBranch = parseIfStmt();
        }else{
            consume(TokenType::TOKEN_LBRACE, "ERROR205","if else body");
            elseBranch = parseBlock();
        }
    }

    return std::make_unique<IfStmtNode>(std::move(condition),std::move(ifBranch),std::move(elseBranch),line); 

}

std::unique_ptr<ASTNode> Parser::parseSwitchStmt() {
    advance();
    int line = previous().line;

    consume(TokenType::TOKEN_LPARAN, "ERROR206","switch");
    auto expr = parseExpression();
    consume(TokenType::TOKEN_RPARAN, "ERROR204","condition");
    consume(TokenType::TOKEN_LBRACE, "ERROR207");

    auto sw = std::make_unique<SwitchStmtNode>(line);
    sw->condition = std::move(expr);

    while (!isAtEnd() && !check(TokenType::TOKEN_RBRACE)) {
        if (check(TokenType::TOKEN_CASE)) {
            advance();
            CaseBlock cb;
            cb.value = parseExpression();
            consume(TokenType::TOKEN_COLON, "ERROR208");
            while (!isAtEnd() && !check(TokenType::TOKEN_CASE) &&
                   !check(TokenType::TOKEN_DEFAULT) && !check(TokenType::TOKEN_RBRACE)) {
                for (auto& statement : parseStatement()) cb.statements.push_back(std::move(statement));
            }
            sw->cases.push_back(std::move(cb));
        }
        else if (check(TokenType::TOKEN_DEFAULT)) {
            advance();
            consume(TokenType::TOKEN_COLON, "ERROR209");
            sw->hasDefault = true;

            while (!isAtEnd() && !check(TokenType::TOKEN_CASE) && !check(TokenType::TOKEN_RBRACE)) {
                for (auto& statement : parseStatement()) sw->defaultBlock.push_back(std::move(statement));
            }
        }
        else {
            errors = true;
            diag.report("ERROR203", peek().line, 1, peek().lexeme);
            advance();
        }
    }
    consume(TokenType::TOKEN_RBRACE, "ERROR210");
    return sw;
}