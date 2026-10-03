#include "parser.h"
/*

    Parsing special controlflow statements
    which are - 
        1. break statement (switch, loops)
        2. continue statement (loops)
        3. pass statement (any block)
*/

#include "ast/controlflow.h"


std::unique_ptr<ASTNode> Parser::parseBreakStmt() {
    Token kw = advance();
    consume(TokenType::TOKEN_SEMICOLON, "ERROR201", "break");
    return withLoc(std::make_unique<BreakStmtNode>(kw.line), kw);
}

std::unique_ptr<ASTNode> Parser::parseContinueStmt() {
    Token kw = advance();
    consume(TokenType::TOKEN_SEMICOLON, "ERROR201", "continue");
    return withLoc(std::make_unique<ContinueStmtNode>(kw.line), kw);
}

std::unique_ptr<ASTNode> Parser::parsePassStmt() {
    Token kw = advance();
    consume(TokenType::TOKEN_SEMICOLON, "ERROR201", "pass");
    return withLoc(std::make_unique<PassStmtNode>(kw.line), kw);
}