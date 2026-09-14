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
    int depth = 0;
    while (!isAtEnd()) {
        if (depth == 0) {
            switch (peek().type) {
                case TokenType::TOKEN_RBRACE:
                case TokenType::TOKEN_IF:
                case TokenType::TOKEN_ELSE:
                case TokenType::TOKEN_WHILE:
                case TokenType::TOKEN_DOWHILE:
                case TokenType::TOKEN_FOR:
                case TokenType::TOKEN_SWITCH:
                case TokenType::TOKEN_BREAK:
                case TokenType::TOKEN_CONTINUE:
                case TokenType::TOKEN_PASS:
                case TokenType::TOKEN_RETURN:
                case TokenType::TOKEN_ENUM:
                case TokenType::TOKEN_CONST:
                case TokenType::TOKEN_INT:
                case TokenType::TOKEN_FLOAT:
                case TokenType::TOKEN_BIGINT:
                case TokenType::TOKEN_DOUBLE:
                case TokenType::TOKEN_STRING:
                case TokenType::TOKEN_BOOL:
                case TokenType::TOKEN_CHAR:
                    return;
                default:
                    break;
            }
        }

        Token t = advance();
        if (t.type == TokenType::TOKEN_LBRACE) {
            depth++;
        } else if (t.type == TokenType::TOKEN_RBRACE) {
            depth--;
        } else if (t.type == TokenType::TOKEN_SEMICOLON && depth == 0) {
            return;
        }
    }
}
