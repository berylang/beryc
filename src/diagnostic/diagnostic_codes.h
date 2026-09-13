#pragma once
#include <string>
#include <unordered_map>
#include "diagnostic.h"

struct DiagnosticInfo {
    Severity severity;
    std::string messageTemplate;
    std::string hint;
};

static const std::unordered_map<std::string, DiagnosticInfo> DiagnosticRegistry = {

    // adding instructions - 
    // we have to add like this -
    //
    //
    //      {errorcode, {severity, msg, hint} }
    // 
    // hint should be basic, and targeted only for that specific error/warning.
    //
    //


    // @LEXER ERRORS and WARNINGS
    { "ERROR009", { Severity::ERROR, "Empty char literal", "put a character between the quotes" } },
    { "ERROR012", { Severity::ERROR, "Newline in char literal", "char literals cannot span multiple lines" } },
    { "ERROR010", { Severity::ERROR, "Incomplete escape sequence", "finish the escape sequence, e.g. '\\\\n'" } },
    { "ERROR011", { Severity::ERROR, "Invalid escape sequence '\\{}'", "use one of: \\n \\t \\r \\\\ \\0 \\\" \\'" } },
    { "ERROR013", { Severity::ERROR, "Multi-character char literal", "char literals must hold exactly one character" } },
    { "ERROR014", { Severity::ERROR, "Unclosed char literal", "add a closing '" } },
    { "ERROR015", { Severity::ERROR, "Invalid escape sequence '\\{}' in string", "use one of: \\n \\t \\r \\\\ \\0 \\\" \\'" } },
    { "ERROR016", { Severity::ERROR, "Unclosed string literal", "add a closing \"" } },
    { "ERROR017", { Severity::ERROR, "Unclosed comment", "add '!--' to close the comment block" } },


    // @PARSER ERRORS and WARNINGS
    { "ERROR201", { Severity::ERROR, "Expected ';' after {}", "put ';' symbol right after {}" } },
    { "ERROR202", { Severity::ERROR, "Expected '}' after {}", "add '}' to close {}" } },
    { "ERROR203", { Severity::ERROR, "Expected 'case' or 'default'", "add a 'case' or 'default' label" } },
    { "ERROR204", { Severity::ERROR, "Expected ')' after {}", "add ')' after {}" } },
    { "ERROR205", { Severity::ERROR, "Expected '{' before {}", "add '{' before {}" } },
    { "ERROR206", { Severity::ERROR, "Expected '(' after {}", "add '(' after {}" } },
    { "ERROR207", { Severity::ERROR, "Expected '{' before switch body", "add '{' before the switch body" } },
    { "ERROR208", { Severity::ERROR, "Expected ':' after case", "add ':' after the case value" } },
    { "ERROR209", { Severity::ERROR, "Expected ':'", "add ':' here" } },
    { "ERROR210", { Severity::ERROR, "Expected '}' after switch body", "add '}' to close the switch body" } },
    { "ERROR211", { Severity::ERROR, "Invalid assignment target", "assign the value to a valid variable or target" } },
    { "ERROR212", { Severity::ERROR, "Expected ':' in ternary operator", "add ':' between the two expressions" } },
    { "ERROR213", { Severity::ERROR, "Expected ',' after lower bound", "add ',' after the lower bound" } },

    { "ERROR214", { Severity::ERROR, "Expected class name after 'new'", "add a class name after 'new'" } },
    { "ERROR215", { Severity::ERROR, "Expected '(' after class name", "add '(' after the class name" } },
    { "ERROR216", { Severity::ERROR, "Expected ')' after constructor arguments", "add ')' after the constructor arguments" } },
    { "ERROR217", { Severity::ERROR, "Expected identifier after '.'", "add an identifier after '.'" } },
    { "ERROR218", { Severity::ERROR, "Expected ']' after array index", "add ']' after the array index" } },

    //functions
    { "ERROR219", { Severity::ERROR, "Expected function name", "add a function name" } },
    { "ERROR220", { Severity::ERROR, "Expected '(' after function name", "add '(' after the function name" } },
    { "ERROR221", { Severity::ERROR, "Expected parameter name", "add a parameter name" } },
    { "ERROR222", { Severity::ERROR, "Expected ']' after '[' in array parameter type", "add ']' to close the array parameter type" } },
    { "ERROR223", { Severity::ERROR, "Expected ')' after parameters", "add ')' after the parameters" } },
    { "ERROR224", { Severity::ERROR, "Expected ']' after '[' in array return type", "add ']' to close the array return type" } },
    { "ERROR225", { Severity::ERROR, "Expected '{' before function body", "add '{' before the function body" } },
    { "ERROR226", { Severity::ERROR, "Expected '(' in function call", "add '(' after the function name" } },
    { "ERROR227", { Severity::ERROR, "Expected ')' after arguments", "add ')' after the arguments" } },
    { "ERROR228", { Severity::ERROR, "Expected ';' after return statement", "put ';' after the return statement" } },

    //loops
    { "ERROR229", { Severity::ERROR, "Expected '(' after 'while'", "add '(' after 'while'" } },
    { "ERROR230", { Severity::ERROR, "Expected '{' before while-body", "add '{' before the while body" } },
    { "ERROR231", { Severity::ERROR, "Expected '{' before do-while-body", "add '{' before the do-while body" } },
    { "ERROR232", { Severity::ERROR, "Expected 'while' condition after do-while-body", "add the while condition after the do-while body" } },
    { "ERROR233", { Severity::ERROR, "Expected ';' after condition-end", "put ';' after the condition" } },
    { "ERROR234", { Severity::ERROR, "Expected '(' after 'for'", "add '(' after 'for'" } },
    { "ERROR235", { Severity::ERROR, "Expected identifier", "add an identifier" } },
    { "ERROR236", { Severity::ERROR, "Expected ')' after for-in declaration", "add ')' after the for-in declaration" } },
    { "ERROR237", { Severity::ERROR, "Expected '{' before loop body", "add '{' before the loop body" } },
    { "ERROR238", { Severity::ERROR, "Expected ';' after loop condition", "put ';' after the loop condition" } },
    { "ERROR239", { Severity::ERROR, "Expected ')' after loop update", "add ')' after the loop update" } },
        
    //OOP
    { "ERROR240", { Severity::ERROR, "Destructor name '~{}' does not match class '{}'", "use the class name as the destructor name" } },
    { "ERROR241", { Severity::ERROR, "Expected class name", "add a class name" } },
    { "ERROR242", { Severity::ERROR, "Expected '{' after class name", "add '{' after the class name" } },
    { "ERROR243", { Severity::ERROR, "Expected '}' after class body", "add '}' to close the class body" } },
    { "ERROR244", { Severity::ERROR, "Expected 'attributes' section", "add the 'attributes' section" } },
    { "ERROR245", { Severity::ERROR, "Expected '[' after 'attributes'", "add '[' after 'attributes'" } },
    { "ERROR246", { Severity::ERROR, "Expected self-reference identifier", "add a self-reference identifier" } },
    { "ERROR247", { Severity::ERROR, "Expected ']' after self-reference", "add ']' after the self-reference" } },
    { "ERROR248", { Severity::ERROR, "Expected '::' after attributes section", "add '::' after the attributes section" } },
    { "ERROR249", { Severity::ERROR, "Expected 'methods' section", "add the 'methods' section" } },
    { "ERROR250", { Severity::ERROR, "Expected '::' after 'methods'", "add '::' after 'methods'" } },
    { "ERROR251", { Severity::ERROR, "Expected class name after '~'", "add the class name after '~'" } },
    { "ERROR252", { Severity::ERROR, "Expected '(' after destructor name", "add '(' after the destructor name" } },
    { "ERROR253", { Severity::ERROR, "Expected '{' before destructor body", "add '{' before the destructor body" } },
    { "ERROR254", { Severity::ERROR, "Expected '(' after constructor name", "add '(' after the constructor name" } },
    { "ERROR255", { Severity::ERROR, "Expected '{' before constructor body", "add '{' before the constructor body" } },

    //expressions/declarations
    { "ERROR256", { Severity::ERROR, "Expected valid expression or literal", "provide a valid expression or literal" } },
    { "ERROR257", { Severity::ERROR, "Arrays must be initialized with list inside '{}'", "initialize the array using a list inside '{}'" } },
    { "ERROR258", { Severity::ERROR, "Expected ']'", "add ']'" } },
    { "ERROR259", { Severity::ERROR, "Expected ';'", "add ';'" } },
    { "ERROR260", { Severity::ERROR, "Expected type", "add a valid type" } },
    { "ERROR261", { Severity::ERROR, "Expected '{'", "add '{'" } },
    { "ERROR262", { Severity::ERROR, "Expected '}'", "add '}'" } },

    //enum
    { "ERROR263", { Severity::ERROR, "Expected enum name", "add an enum name" } },
    { "ERROR264", { Severity::ERROR, "Expected '=' after enum name", "add '=' after the enum name" } },
    { "ERROR265", { Severity::ERROR, "Expected '{' to start enum values", "add '{' before enum values" } },
    { "ERROR266", { Severity::ERROR, "Expected enum value", "add an enum value" } },
    { "ERROR267", { Severity::ERROR, "Expected '}' after enum values", "add '}' after enum values" } },
    { "ERROR268", { Severity::ERROR, "Expected ';' after enum declaration", "put ';' after the enum declaration" } },

    //imports
    { "ERROR269", { Severity::ERROR, "Expected module name after 'import'", "add a module name after 'import'" } },
    { "ERROR270", { Severity::ERROR, "Expected 'func' after 'extern'", "add 'func' after 'extern'" } },
    { "ERROR271", { Severity::ERROR, "Expected function name after 'func'", "add a function name after 'func'" } },
    { "ERROR272", { Severity::ERROR, "Expected ')' after extern parameters", "add ')' after the extern parameters" } },
    { "ERROR273", { Severity::ERROR, "Expected ';' after extern declaration", "put ';' after the extern declaration" } },

    { "ERROR274", { Severity::ERROR, "Unexpected token", "remove or replace the unexpected token" } },
    { "ERROR275", { Severity::ERROR, "no run{} block found", "add a run{} block to the program" } },
    { "ERROR276", { Severity::ERROR, "Expected '{' after run", "add '{' after run" } },
    { "ERROR277", { Severity::ERROR, "Expected '}' after run block", "add '}' to close the run block" } },
    { "ERROR278", { Severity::ERROR, "Expected '}' after block", "add '}' to close the block" } },


    // @SEMA ERRORS and WARNINGS
    { "ERROR003", { Severity::ERROR, "Undefined variable '{}'", "declare '{}' before using it" } },
    { "ERROR004", { Severity::ERROR, "'{}' already declared in this scope", "rename this or remove the earlier declaration" } },
    { "ERROR005", { Severity::ERROR, "'{}' used outside of a loop or switch", "use {} only inside a loop/switch body" } },
};