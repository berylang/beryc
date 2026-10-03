
#include "parser.h"
#include <stdexcept>
#include <iostream>
#include "ast/classes.h"
#include "ast/functions.h"

std::unique_ptr<ASTNode> Parser::parseClassDecl() {
    advance();
    Token className = consume(TokenType::TOKEN_IDENT, "ERROR214");

    std::string parentName = "";
    if (check(TokenType::TOKEN_COLON)) {
        advance();
        Token parentToken = consume(TokenType::TOKEN_IDENT, "ERROR241");
        parentName = parentToken.lexeme;
    }
    consume(TokenType::TOKEN_LBRACE, "ERROR242");

    auto attrSection = parseAttributeSection();

    std::unique_ptr<MethodSectionNode> methodSection = nullptr;
    if (check(TokenType::TOKEN_METHODS)) {
        methodSection = parseMethodSection(className.lexeme);
    }
    consume(TokenType::TOKEN_RBRACE, "ERROR243");
    return withLoc(std::make_unique<ClassDefNode>(className.lexeme, parentName, std::move(attrSection), std::move(methodSection), className.line), className);
}

std::unique_ptr<AttributeSectionNode> Parser::parseAttributeSection() {
    consume(TokenType::TOKEN_ATTRIBUTES, "ERROR244");
    consume(TokenType::TOKEN_LBRACKET, "ERROR245");
    Token selfToken = consume(TokenType::TOKEN_IDENT, "ERROR246");
    consume(TokenType::TOKEN_RBRACKET, "ERROR247");
    consume(TokenType::TOKEN_DCOLON, "ERROR248");

    std::vector<std::unique_ptr<ASTNode>> attributes;
    while(!isAtEnd()&& (isTypeToken(peek().type) || isClassVarDecl() || check(TokenType::TOKEN_PUBLIC) || check(TokenType::TOKEN_PRIVATE) || check(TokenType::TOKEN_PROTECTED))) {

        AccessSpecifier access = AccessSpecifier::PUBLIC;
        if(check(TokenType::TOKEN_PUBLIC)){
            advance();
            access=AccessSpecifier::PUBLIC;
        }
        else if(check(TokenType::TOKEN_PRIVATE)){
            advance();
            access=AccessSpecifier::PRIVATE;
        }
        else if(check(TokenType::TOKEN_PROTECTED)){
            advance();
            access=AccessSpecifier::PROTECTED;
        }
        if(!isTypeToken(peek().type) && !isClassVarDecl()) break;
        
        auto vars= parseVarDecl(access, false);
        for (auto& v : vars) {
            attributes.push_back(std::move(v));
        }
    }
    return withLoc(std::make_unique<AttributeSectionNode>(selfToken.lexeme, std::move(attributes), selfToken.line), selfToken);
}

std::unique_ptr<MethodSectionNode> Parser::parseMethodSection(const std::string& className) {
    Token methodsTok = consume(TokenType::TOKEN_METHODS, "ERROR249");
    consume(TokenType::TOKEN_DCOLON, "ERROR250");

    std::vector<std::unique_ptr<ASTNode>> methods;
    while(!isAtEnd()){
        AccessSpecifier access = AccessSpecifier::PUBLIC;
        if(check(TokenType::TOKEN_PUBLIC)){
            advance();
            access=AccessSpecifier::PUBLIC;
        }
        else if(check(TokenType::TOKEN_PRIVATE)){
            advance();
            access=AccessSpecifier::PRIVATE;
        }
        else if(check(TokenType::TOKEN_PROTECTED)){
            advance();
            access=AccessSpecifier::PROTECTED;
        }

        if (check(TokenType::TOKEN_TILDE)) {
            advance();
            Token nameToken =consume(TokenType::TOKEN_IDENT, "ERROR251");
            if (nameToken.lexeme!= className) {
                diag.report("ERROR240", nameToken.line, nameToken.column, nameToken.length, className);
                errors = true;
            }
            // No need - int declLine = nameToken.line;
            consume(TokenType::TOKEN_LPARAN, "ERROR252");
            std::vector<std::pair<std::string, std::string>> params;
            if (!check(TokenType::TOKEN_RPARAN)) {
                do {
                    Token typeToken = advance();
                    Token pNameToken = consume(TokenType::TOKEN_IDENT, "ERROR221");
                    std::string paramType = typeToken.lexeme;
                    if (check(TokenType::TOKEN_LBRACKET)) {
                        advance();
                        consume(TokenType::TOKEN_RBRACKET, "ERROR222");
                        paramType = "array<" + paramType + ">";
                    }
                    params.push_back({paramType, pNameToken.lexeme});
                } while (!isAtEnd() && check(TokenType::TOKEN_COMMA) && (advance(), true));
            }
            consume(TokenType::TOKEN_RPARAN, "ERROR223");
            consume(TokenType::TOKEN_LBRACE, "ERROR253");
            auto body = parseBlock();
            methods.push_back(withLoc(std::make_unique<FunctionDefNode>(className, std::move(params), "void", std::move(body), access, nameToken.line, false, true), nameToken));
            continue;
        }

        if (check(TokenType::TOKEN_IDENT) && peek().lexeme == className &&
            current + 1 < (int)tokens.size() && tokens[current + 1].type == TokenType::TOKEN_LPARAN) {
            Token nameToken = advance();
            int declLine = nameToken.line;
            consume(TokenType::TOKEN_LPARAN, "ERROR254");
            std::vector<std::pair<std::string, std::string>> params;
            if (!check(TokenType::TOKEN_RPARAN)) {  do {
                    Token typeToken = advance();
                    Token pNameToken = consume(TokenType::TOKEN_IDENT, "ERROR221");
                    std::string paramType = typeToken.lexeme;
                    if (check(TokenType::TOKEN_LBRACKET)) {
                        advance();
                        consume(TokenType::TOKEN_RBRACKET, "ERROR222");
                        paramType = "array<" + paramType + ">";
                    }
                    params.push_back({paramType, pNameToken.lexeme});
                } while (!isAtEnd() && check(TokenType::TOKEN_COMMA) && (advance(), true));
            }
            consume(TokenType::TOKEN_RPARAN, "ERROR223");
            consume(TokenType::TOKEN_LBRACE, "ERROR255");
            auto body =parseBlock();
            methods.push_back(withLoc(std::make_unique<FunctionDefNode>(className, std::move(params), "void", std::move(body), access, nameToken.line, true, false), nameToken));
            continue;
        }

        if(!check(TokenType::TOKEN_FUNC))
            break;
        methods.push_back(parseFunctionDef(access));
    }

    return withLoc(std::make_unique<MethodSectionNode>(std::move(methods), methodsTok.line), methodsTok);
}

bool Parser::isClassVarDecl() {
    return peek().type == TokenType::TOKEN_IDENT && current + 1 < (int)tokens.size() && tokens[current + 1].type == TokenType::TOKEN_IDENT;
}