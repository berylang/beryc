#include "parser.h"
#include "ast/functions.h"

/*

    Bery Functions,
    which are uniquely defined by new sytanx :

        func <func_name>([param_list]) -> return_type {
            // body
        }
    
    calling functions - 
        
        <func_name>([arg_list]);
    
    
*/

std::unique_ptr<ASTNode> Parser::parseFunctionDef(AccessSpecifier access) {
    advance(); 
    int line = previous().line;
    Token nameTokenen = consume(TokenType::TOKEN_IDENT, "ERROR219");
    consume(TokenType::TOKEN_LPARAN, "ERROR220");

    std::vector<std::pair<std::string, std::string>> params;
    if (!check(TokenType::TOKEN_RPARAN)) {
        do {
            Token typeToken = advance();
            Token nameToken = consume(TokenType::TOKEN_IDENT, "ERROR221");
            std::string paramType = typeToken.lexeme;
            if (check(TokenType::TOKEN_LBRACKET)) {
                advance();
                consume(TokenType::TOKEN_RBRACKET, "ERROR222");
                paramType = "array<" + paramType + ">";
            }
            params.push_back({paramType, nameToken.lexeme});
        } while (!isAtEnd() && check(TokenType::TOKEN_COMMA) && (advance(), true));
    }
    consume(TokenType::TOKEN_RPARAN, "ERROR223");

    std::string returnType = "void"; 
    if (check(TokenType::TOKEN_ARROW)) {
        advance();
        returnType = advance().lexeme;
        if (check(TokenType::TOKEN_LBRACKET)) {
            advance();
            consume(TokenType::TOKEN_RBRACKET, "ERROR224");
            returnType = "array<" + returnType + ">";
        }
    }

    consume(TokenType::TOKEN_LBRACE, "ERROR225");
    auto body = parseBlock();
    return std::make_unique<FunctionDefNode>(nameTokenen.lexeme, std::move(params), returnType, std::move(body),access, line);
}

std::unique_ptr<ASTNode> Parser::parseCallExpr(const Token& identifierToken) {
    consume(TokenType::TOKEN_LPARAN, "ERROR226");
    
    std::vector<std::unique_ptr<ASTNode>> arguments;
    if (!check(TokenType::TOKEN_RPARAN)) {
        do {
            arguments.push_back(parseExpression());
        } while (!isAtEnd() && check(TokenType::TOKEN_COMMA) && (advance(), true));
    }
    consume(TokenType::TOKEN_RPARAN, "ERROR227");
    
    return std::make_unique<CallExprNode>(identifierToken.lexeme, std::move(arguments), identifierToken.line);
}

std::unique_ptr<ASTNode> Parser::parseReturnStmt() {
    advance(); 
    int line = previous().line;
    std::unique_ptr<ASTNode> value = nullptr;
    if (!check(TokenType::TOKEN_SEMICOLON)) {
        value = parseExpression();
    }
    
    consume(TokenType::TOKEN_SEMICOLON, "ERROR228");
    return std::make_unique<ReturnStmtNode>(std::move(value), line);
}
