#include "parser.h"


/*
    Error Recovery (Panic Mode)

    when parser hits a syntax error it throws ParseError() which unwinds the call stack instantly. 
    parse() catches it and calls synchronize() funciton, which skips tokens 
    until it finds a safe resync point (like ; or }), 
    
    then continues parsing the rest of the file that's why one error doesn't stop whole parse.
    thus we get multiple syntax errors reports in single pass.

*/
bool Parser::hasErrors() {return errors;}

void Parser::synchronize() {
    while (!isAtEnd()) {
        switch (peek().type) {
            case TokenType::TOKEN_RBRACE:
            case TokenType::TOKEN_LBRACE:
            case TokenType::TOKEN_FUNC:
            case TokenType::TOKEN_CLASS:
            case TokenType::TOKEN_ENUM:
            case TokenType::TOKEN_IMPORT:
            case TokenType::TOKEN_EXTERN:
            case TokenType::TOKEN_RUN:
            case TokenType::TOKEN_CONST:
            case TokenType::TOKEN_INT:
            case TokenType::TOKEN_FLOAT:
            case TokenType::TOKEN_BIGINT:
            case TokenType::TOKEN_DOUBLE:
            case TokenType::TOKEN_STRING:
            case TokenType::TOKEN_BOOL:
            case TokenType::TOKEN_CHAR:
            case TokenType::TOKEN_IF:
            case TokenType::TOKEN_WHILE:
            case TokenType::TOKEN_DOWHILE:
            case TokenType::TOKEN_FOR:
            case TokenType::TOKEN_SWITCH:
            case TokenType::TOKEN_CASE:
            case TokenType::TOKEN_DEFAULT:
            case TokenType::TOKEN_RETURN:
            case TokenType::TOKEN_BREAK:
            case TokenType::TOKEN_CONTINUE:
            case TokenType::TOKEN_DELETE:
            case TokenType::TOKEN_PUBLIC:
            case TokenType::TOKEN_PRIVATE:
            case TokenType::TOKEN_PROTECTED:
            case TokenType::TOKEN_METHODS:
            case TokenType::TOKEN_ATTRIBUTES:
                return;
            default:
                break;
        }
        if (advance().type == TokenType::TOKEN_SEMICOLON) return;
    }
}
