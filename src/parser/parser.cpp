/*

    Parser Class

    implementation of the Parser class's utility functions and parse() function.
    it includes - 
        1. parse() - public method
        2. utility functions
        3. parseBlock() and parseStatements()

*/

#include "parser.h"
#include "ast/programnode.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens, DiagnosticEngine& diag)
    : tokens(tokens), current(0), errors(false), diag(diag) {}

std::unique_ptr<ASTNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();

    try {
        parsePrologues(program.get());
    } catch (ParseError& e) {
        synchronize();
    }

    while (!isAtEnd()) {
        size_t startPos = current;
        try {
            if (check(TokenType::TOKEN_RUN)) {
                advance();
                int runline = previous().line;
                consume(TokenType::TOKEN_LBRACE, "ERROR276");
                auto runBlock = std::make_unique<RunBlockNode>(runline);
                while (!isAtEnd() && !check(TokenType::TOKEN_RBRACE)) {
                    size_t startPos = current;
                    try {
                        for (auto& statement : parseStatement()) runBlock->statements.push_back(std::move(statement));
                    } catch(ParseError& e) {
                        synchronize();
                        if (current == startPos && !isAtEnd()) advance();
                    }
                }
                program->runBlock = std::move(runBlock);
                consume(TokenType::TOKEN_RBRACE, "ERROR277");
            } 
            else if (check(TokenType::TOKEN_FUNC)) {
                program->globals.push_back(parseFunctionDef(AccessSpecifier::PUBLIC));
            } else if (check(TokenType::TOKEN_ENUM)) {
                program->globals.push_back(parseEnumDecl());
            } else if (check(TokenType::TOKEN_IMPORT)) {
                program->globals.push_back(parseImportDecl());
            } else if (check(TokenType::TOKEN_EXTERN)) {
                program->globals.push_back(parseExternDecl());
            } else if(check(TokenType::TOKEN_CLASS)) {
                program->globals.push_back(parseClassDecl());
            } else {
                bool isConst = false;
                if (check(TokenType::TOKEN_CONST)) { advance(); isConst = true; }
                if (isTypeToken(peek().type) || isClassVarDecl()) {
                    auto decls = parseVarDecl(AccessSpecifier::PUBLIC, isConst);
                    for (auto& d : decls) program->globals.push_back(std::move(d));
                } else {
                    diag.report("ERROR274", peek().line, 1, peek().lexeme);
                    errors = true;
                    throw ParseError();
                }
            }
        } catch(ParseError& e) {
            synchronize();
            if (current == startPos && !isAtEnd()) advance();
        }
    }

    if (!program->runBlock && !program->moduleOptionalRun) {
        diag.report("ERROR275", peek().line,1,peek().lexeme);
        errors = true;
    }
    
    return program;
}


Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token Parser::peek() {
    if (current >= (int)tokens.size()) return tokens.back(); 
    return tokens[current];

}

Token Parser::previous() {
    return tokens[current - 1];
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::TOKEN_EOF;
}

bool Parser::check(TokenType type) { 
    return peek().type == type;
}

Token Parser::consume(TokenType type, const std::string& code, const std::string& context) {
    if (check(type)) return advance();
    errors = true;
    diag.report(code, peek().line, 1, peek().lexeme, context);
    throw ParseError();
}

bool Parser::isTypeToken(TokenType t) {
    return t == TokenType::TOKEN_INT ||
            t == TokenType::TOKEN_BIGINT ||
           t == TokenType::TOKEN_BOOL ||
           t == TokenType::TOKEN_FLOAT ||
           t == TokenType::TOKEN_DOUBLE ||
           t == TokenType::TOKEN_CHAR ||
           t == TokenType::TOKEN_STRING; 
}
std::vector<std::unique_ptr<ASTNode>> Parser::single(std::unique_ptr<ASTNode> node) {
    std::vector<std::unique_ptr<ASTNode>> outputStream;
    outputStream.push_back(std::move(node));
    return outputStream;
}
std::unique_ptr<BlockNode> Parser::parseBlock() {
    int line = previous().line;

    auto block = std::make_unique<BlockNode>(line);
    while(!isAtEnd() && !check(TokenType::TOKEN_RBRACE)){
        size_t startPos = current;
        try {
            for (auto& statement : parseStatement()) block->statements.push_back(std::move(statement));
        }
        catch(ParseError& e) {
            synchronize();
            if (current == startPos && !isAtEnd()) advance(); 
        }
    }
    consume(TokenType::TOKEN_RBRACE, "ERROR278");
    return block;
    
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseStatement() {
    if (check(TokenType::TOKEN_IF)) {
        return single(parseIfStmt());
    }
    if (check(TokenType::TOKEN_ELSE)){
        advance();
        return single(parseBlock());
    }
    if(check(TokenType::TOKEN_WHILE)){
        return single(parseWhileStmt());
    }
    if(check(TokenType::TOKEN_DOWHILE)){
        return single(parseDoWhileStmt());
    }
    if(check(TokenType::TOKEN_FOR)){
        return single(parseForStmt());
    }
    bool isConst = false;
    if(check(TokenType::TOKEN_CONST)){
        advance();
        isConst = true;
    }
    if (check(TokenType::TOKEN_SWITCH)) {
        return single(parseSwitchStmt());
    }
    if (check(TokenType::TOKEN_BREAK)) {
        return single(parseBreakStmt());
    }
    if(check(TokenType::TOKEN_CONTINUE)) {
        return single(parseContinueStmt());
    }
    if(check(TokenType::TOKEN_PASS)) {
        return single(parsePassStmt());
    }
    if(check(TokenType::TOKEN_RETURN)){
        return single(parseReturnStmt());
    }
    if (check(TokenType::TOKEN_ENUM)) {
        return single(parseEnumDecl());
    }

    if(isTypeToken(peek().type) || isClassVarDecl()){
        return parseVarDecl(AccessSpecifier::PUBLIC, isConst);
    }
    auto expr = parseExpression();
    consume(TokenType::TOKEN_SEMICOLON, "ERROR201");
    return single(std::move(expr));
}


void Parser::parsePrologues(ProgramNode* program) {
    std::unordered_set<std::string> seen;
    while (check(TokenType::TOKEN_HASH)) {
        advance();
        Token nameTok = consume(TokenType::TOKEN_IDENT, "ERROR282");
        std::string name = nameTok.lexeme;

        auto it = PROLOGUE_VALUES.find(name);
        if (it == PROLOGUE_VALUES.end()) {
            diag.report("ERROR279", nameTok.line, 1, nameTok.lexeme, nameTok.lexeme);
            errors = true;
            throw ParseError();
        }
        if (!seen.insert(name).second) {
            diag.report("ERROR281", nameTok.line, 1, nameTok.lexeme, nameTok.lexeme);
            errors = true;
            throw ParseError();
        }

        Token valueTok = peek();
        bool validValueToken = check(TokenType::TOKEN_IDENT) || check(TokenType::TOKEN_TRUE) || check(TokenType::TOKEN_FALSE);
        if (!validValueToken || !it->second.count(valueTok.lexeme)) {
            diag.report("ERROR280", valueTok.line, 1, valueTok.lexeme, std::vector<std::string>{valueTok.lexeme, name});
            errors = true;
            throw ParseError();
        }
        advance();

        if (name == "memory") program->memoryManaged = (valueTok.lexeme == "managed");
        else if (name == "module") program->moduleOptionalRun = (valueTok.lexeme == "true");

        consume(TokenType::TOKEN_SEMICOLON, "ERROR201", "prologue directive");
    }
}