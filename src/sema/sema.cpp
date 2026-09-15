#include "sema.h"

/*

    Semantic Analyzer,
    this is main file, where we implemented the analyze() function, which has two passes, and
    delegate type checking to the TypeChecker.

    
*/
#include "../parser/ast/programnode.h"
#include "../parser/ast/vardecl.h"
#include "../parser/ast/arraydeclare.h"
#include "../parser/ast/functions.h"
#include "../parser/ast/classes.h"
#include <iostream>

SemanticAnalyzer::SemanticAnalyzer(ASTNode* root, DiagnosticEngine& diag)
   : root(root),  typeChecker(symbolTable, functions, classes, currentClassContext, currentFunctionIsConstructor, diag), diag(diag) {}

void SemanticAnalyzer::analyze() {
    /*
    
    This function performs semantic analysis on the entier program. It first
    collects global declarations such as -
        functions,
        classes,
        extern functions,
        and enums
    
    so they can referenced later. so declarations after run {} block it can be reffered.

    */
    auto* program = static_cast<ProgramNode*>(root);
    for (auto& node : program->globals) {
        if (node->type == NodeType::FUNC_DEF) {
            auto* func = static_cast<FunctionDefNode*>(node.get());

            /*
            
            first needs to build the function signature and register it so the analyzer can later
            resolve function calls and check for duplicate overloads

            */
            FunctionSignature sig;
            sig.returnType = func->returnType;
            for (auto& p : func->parameters) sig.parameterTypes.push_back(p.first);
            std::vector<FunctionSignature>& overload = functions[func->name];
            for(auto& existing : overload) {
                if(existing.parameterTypes == sig.parameterTypes){
                    diag.report("ERROR305", func->line, 1, "", func->name);
                    break; 
                }
            } 
            overload.push_back(sig);
    
        } else if (node->type == NodeType::CLASS_DEF) {
            auto* cls = static_cast<ClassDefNode*>(node.get());

            /*
            
            Register the class name and make sure another class with the same name 
            has not been declared. thus it stops duplicate declarations of Classes.

            */
            if (classes.find(cls->name) != classes.end()) {
                diag.report("ERROR401", cls->line, 1, "", cls->name);
            } else {
                classes[cls->name] = cls;
            }
        }
        else if (node->type == NodeType::EXTERN_DECL) {
            auto* extern_node = static_cast<ExternDeclNode*>(node.get());

            /*
            
            Register the external functions (FFI) using the same signature table used for normal functions,
            so calls to them can be resolved.

            */
            FunctionSignature sig;
            sig.returnType = extern_node->returnType;
            for (auto& p : extern_node->parameters) sig.parameterTypes.push_back(p.first);
            functions[extern_node->name].push_back(sig);
        }
        else if (node->type == NodeType::ENUM_DECL) {
            analyzeEnumDecl(node.get()); // analyze the enumerate declaration and register its members
        }
    }

    /*
    
    Once all global declarations are known and registered successfully, analyse their bodies (blocks of code they are possesing) 
    and their semantic details. 

    Here Enumerators are skipped because they were already handled during the first pass.

    */
    for (auto& node : program->globals)
        if (node->type != NodeType::ENUM_DECL) {
            analyzeNode(node.get());
        }

    // FInally analyze the run block in its own scope so variables declared inside it remains local to the run block (just like functions)
    // it is a main entry point of exectuion just like the main() function.
    if (program->runBlock) {
        symbolTable.pushScope();
        for (auto& node : program->runBlock->statements)
            analyzeNode(node.get());
        symbolTable.popScope();
    }
}

void SemanticAnalyzer::analyzeNode(ASTNode* node) {

    /*
    
    One can identify this function as the main dispatcher.
    it determines what kinda of AST node it has received and sends it to the corresponding semantic analysis funciton.
    this keeps the handling of different node types separate instead of putting all 
    the analysis in one large function. 

    so every node type gets it's own analysis function for maintainable code.
    
    */
    if (node->type == NodeType::VAR_DECL)               analyzeVarDecl(node);
    else if (node->type == NodeType::ARRAY_DECL)        analyzeArrayDecl(node);
    else if (node->type == NodeType::IF_STMT)           analyzeIfStmt(node);
    else if (node->type == NodeType::WHILE_STMT)        analyzeWhileStmt(node);
    else if (node->type == NodeType::DOWHILE_STMT)      analyzeDoWhileStmt(node);
    else if (node->type == NodeType::FOR_STMT)          analyzeForStmt(node);
    else if (node->type == NodeType::FUNC_DEF)          analyzeFuncDef(node);
    else if (node->type == NodeType::RETURN_STMT)       analyzeReturnStmt(node);
    else if (node->type == NodeType::BREAK_STMT)        analyzeBreakStmt(node);
    else if (node->type == NodeType::CONTINUE_STMT)     analyzeContinueStmt(node);
    else if (node->type == NodeType::SWITCH_STMT)       analyzeSwitchStmt(node);
    else if (node->type == NodeType::ENUM_DECL)         analyzeEnumDecl(node);
    else if (node->type == NodeType::FOR_IN_STMT)       analyzeForInStmt(node);
    else if (node->type == NodeType::CLASS_DEF)         analyzeClassDecl(node);

    // These nodes don't require any semantic analysis at this stage.
    else if (node->type == NodeType::PASS_STMT){}
    else if (node->type == NodeType::EXTERN_DECL){}

    // Expressions that can affect types are passed to the type checker so their types and other semantic rules can be validated.
    else if (node->type == NodeType::ASSIGNMENT_EXPR || node->type == NodeType::CALL_EXPR ||  node->type == NodeType::UNARY_EXPR)   
        typeChecker.analyzeExpression(node);
}