/*

    Lexer Class

    actual implementation of the Lexer class.
    it includes - 
        1. keyword hashmap, which is checked by scanIdentifierOrKeyword() -> checkKeyword()
        2. Scanner, implemented by maximal munch method.

*/

#include "lexer.h"
#include <unordered_map>
#include <stdexcept>
#include <iostream>


static std::unordered_map<std::string, TokenType> keywords = {

    // @types
    {"int", TokenType::TOKEN_INT},
    {"float", TokenType::TOKEN_FLOAT},
    {"bigint", TokenType::TOKEN_BIGINT},
    {"double", TokenType::TOKEN_DOUBLE},
    {"bool", TokenType::TOKEN_BOOL},
    {"char", TokenType::TOKEN_CHAR},
    {"string", TokenType::TOKEN_STRING},

    // @blocks
    {"run", TokenType::TOKEN_RUN},

    // @literals
    {"true", TokenType::TOKEN_TRUE},
    {"false", TokenType::TOKEN_FALSE},
    {"null", TokenType::TOKEN_NULL},

    // @conditionals
    {"if", TokenType::TOKEN_IF},
    {"else", TokenType::TOKEN_ELSE},
    {"switch",TokenType::TOKEN_SWITCH},
    {"case",TokenType::TOKEN_CASE},
    {"default",TokenType::TOKEN_DEFAULT},
    
    // @controlflow
    {"break",TokenType::TOKEN_BREAK},
    {"continue", TokenType::TOKEN_CONTINUE},
    {"pass", TokenType::TOKEN_PASS},

    // @loops
    {"while", TokenType::TOKEN_WHILE},
    {"do", TokenType::TOKEN_DOWHILE},
    {"for", TokenType::TOKEN_FOR},
    {"in", TokenType::TOKEN_IN},

    // @functions
    {"func", TokenType::TOKEN_FUNC},
    {"return", TokenType::TOKEN_RETURN},

    // @special keywords
    {"enum", TokenType::TOKEN_ENUM},
    {"import", TokenType::TOKEN_IMPORT}, 
    {"extern", TokenType::TOKEN_EXTERN},
    {"const", TokenType::TOKEN_CONST},
    {"delete", TokenType::TOKEN_DELETE},
    
    // @object oriented programming
    {"class", TokenType::TOKEN_CLASS},
    {"attributes", TokenType::TOKEN_ATTRIBUTES},
    {"methods", TokenType::TOKEN_METHODS},
    {"new", TokenType::TOKEN_NEW},
    {"ref", TokenType::TOKEN_REF},
    {"public", TokenType::TOKEN_PUBLIC},
    {"private", TokenType::TOKEN_PRIVATE},
    {"protected", TokenType::TOKEN_PROTECTED},
    {"super", TokenType::TOKEN_SUPER},
};

Lexer::Lexer(const std::string& source, DiagnosticEngine& diag)
    : source(source), current(0), line(1), col(1), startColumn(1), startLine(1), diag(diag) {}


std::vector<Token> Lexer::tokanize() {
    while (!isAtEnd()) {
        skipWhitespaces();
        if (!isAtEnd()) scanToken();
    }
    tokens.push_back({TokenType::TOKEN_EOF, "", line, col});

    // it goes to parser next. vector of tokens.
    return tokens;
}

void Lexer::scanToken() {
    startColumn = col;
    startLine = line;
    char c = advance();

    if (isAlpha(c)) {
        scanIdentifierOrKeyword();
        return;
    }

    if (isDigit(c)) {
        scanNumber();
        return;
    }
    switch(c) {
        case '=' : 
            if(peek()=='='){
                advance();
                emit(TokenType::TOKEN_EQUAL_EQUAL,"==");
                return;
            }
            emit(TokenType::TOKEN_EQUAL, "=");
            return;
        case ';': 
            emit(TokenType::TOKEN_SEMICOLON, ";");
            return;
        case '{':
            emit(TokenType::TOKEN_LBRACE, "{");
            return;//change pushback to emit
        case '}':
            emit(TokenType::TOKEN_RBRACE, "}");
            return;
        case ',':
            emit(TokenType::TOKEN_COMMA, ",");
            return;
        case '[':
            emit(TokenType::TOKEN_LBRACKET, "[");
            return;
        case ']':
            emit(TokenType::TOKEN_RBRACKET, "]");
            return;
        case '\'':
            scanCharLit();
            return;
        case '"':
            scanStringLit();
            return;
        case '*':
            if(peek()=='='){
                advance();
                emit(TokenType::TOKEN_MUL_ASSIGN, "*=");
            }
            else if(peek() == '*'){
                if(peekNext()== '='){
                    advance();
                    advance();
                    emit(TokenType::TOKEN_DSTAR_ASSIGN, "**=");
                    return; 
                }else{
                    advance();
                    emit(TokenType::TOKEN_POWER, "**");
                return;
                }
            }
            else{
                emit(TokenType::TOKEN_STAR, "*");
            }
            return;
        case '/':
            if(peek()=='='){
                    advance();
                    emit(TokenType::TOKEN_DIV_ASSIGN, "/=");
            }else{
                emit(TokenType::TOKEN_FSLASH, "/");
                }
                return;
        case '%':
            if(peek()=='='){
                advance();
                emit(TokenType::TOKEN_MODULE_ASSIGN,"%=");
            }else{
            emit(TokenType::TOKEN_PERCENT, "%");
            }
            return;
        
        case '-':
            if(peek()=='-'){
                if(peekNext()=='-'){ 
                    advance();
                    advance();
                    skipComments(false);
                    return;
                }
                else if(peekNext()=='!'){ 
                    advance();
                    advance();
                    skipComments(true);
                    return;
                }
                else{
                    advance();
                    emit(TokenType::TOKEN_DEC, "--");
                    return;
                }
            }
            else if(peek()=='='){
                advance();
                emit(TokenType::TOKEN_SUB_ASSIGN,"-=");
                return;
            }
            else if(peek()=='>'){
                advance();
                emit(TokenType::TOKEN_ARROW, "->");
                return;
            }
            else{
                emit(TokenType::TOKEN_MINUS, "-");
                return;
            }
            return;
        case '+':
           if(peek()=='+'){
                advance();
                emit(TokenType::TOKEN_INC, "++");
                return;
            }
            else if(peek()=='='){
                advance();
                emit(TokenType::TOKEN_ADD_ASSIGN,"+=");
                return;
            }
            else{
                emit(TokenType::TOKEN_PLUS, "+");
                return;
            }
            return;
        case '~':
            emit(TokenType::TOKEN_TILDE, "~");
            return;
        case '!':
            if(peek()=='='){
                advance();
                emit(TokenType::TOKEN_NOT_EQUAL, "!=");
                return;
            }
            else if(peek()=='>' && peekNext()=='<'){
                advance();
                advance();
                emit(TokenType::TOKEN_NOT_BETWEEN, "!><");
                return;
            }
            else{
                emit(TokenType::TOKEN_BANG, "!");
                return;
            }
            return;
        case '(':
            emit(TokenType::TOKEN_LPARAN, "(");
            return;
        case ')':
            emit(TokenType::TOKEN_RPARAN, ")");
            return;
        case '<':
            if(peek()=='<'){
                if(peekNext()=='='){
                    advance();
                    advance();
                emit(TokenType::TOKEN_LSHIFT_ASSIGN, "<<=");
                return;
                }else{
                advance();
                emit(TokenType::TOKEN_LSHIFT, "<<");
                return;
                }
            }
            else if(peek()=='='){
                advance();
                emit(TokenType::TOKEN_LTEQUAL, "<=");
                return;
            }
            else{
               emit(TokenType::TOKEN_LTHAN, "<");
                return;
            }
            return;
        case '>':
            if(peek()=='>'){
                if(peekNext()=='='){
                    advance();
                    advance();
                    emit(TokenType::TOKEN_RSHIFT_ASSIGN, ">>=");
                    return;
                }else{
                advance();
                emit(TokenType::TOKEN_RSHIFT, ">>");
                }
                return;
            }
            else if(peek()=='<'){
                advance();
                emit(TokenType::TOKEN_BETWEEN, "><");
                return;
            }
            else if(peek()=='='){
                advance();
                emit(TokenType::TOKEN_GTEQUAL, ">=");
                return;
            }
            else{
                emit(TokenType::TOKEN_GTHAN, ">");
                return;
            }
            return;
        case '^':
            if(peek()=='='){
                advance();
                emit(TokenType::TOKEN_XOR_ASSIGN, "^=");
                return;
            }else{
            emit(TokenType::TOKEN_CARET, "^");
            }
            return;
        case '&':
            if(peek()=='='){
                advance();
                emit(TokenType::TOKEN_AND_ASSIGN, "&=");
                return;
            }
            else if(peek()=='&'){
                advance();
                emit(TokenType::TOKEN_AND, "&&");
                return;
            }else{
            emit(TokenType::TOKEN_AMPERSAND, "&");
            }
            return;
        case '|':
            if(peek()=='='){
                advance();
                emit(TokenType::TOKEN_OR_ASSIGN, "|=");
                return;
            }
            else if(peek()=='|'){
                advance();
                emit(TokenType::TOKEN_OR, "||");
                return;
            }else{
            emit(TokenType::TOKEN_PIPE, "|");
            }
            return;
        case ':':
            if(peek()==':'){
                advance();
                emit(TokenType::TOKEN_DCOLON, "::");
                return;
            }else{
                emit(TokenType::TOKEN_COLON, ":");
                return;
            }
        case '?':
            emit(TokenType::TOKEN_QUESTION, "?");
            return;
        case '.':
            if (peek() == '.') {
                advance();
                emit(TokenType::TOKEN_RANGE, "..");
                return;
            }
            emit(TokenType::TOKEN_DOT, ".");
            return;
        }
        
}
/*

scanNumber() scans both integers and floating point numbers.

10 is valid integer
10.2 is valid floating point

10. is invalid
.10 is invalid

*/
void Lexer::scanNumber() {
    int start = current - 1;

    if (source[start] == '0' && !isAtEnd() && (peek() == 'x' || peek() == 'X' || peek() == 'b' || peek() == 'B' || peek() == 'o' || peek() == 'O')) {
        char prefix = peek();
        advance();
        int base = 10;
        std::string noDigitsCode, badDigitCode;
        int digitsStart = current;

        if (prefix == 'x' || prefix == 'X') {
            base = 16; 
            noDigitsCode = "ERROR110"; 
            badDigitCode = "ERROR113";
            while (!isAtEnd() && isHexDigit(peek())) advance();
        } else if (prefix == 'b' || prefix == 'B') {
            base = 2;  
            noDigitsCode = "ERROR111"; 
            badDigitCode = "ERROR114";
            while (!isAtEnd() && isBinaryDigit(peek())) advance();
        } else {
            base = 8;  
            noDigitsCode = "ERROR112"; 
            badDigitCode = "ERROR115";
            while (!isAtEnd() && isOctetDigit(peek())) advance();
        }

        if (current == digitsStart) {
            std::string full = source.substr(start, current - start);
            diag.report(noDigitsCode, startLine, startColumn, full, full);
            emit(TokenType::TOKEN_INT_LIT, "0");
            return;
        }
        if (!isAtEnd() && isAlphaNumeric(peek())) {
            std::string bad(1, peek());
            diag.report(badDigitCode, line, col, bad, bad);
            advance();
        }
        std::string digits = source.substr(digitsStart, current - digitsStart);
        try {
            unsigned long long value = std::stoull(digits, nullptr, base);
            emit(TokenType::TOKEN_INT_LIT, std::to_string(value));
        } catch (const std::out_of_range&) {
            std::string full = source.substr(start, current - start);
            diag.report("ERROR117", startLine, startColumn, full, full);
            emit(TokenType::TOKEN_INT_LIT, "0");
        }
        return;
    }
    while (!isAtEnd() && isDigit(peek())) advance();

    bool isDecimal = false;
    if (!isAtEnd() && peek() == '.' && isDigit(peekNext())) {
        isDecimal = true;
        advance();
        while (!isAtEnd() && isDigit(peek())) advance();
    }

    if (!isAtEnd() && (peek() == 'e' || peek() == 'E')) {
        int off = 1;
        if (current + off < (int)source.size() && (source[current + off] == '+' || source[current + off] == '-')) off++;
        bool hasExpDigit = (current + off < (int)source.size()) && isDigit(source[current + off]);

        if (hasExpDigit) {
            isDecimal = true;
            advance();
            if (!isAtEnd() && (peek() == '+' ||peek() =='-')) advance();
            while (!isAtEnd() && isDigit(peek())) advance();
        } else {
            std::string bad = source.substr(start, current - start + 1);
            diag.report("ERROR116", startLine, startColumn, bad, bad);
            advance();
            if (!isAtEnd() && (peek() == '+' || peek() == '-')) advance();
        }
    }
    std::string text = source.substr(start, current - start);
    emit(isDecimal ? TokenType::TOKEN_DECIMAL_LIT : TokenType::TOKEN_INT_LIT, text);
}

// @todo : Enhance it later for Errors
void Lexer::scanCharLit() {

    /*
    
        Single character datatype currently only finds
        ASCII values.

        @todo : Changed it to the UTF-8 encoding.
    
    */
    if (peek() == '\'') { 
        diag.report("ERROR100", startLine, startColumn, "'");
        advance(); 
        return;
    }

    char value = 0;

    if (peek() == '\\') { 
        advance(); 
        if (isAtEnd() || peek() == '\'') { 
            diag.report("ERROR102", startLine, startColumn, "\\");
            return;
        }

        char es = advance();
        switch (es) {

            // supported escape sequences - \n, \t, \r, \\, \0, \" and \'
            // change it for UTF-8 encoding.
            case 'n':  value = '\n'; break;
            case 't':  value = '\t'; break;
            case 'r':  value = '\r'; break;
            case '\\': value = '\\'; break;
            case '0':  value = '\0'; break;
            case '"':  value = '\"'; break;
            case '\'': value = '\''; break;
            default:
                diag.report("ERROR103", startLine, startColumn, std::string(1, es));
                return;
        }
    } 
    
    else {
        if (peek() == '\n' || peek() == '\r') {
            diag.report("ERROR101", startLine, startColumn, "");
            return;
        }
        value = advance();
    }

    if (!isAtEnd() && peek() == '\'') {        
        advance(); 
        emit(TokenType::TOKEN_CHAR_LIT, std::string(1,value));
        return;
    }

    bool foundClosingQuote = false;
    while (!isAtEnd() && peek() != '\'' && peek() != '\n' && peek() != '\r') {
        advance();
    }
    if (!isAtEnd() && peek() == '\'') {
        advance(); 
        foundClosingQuote = true;
    }
    if (foundClosingQuote) {
        diag.report("ERROR104", startLine, startColumn, "");
    } else {
        diag.report("ERROR105", startLine, startColumn, "");
    }
}


void Lexer::scanStringLit() {
    std::string value = "";
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n') bumpLine();
        if (peek() == '\\') {
            advance();
            if (isAtEnd()) return;
            
            char es = advance();
            switch (es) {
                case 'n':  value += '\n'; break;
                case 't':  value += '\t'; break;
                case 'r':  value += '\r'; break;
                case '\\': value += '\\'; break;
                case '0':  value += '\0'; break;
                case '"':  value += '\"'; break;
                case '\'': value += '\''; break;
                default:
                    diag.report("ERROR106", startLine, startColumn, std::string(1, es));
                    value += es; 
                    break;
            }
        } else {
            value += advance();
        }
    }
    if (isAtEnd()) {
        diag.report("ERROR107", startLine, startColumn, "");
        return;
    }

    advance(); 
    emit(TokenType::TOKEN_STRING_LIT, value);
}
void Lexer::scanIdentifierOrKeyword() {
    int start = current - 1;
    while (!isAtEnd() && isAlphaNumeric(peek())) advance();
    std::string lexeme = source.substr(start, current - start);
    emit(checkKeyword(lexeme), lexeme);
}


void Lexer::skipWhitespaces() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') advance();
        else break;
    }
}

void Lexer::skipComments(bool isMLC){
    if(isMLC){
        while(!isAtEnd()){
            if(peek()=='!' && peekNext()=='-'){
                advance();
                advance();
                if(peek()=='-'){
                    advance();
                    return;
                }
            }
            if(peek()=='\n'){bumpLine();}
            advance();
        }
        // errors=true;
        diag.report("ERROR108", startLine, startColumn, "");
        
    }
    else{
        while(!isAtEnd() && peek()!='\n'){advance();}

    }
}

TokenType Lexer::checkKeyword(const std::string& lexeme) {

    // O(1) lookup in the hashmap to check keyword;
    auto it = keywords.find(lexeme); 
    if (it != keywords.end()) return it->second;
    return TokenType::TOKEN_IDENT;
}

// @helpers
bool Lexer::isAtEnd() { return current >= (int) source.size(); }
bool Lexer::isAlphaNumeric(char c) {return isAlpha(c) || isDigit(c);}
bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    // it makes identifiers to follow -
    // [a-zA-Z_][a-zA-Z0-9_]

}
bool Lexer::isDigit(char c) {return c >= '0' && c <= '9';}

bool Lexer::isBinaryDigit(char c) { return c == '0' || c == '1'; }

bool Lexer::isOctetDigit(char c) { return c >= '0' && c <= '7'; }

bool Lexer::isHexDigit(char c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');}
char Lexer::advance() {
    char c = source[current++];
    if (c == '\n') bumpLine();
    else col++;
    return c;
}
char Lexer::peek() {return source[current];}
char Lexer::peekNext() {

    // Bound check added if lexer try to perform maximal 
    // munch at the end of the file.
    if(source.size() > current + 1) 
        return source[current + 1];
    return '\0';
}

// bool Lexer::hasErrors() {return errors;}

void Lexer::emit(TokenType type, const std::string& lexeme) {
    tokens.push_back({type, lexeme, line, startColumn});
}

void Lexer::bumpLine() {
    line++;
    col = 1;
}