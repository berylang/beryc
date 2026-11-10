#pragma once 

/*

LEXER DEFINITION

This is header file served for Lexer's definition.
it contains every helper functions which eventually helps the 'tokanize()' method.

*/

#include <string>
#include <vector>
#include <cstdint>
#include "token.h"
#include "../diagnostic/diagnostic_engine.h"

class Lexer {

public:
    Lexer(const std::string& source, DiagnosticEngine& diag);
    std::vector<Token> tokanize();


    // removed it. but let it be here for now -
    // bool hasErrors();

private:
    // @tokens data
    std::string source;
    int current;
    int line;
    int col;
    int startColumn;
    int startLine;
    int startOffset; 
    // bool errors;
    std::vector<Token> tokens;
    DiagnosticEngine& diag;
    int spanLen() const;
    void emit(TokenType type, const std::string& lexeme);
    void bumpLine();
    // @pointers inside the source
    char advance();
    char peek();
    char peekNext();
    bool isAtEnd();

    // @skip unnessesory
    void skipWhitespaces();
    void skipComments(bool isMLC);

    // @actual scanning
    void scanToken();
    void scanIdentifierOrKeyword();
    void scanNumber(); 
    void scanCharLit(); 
    void scanStringLit();

    // @identification of alpha-numeric data
    bool isDigit(char c);
    bool isBinaryDigit(char c);
    bool isOctetDigit(char c);
    bool isHexDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);

    // @utf-8 support
    void validateSource();
    void advanceCodePoint();
    void scanNonAscii();
    bool isNonAscii(char c);
    bool isIdentifierPartHere();
    bool scanUnicodeEscape(uint32_t& outValue, int escLine, int escCol, int escOffset);

    // @distinguish between tokens
    TokenType checkKeyword(const std::string& lexeme);
};