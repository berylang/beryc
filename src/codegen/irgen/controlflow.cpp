#include "../codegen.h"
#include "../../parser/ast/controlflow.h"
#include "../../parser/ast/blocknode.h"
#include "../../parser/ast/expressions.h"
#include "../../sema/symboltable.h"
#include <iostream>

void CodeGen::genBlock(ASTNode* node, std::ostream& out) {
    auto* block = static_cast<BlockNode*>(node);
    symTable.pushScope(); 
    for (auto& stmt : block->statements) {
        genStatement(stmt.get(), out);
    }
    symTable.popScope();
}

void CodeGen::genIfStmt(ASTNode* node, std::ostream& out) {
    auto* ifStmt = static_cast<IfStmtNode*>(node);
    std::string condReg = genExpression(ifStmt->conditions.get(), "bool", out);

    int blockId = ++regCounter;
    std::string thenLbl = "if_then_" + std::to_string(blockId);
    std::string elseLbl = "if_else_" + std::to_string(blockId);
    std::string endLbl = "if_end_" + std::to_string(blockId);

    if (ifStmt->elseBranch) out << "    br i1 " << condReg << ", label %" << thenLbl << ", label %" << elseLbl << "\n";
    else out << "    br i1 " << condReg << ", label %" << thenLbl << ", label %" << endLbl << "\n";

    out << "\n" << thenLbl << ":\n";
    genBlock(ifStmt->ifBranch.get(), out);
    out << "    br label %" << endLbl << "\n"; 

    if (ifStmt->elseBranch) {
        out << "\n" << elseLbl << ":\n";
        if (ifStmt->elseBranch->type == NodeType::IF_STMT) genIfStmt(ifStmt->elseBranch.get(), out); 
        else genBlock(ifStmt->elseBranch.get(), out); 
        out << "    br label %" << endLbl << "\n"; 
    }

    out << "\n" << endLbl << ":\n";
}

void CodeGen::genWhileStmt(ASTNode* node, std::ostream& out) {
    auto* whilestmt = static_cast<WhileStmtNode*>(node);
    int blockid = ++regCounter;
    std::string conditionLabel = "while_cond_"+std::to_string(blockid);
    std::string bodyLabel = "while_body_"+std::to_string(blockid);
    std::string endLabel = "while_end_"+std::to_string(blockid);
    
    breakTracker.push_back(endLabel);
    continueTracker.push_back(conditionLabel);
    out << "    br label %" << conditionLabel << "\n";
    out << "\n" << conditionLabel << ":\n";
    std::string conditionReg = genExpression(whilestmt->condition.get(), "bool", out);
    out << "    br i1 " << conditionReg << ", label %" << bodyLabel << ", label %" << endLabel << "\n";
    //br i1 %13, label %while_body_15, label %while_end_18
    out << "\n" << bodyLabel << ":\n";
    genBlock(whilestmt->body.get(), out);
    out << "    br label %" << conditionLabel << "\n";
    out <<"\n" << endLabel << ":\n";
    continueTracker.pop_back();
    breakTracker.pop_back();
}

void CodeGen::genDoWhileStmt(ASTNode* node, std::ostream& out) {
    auto* dowhile = static_cast<DoWhileStmtNode*>(node);
    int blockId = ++regCounter;
    
    std::string bodyLbl = "dowhile_body_" + std::to_string(blockId);
    std::string condLbl = "dowhile_cond_" + std::to_string(blockId);
    std::string endLbl = "dowhile_end_" + std::to_string(blockId);

    breakTracker.push_back(endLbl);
    continueTracker.push_back(condLbl);
    out << "    br label %" << bodyLbl << "\n";
    out << "\n" << bodyLbl << ":\n";
    genBlock(dowhile->body.get(), out);
    continueTracker.pop_back();
    out << "    br label %" << condLbl << "\n";
    out << "\n" << condLbl << ":\n";
    std::string condReg = genExpression(dowhile->condition.get(), "bool", out);
    
    out << "    br i1 " << condReg << ", label %" << bodyLbl << ", label %" << endLbl << "\n";
    out << "\n" << endLbl << ":\n";
    
    breakTracker.pop_back();
}

void CodeGen::genSwitchStmt(ASTNode* node, std::ostream& out) {
    auto* sw = static_cast<SwitchStmtNode*>(node);

    std::string condReg = genExpression(sw->condition.get(), "any", out);
    std::string llvmCondType = "i32";
    
    int blockId = ++regCounter;
    std::string endLbl = "switch_end_" + std::to_string(blockId);
    
    breakTracker.push_back(endLbl);
    std::string nextCmpLbl = "switch_cmp_0_" + std::to_string(blockId);
    out << "    br label %" << nextCmpLbl << "\n";

    for (size_t i = 0; i < sw->cases.size(); i++) {
        auto& c = sw->cases[i];
        
        std::string bodyLbl = "switch_body_" + std::to_string(i) + "_" + std::to_string(blockId);
        std::string nextCmp = (i + 1 < sw->cases.size()) ? "switch_cmp_" + std::to_string(i + 1) + "_" + std::to_string(blockId) : (sw->hasDefault ? "switch_default_" + std::to_string(blockId) : endLbl);
        out << "\n" << nextCmpLbl << ":\n";
        std::string caseReg = genExpression(c.value.get(), "any", out);
        std::string isMatch = newReg();
        out << "    " << isMatch << " = icmp eq " << llvmCondType << " " << condReg << ", " << caseReg << "\n";
        out << "    br i1 " << isMatch << ", label %" << bodyLbl << ", label %" << nextCmp << "\n";
        
        nextCmpLbl = nextCmp;

        out << "\n" << bodyLbl << ":\n";
        symTable.pushScope();
        for (auto& stmt : c.statements) {
            genStatement(stmt.get(), out);
        }
        symTable.popScope();
        
        bool endsWithBreak = !c.statements.empty() && c.statements.back()->type == NodeType::BREAK_STMT;
        if (!endsWithBreak) {
            std::string nextBody = (i + 1 < sw->cases.size()) ? "switch_body_" + std::to_string(i + 1) + "_" + std::to_string(blockId) : (sw->hasDefault ? "switch_default_body_" + std::to_string(blockId) : endLbl);
            out << "    br label %" << nextBody << "\n";
        }
    }

    if (sw->hasDefault) {
        out << "\nswitch_default_" << blockId << ":\n";
        out << "    br label %switch_default_body_" << blockId << "\n";
        out << "\nswitch_default_body_" << blockId << ":\n";
        symTable.pushScope();
        for (auto& stmt : sw->defaultBlock) {
            genStatement(stmt.get(), out);
        }
        symTable.popScope();
        
        bool endsWithBreak = !sw->defaultBlock.empty() && sw->defaultBlock.back()->type == NodeType::BREAK_STMT;
        if (!endsWithBreak) {
             out << "    br label %" << endLbl << "\n";
        }
    } else {
        out << "\n" << nextCmpLbl << ":\n";
        out << "    br label %" << endLbl << "\n";
    }

    out << "\n" << endLbl << ":\n";
    breakTracker.pop_back(); 
}

void CodeGen::genBreakStmt(ASTNode* node, std::ostream& out) {
    if (breakTracker.empty()) return;
    out << "    br label %" << breakTracker.back() << "\n";
}

void CodeGen::genContinueStmt(ASTNode* node, std::ostream& out) {
    if (continueTracker.empty()) return;
    out << "    br label %" << continueTracker.back() << "\n";
}