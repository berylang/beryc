#include "sema.h"

/*

    Semantic Analyzer, Declarations,

    this file analyze - 
        if statements,
        switch statements,
        for loops,
        for in loops
        while loops
        do-while loops, 
        and continue / break statements


*/
#include <iostream>
#include "../parser/ast/blocknode.h"
#include "../parser/ast/controlflow.h"


void SemanticAnalyzer::analyzeBlock(ASTNode* node) { 
    auto* block = static_cast<BlockNode*>(node);
    symbolTable.pushScope();
    
    for(auto& statement:block->statements){
        analyzeNode(statement.get());
    }

    symbolTable.popScope();
}

void SemanticAnalyzer::analyzeIfStmt(ASTNode* node) { 
    auto* ifStmt = static_cast<IfStmtNode*>(node);
    std::string conditionType = typeChecker.analyzeExpression(ifStmt->conditions.get());

    if(conditionType != "bool" && conditionType != "unknown"){
        diag.report("ERROR306", ifStmt->line, 1, "", "");
        
    }

    analyzeBlock(ifStmt->ifBranch.get());

    if(ifStmt->elseBranch){
        if(ifStmt->elseBranch->type == NodeType::IF_STMT){
            analyzeIfStmt(ifStmt->elseBranch.get());
        }
        else{
            analyzeBlock(ifStmt->elseBranch.get());
        }
    }
}

void SemanticAnalyzer::analyzeSwitchStmt(ASTNode* node) {
    auto* sw = static_cast<SwitchStmtNode*>(node);
    std::string condType = typeChecker.analyzeExpression(sw->condition.get());

    if (condType != "unknown" && condType != "int" && condType != "bigint" && condType != "char") {
        diag.report("ERROR307", sw->line, 1, "", condType);
        
    }

    loopOrSwitchDepth++;

    for (auto& c : sw->cases) {
        if (c.value) {
            std::string caseType = typeChecker.analyzeExpression(c.value.get());
            if (caseType != "unknown" && condType != "unknown" && caseType != condType) {
                diag.report("ERROR308", sw->line, 1, "", caseType + "' does not match switch condition type '" + condType);
                
            }
        }
        
        symbolTable.pushScope();
        for (auto& s : c.statements) analyzeNode(s.get());
        symbolTable.popScope();
    }

    if (sw->hasDefault) {
        symbolTable.pushScope();
        for (auto& s : sw->defaultBlock) analyzeNode(s.get());
        symbolTable.popScope();
    }

    loopOrSwitchDepth--;
}

void SemanticAnalyzer::analyzeBreakStmt(ASTNode* node) {
    if (loopOrSwitchDepth <= 0) {
        diag.report("ERROR309", node->line, 1, "", "");
        
    }
}

void SemanticAnalyzer::analyzeContinueStmt(ASTNode* node) {
    if (loopDepth <= 0){
        diag.report("ERROR310", node->line, 1, "", "");
        
    }
}

void SemanticAnalyzer::analyzeWhileStmt(ASTNode* node){
    auto* whileStmt = static_cast<WhileStmtNode*>(node);
    std::string conditionType = typeChecker.analyzeExpression(whileStmt->condition.get());

    if(conditionType != "bool" && conditionType != "unknown"){
        diag.report("ERROR311", whileStmt->line, 1, "", "");
        
    }

    loopOrSwitchDepth++;
    loopDepth++;
    analyzeBlock(whileStmt->body.get());
    loopDepth--;
    loopOrSwitchDepth--;
}

void SemanticAnalyzer::analyzeDoWhileStmt(ASTNode* node){
    auto* dowhilestmt = static_cast<DoWhileStmtNode*>(node);
    std::string conditionType = typeChecker.analyzeExpression(dowhilestmt->condition.get());

    if(conditionType != "bool" && conditionType != "unknown"){
        diag.report("ERROR311", dowhilestmt->line, 1, "", "");
        
    }

    loopOrSwitchDepth++;
    loopDepth++;
    analyzeBlock(dowhilestmt->body.get());
    loopDepth--;
    loopOrSwitchDepth--;
}

void SemanticAnalyzer::analyzeForStmt(ASTNode* node) {
    auto* forStmt = static_cast<ForStmtNode*>(node);
    symbolTable.pushScope();
    
    for (auto& initStmt : forStmt->init) { 
        analyzeNode(initStmt.get());
    }
    
    if (forStmt->condition) {
        std::string condType = typeChecker.analyzeExpression(forStmt->condition.get());
        if (condType != "bool" && condType != "unknown") {
            diag.report("ERROR312", forStmt->line, 1, "", "");
            
        }
    }
    
    for (auto& updateExpr : forStmt->update) {
        typeChecker.analyzeExpression(updateExpr.get());
    }
    loopDepth++; loopOrSwitchDepth++;
    analyzeBlock(forStmt->body.get()); 
    loopDepth--; loopOrSwitchDepth--;
    symbolTable.popScope();
}

void SemanticAnalyzer::analyzeForInStmt(ASTNode* node) {
    auto* forIn = static_cast<ForInNode*>(node);
    symbolTable.pushScope();

    std::string actualVarType = forIn->varType;
    if (forIn->rangeEnd) {
        std::string startType = typeChecker.analyzeExpression(forIn->iterableOrStart.get());
        std::string endType = typeChecker.analyzeExpression(forIn->rangeEnd.get());
        if (actualVarType == "unknown" || actualVarType == "") {
            actualVarType = (startType != "unknown") ? startType : "int"; 
        }
    } else {
        std::string iterType = typeChecker.analyzeExpression(forIn->iterableOrStart.get());
        std::string elementType = "unknown";
        
        //array<int> 
        if (iterType.size() > 6 && iterType.substr(0,6)=="array<" && iterType.back()=='>'){
            elementType = iterType.substr(6,iterType.size()-7);
        } else if(iterType == "string"){
            elementType = "char";
        } else if(iterType != "unknown") {
            diag.report("ERROR313", forIn->line, 1, "", iterType);
            
        }
        if(actualVarType == "unknown" || actualVarType == ""){
            actualVarType = elementType;
        } else if(elementType!="unknown" && actualVarType!=elementType){
            bool compatible = (actualVarType == "float" && elementType == "int") || (actualVarType == "double" && elementType == "int") || (actualVarType == "double" && elementType == "float") || (actualVarType == "bigint" && elementType == "int");
            if(!compatible){
                diag.report("ERROR314", forIn->line, 1, "", forIn->varName + "' declared as '" + actualVarType + "' but iterable has element type '" + elementType);
                
            }
        }
    }

    forIn->varType = actualVarType;
    
    if (forIn->step) {
        typeChecker.analyzeExpression(forIn->step.get());
    }
    symbolTable.addVariable(forIn->varName, actualVarType, false, true, forIn->line);

    loopDepth++; 
    loopOrSwitchDepth++;
    analyzeBlock(forIn->body.get());
    loopDepth--; 
    loopOrSwitchDepth--;

    symbolTable.popScope();
}
