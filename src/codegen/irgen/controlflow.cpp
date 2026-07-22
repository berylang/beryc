#include "../codegen.h"
#include "../../parser/ast/controlflow.h"
#include "../../parser/ast/blocknode.h"
#include "../../parser/ast/expressions.h"
#include "../../parser/ast/literals.h"
#include "../../sema/symboltable.h"
#include <iostream>

void CodeGen::genBlock(ASTNode* node, std::ostream& out) {
    auto* block = static_cast<BlockNode*>(node);
    symTable.pushScope(); 
    pushGCScope();
    for (auto& stmt : block->statements) {
        genStatement(stmt.get(), out);
    }
    int roots = popGCScope();
    emitGCPops(roots, out);
    symTable.popScope();
}

void CodeGen::genIfStmt(ASTNode* node, std::ostream& out) {
    auto* ifStmt = static_cast<IfStmtNode*>(node);
    std::string conditionReg = genExpression(ifStmt->conditions.get(), "bool", out);
    int blockId = llvm.__uniqueId();
    std::string thenLabel = llvm.__labelWithId("if_then", blockId);
    std::string elseLabel = llvm.__labelWithId("if_else", blockId);
    std::string endLabel = llvm.__labelWithId("if_end", blockId);

    if (ifStmt->elseBranch) llvm.__emitCondBr(conditionReg, thenLabel, elseLabel, out);
    else llvm.__emitCondBr(conditionReg, thenLabel, endLabel, out);
    llvm.__emitLabel(thenLabel, out);
    genBlock(ifStmt->ifBranch.get(), out);
    llvm.__emitBr(endLabel, out);

    if (ifStmt->elseBranch) {
        llvm.__emitLabel(elseLabel, out);
        if (ifStmt->elseBranch->type == NodeType::IF_STMT) genIfStmt(ifStmt->elseBranch.get(), out);
        else genBlock(ifStmt->elseBranch.get(), out);
        llvm.__emitBr(endLabel, out);
    }
    llvm.__emitLabel(endLabel, out);
}

void CodeGen::genWhileStmt(ASTNode* node, std::ostream& out) {
    auto* whilestmt = static_cast<WhileStmtNode*>(node);
    int blockid = llvm.__uniqueId();
    std::string conditionLabel = llvm.__labelWithId("while_cond", blockid);
    std::string bodyLabel = llvm.__labelWithId("while_body", blockid);
    std::string endLabel = llvm.__labelWithId("while_end", blockid);

    breakTracker.push_back(endLabel);
    continueTracker.push_back(conditionLabel);
    llvm.__emitBr(conditionLabel, out);
    llvm.__emitLabel(conditionLabel, out);
    std::string conditionReg = genExpression(whilestmt->condition.get(), "bool", out);
    llvm.__emitCondBr(conditionReg, bodyLabel, endLabel, out);
    llvm.__emitLabel(bodyLabel, out);
    genBlock(whilestmt->body.get(), out);
    llvm.__emitBr(conditionLabel, out);
    llvm.__emitLabel(endLabel, out);
    continueTracker.pop_back();
    breakTracker.pop_back();
}

void CodeGen::genDoWhileStmt(ASTNode* node, std::ostream& out) {
    auto* dowhile = static_cast<DoWhileStmtNode*>(node);
    int blockId = llvm.__uniqueId();
    std::string bodyLabel = llvm.__labelWithId("dowhile_body", blockId);
    std::string conditionLabel = llvm.__labelWithId("dowhile_cond", blockId);
    std::string endLabel = llvm.__labelWithId("dowhile_end", blockId);
    breakTracker.push_back(endLabel);
    continueTracker.push_back(conditionLabel);
    llvm.__emitBr(bodyLabel, out);
    llvm.__emitLabel(bodyLabel, out);
    genBlock(dowhile->body.get(), out);
    continueTracker.pop_back();
    llvm.__emitBr(conditionLabel, out);
    llvm.__emitLabel(conditionLabel, out);
    std::string conditionReg = genExpression(dowhile->condition.get(), "bool", out);

    llvm.__emitCondBr(conditionReg, bodyLabel, endLabel, out);
    llvm.__emitLabel(endLabel, out);

    breakTracker.pop_back();
}


void CodeGen::genSwitchStmt(ASTNode* node, std::ostream& out) {
    auto* sw = static_cast<SwitchStmtNode*>(node);

    std::string conditionReg = genExpression(sw->condition.get(), "any", out);
    std::string condType = sw->condition->resolvedType; 
    if (condType.empty() || condType == "unknown") { condType = "int"; } 
    std::string llvmCondType = llvmType(condType);

    int blockId = llvm.__uniqueId();
    std::string endLabel = llvm.__labelWithId("switch_end", blockId);

    breakTracker.push_back(endLabel);
    std::string nextComparisonLabel = llvm.__indexedLabel("switch_cmp", 0, blockId);
    llvm.__emitBr(nextComparisonLabel, out);

    for (size_t i = 0; i < sw->cases.size(); i++) {
        auto& c = sw->cases[i];

        std::string bodyLabel = llvm.__indexedLabel("switch_body", i, blockId);
        std::string nextComparison = (i + 1 < sw->cases.size()) ? llvm.__indexedLabel("switch_cmp", i + 1, blockId) : (sw->hasDefault ? llvm.__labelWithId("switch_default", blockId) : endLabel);
        llvm.__emitLabel(nextComparisonLabel, out);
        std::string caseReg = genExpression(c.value.get(), "any", out);
        std::string isMatch = llvm.__emitBinaryOp("==", llvmCondType, false, conditionReg, caseReg, out);
        llvm.__emitCondBr(isMatch, bodyLabel, nextComparison, out);

        nextComparisonLabel = nextComparison;

        llvm.__emitLabel(bodyLabel, out);
        symTable.pushScope();
        pushGCScope();
        for (auto& stmt : c.statements) {
            genStatement(stmt.get(), out);
        }
        int roots = popGCScope();
        emitGCPops(roots, out);
        symTable.popScope();

        bool endsWithBreak = !c.statements.empty() && c.statements.back()->type == NodeType::BREAK_STMT;
        if (!endsWithBreak) {
            std::string nextBody = (i+1<sw->cases.size())?llvm.__indexedLabel("switch_body",i+1,blockId):(sw->hasDefault? llvm.__labelWithId("switch_default_body",blockId) : endLabel);
            llvm.__emitBr(nextBody, out);
        }
    }

    if (sw->hasDefault) {
        llvm.__emitLabel(llvm.__labelWithId("switch_default", blockId), out);
        llvm.__emitBr(llvm.__labelWithId("switch_default_body", blockId), out);
        llvm.__emitLabel(llvm.__labelWithId("switch_default_body", blockId), out);
        symTable.pushScope();
        pushGCScope();
        for (auto& stmt : sw->defaultBlock) {
            genStatement(stmt.get(), out);
        }
        int roots = popGCScope();
        emitGCPops(roots, out);
        symTable.popScope();

        bool endsWithBreak = !sw->defaultBlock.empty() && sw->defaultBlock.back()->type == NodeType::BREAK_STMT;
        if (!endsWithBreak) {
            llvm.__emitBr(endLabel, out);
        }
    } else {
        llvm.__emitLabel(nextComparisonLabel, out);
        llvm.__emitBr(endLabel, out);
    }

    llvm.__emitLabel(endLabel, out);
    breakTracker.pop_back();
}

void CodeGen::genBreakStmt(ASTNode* node, std::ostream& out) {
    if (breakTracker.empty()) return;
    llvm.__emitBr(breakTracker.back(), out);
}

void CodeGen::genContinueStmt(ASTNode* node, std::ostream& out) {
    if (continueTracker.empty()) return;
    llvm.__emitBr(continueTracker.back(), out);
}

void CodeGen::genForStmt(ASTNode* node, std::ostream& out) {
    auto* forStmt = static_cast<ForStmtNode*>(node);

    symTable.pushScope();
    pushGCScope();
    for (auto& initStmt : forStmt->init) {
        genStatement(initStmt.get(), out);
    }

    int blockid = llvm.__uniqueId();
    std::string conditionLabel = llvm.__labelWithId("for_cond", blockid);
    std::string bodyLabel = llvm.__labelWithId("for_body", blockid);
    std::string updateLabel = llvm.__labelWithId("for_update", blockid);
    std::string endLabel = llvm.__labelWithId("for_end", blockid);

    llvm.__emitBr(conditionLabel, out);
    llvm.__emitLabel(conditionLabel, out);
    if (forStmt->condition) {
        std::string conditionReg = genExpression(forStmt->condition.get(), "bool", out);
        llvm.__emitCondBr(conditionReg, bodyLabel, endLabel, out);
    } else {
        llvm.__emitBr(bodyLabel, out);
    }
    llvm.__emitLabel(bodyLabel, out);

    breakTracker.push_back(endLabel);
    continueTracker.push_back(updateLabel);

    genBlock(forStmt->body.get(), out);

    continueTracker.pop_back();
    breakTracker.pop_back();

    llvm.__emitBr(updateLabel, out);
    llvm.__emitLabel(updateLabel, out);
    for (auto& updateExpr : forStmt->update) {
        genExpression(updateExpr.get(), "any", out);
    }
    llvm.__emitBr(conditionLabel, out);
    llvm.__emitLabel(endLabel, out);
    symTable.popScope();
    int roots = popGCScope();
    emitGCPops(roots, out);
}


void CodeGen::genForInStmt(ASTNode* node, std::ostream& out) {
    auto* forIn = static_cast<ForInNode*>(node);
    symTable.pushScope();
    pushGCScope();

    if (forIn->rangeEnd) {
        std::string lt = "i32";
        bool isFloat = false;

        if (forIn->varType == "float" || forIn->varType == "double") {
            lt = "double";
            isFloat = true;
        } else if (forIn->varType == "char") {
            lt = "i8";
        } else if (forIn->varType == "bigint") {
            lt = "i64";
        }
        std::string varReg = llvm.__emitNamedAlloca(forIn->varName, lt, out);
        Symbol sym;
        sym.symbolType = SymbolType::VARIABLE;
        sym.type = forIn->varType;
        sym.isInitialized = true;
        sym.line = forIn->line;
        sym.llvmRegister = varReg;
        sym.llvmAllocType = lt;
        symTable.add(forIn->varName, sym);
        std::string startReg = genExpression(forIn->iterableOrStart.get(), lt, out);
        llvm.__emitStore(lt, startReg, varReg, out);

        std::string limitReg = genExpression(forIn->rangeEnd.get(), lt, out);
        std::string stepReg = isFloat ? "1.0" : "1";
        if (forIn->step) {
            stepReg = genExpression(forIn->step.get(), lt, out);
        }
        int blockid = llvm.__uniqueId();
        std::string conditionLabel = llvm.__labelWithId("forin_cond", blockid);
        std::string bodyLabel = llvm.__labelWithId("forin_body", blockid);
        std::string updateLabel = llvm.__labelWithId("forin_update", blockid);
        std::string endLabel = llvm.__labelWithId("forin_end", blockid);

        llvm.__emitBr(conditionLabel, out);
        llvm.__emitLabel(conditionLabel, out);
        std::string currValReg = llvm.__emitLoad(lt, varReg, out);
        std::string conditionReg = llvm.__emitBinaryOp("<=", lt, isFloat, currValReg, limitReg, out);

        llvm.__emitCondBr(conditionReg, bodyLabel, endLabel, out);
        llvm.__emitLabel(bodyLabel, out);
        breakTracker.push_back(endLabel);
        continueTracker.push_back(updateLabel);
        genBlock(forIn->body.get(), out);
        continueTracker.pop_back();
        breakTracker.pop_back();
        llvm.__emitBr(updateLabel, out);
        llvm.__emitLabel(updateLabel, out);
        std::string upLoadReg = llvm.__emitLoad(lt, varReg, out);
        std::string addReg = llvm.__emitBinaryOp("+", lt, isFloat, upLoadReg, stepReg, out);
        llvm.__emitStore(lt, addReg, varReg, out);
        llvm.__emitBr(conditionLabel, out);
        llvm.__emitLabel(endLabel, out);

    } else {
        std::string iterType = forIn->iterableOrStart->resolvedType;
        std::string lt = llvmType(forIn->varType);

        std::string idxType ="i32";
        std::string varReg =  llvm.__emitNamedAlloca(forIn->varName, lt,out);
        Symbol sym;
        sym.symbolType = SymbolType::VARIABLE;
        sym.type = forIn->varType;
        sym.isInitialized = true;
        sym.line = forIn->line;
        sym.llvmRegister = varReg;
        sym.llvmAllocType = lt;
        symTable.add(forIn->varName, sym);

        std::string idxReg = llvm.__emitNamedAlloca("forin_idx", idxType, out);
        llvm.__emitStore(idxType, "0", idxReg, out);

        int blockid = llvm.__uniqueId();
        std::string conditionLabel = llvm.__labelWithId("forin_cond", blockid);
        std::string bodyLabel = llvm.__labelWithId("forin_body", blockid);
        std::string updateLabel = llvm.__labelWithId("forin_update", blockid);
        std::string endLabel = llvm.__labelWithId("forin_end", blockid);

        if (iterType.size() > 6 && iterType.substr(0, 6) == "array<") {
            std::string elementType = iterType.substr(6, iterType.size() - 7);
            std::string elementLlvmType = llvmType(elementType);
            llvm.__declareExternFn("i64", "bery_array_length", {"i8*"});
            llvm.__declareExternFn("i8*", "bery_array_get", {"i8*", "i64*"});
            std::string arrReg = genExpression(forIn->iterableOrStart.get(), iterType, out);
            std::string arrHold = llvm.__emitNamedAlloca("forin_arr", "i8*", out);
            llvm.__emitStore("i8*", arrReg, arrHold, out);
            emitGCPush(arrHold, "i8*", out);

            std::string lenReg64 = llvm.__emitCall("i64", "bery_array_length", {{"i8*", arrReg}}, out);
            std::string limitReg = llvm.__emitConvert("trunc", "i64", lenReg64, "i32", out);
            llvm.__emitBr(conditionLabel, out);
            llvm.__emitLabel(conditionLabel, out);

            std::string currIdxReg = llvm.__emitLoad(idxType, idxReg, out);
            std::string conditionReg = llvm.__emitBinaryOp("<", idxType, false, currIdxReg, limitReg, out);
            llvm.__emitCondBr(conditionReg, bodyLabel, endLabel, out);
            llvm.__emitLabel(bodyLabel, out);

            std::string arrLoadReg = llvm.__emitLoad("i8*", arrHold, out);
            std::string indexExt = llvm.__emitSext("i32", currIdxReg, "i64", out);
            std::string rawReg = llvm.__emitCall("i8*", "bery_array_get", {{"i8*", arrLoadReg}, {"i64", indexExt}}, out);
            std::string castReg = llvm.__emitBitcast("i8*", rawReg, elementLlvmType + "*", out);
            std::string valReg = llvm.__emitLoad(elementLlvmType, castReg, out);
            std::string finalReg = valReg;
            if (lt != elementLlvmType) {
                if ((elementLlvmType == "i32" || elementLlvmType == "i64") && (lt == "float" || lt == "double")) {
                    finalReg = llvm.__emitConvert("sitofp", elementLlvmType, valReg, lt, out);
                } else if (elementLlvmType == "i32" && lt == "i64") {
                    finalReg = llvm.__emitConvert("sext", "i32", valReg, "i64", out);
                } else if (elementLlvmType == "float" && lt == "double") {
                    finalReg = llvm.__emitConvert("fpext", "float", valReg, "double", out);
                }
            }
            llvm.__emitStore(lt, finalReg, varReg, out);

            breakTracker.push_back(endLabel);
            continueTracker.push_back(updateLabel);
            genBlock(forIn->body.get(), out);
            continueTracker.pop_back();
            breakTracker.pop_back();
            llvm.__emitBr(updateLabel, out);

            llvm.__emitLabel(updateLabel, out);
            std::string upLoadIdx = llvm.__emitLoad(idxType, idxReg, out);
            std::string addIdx = llvm.__emitBinaryOp("+", idxType, false, upLoadIdx, "1", out);
            llvm.__emitStore(idxType, addIdx, idxReg, out);
            llvm.__emitBr(conditionLabel, out);
            llvm.__emitLabel(endLabel, out);

        } else if (iterType == "string") {
            llvm.__declareExternFn("i64", "bery_string_length", {"i8*"});
            llvm.__declareExternFn("i8*","bery_string_char_at", {"i8*", "i64*"});

            std::string strReg = genExpression(forIn->iterableOrStart.get(), "string", out);
            std::string strHold = llvm.__emitNamedAlloca("forin_str", "i8*", out);
            llvm.__emitStore("i8*", strReg, strHold, out);
            emitGCPush(strHold, "i8*", out);

            std::string lenReg64 = llvm.__emitCall("i64", "bery_string_length", {{"i8*", strReg}}, out);
            std::string limitReg = llvm.__emitConvert("trunc", "i64", lenReg64, "i32", out);
            llvm.__emitBr(conditionLabel, out);
            llvm.__emitLabel(conditionLabel, out);

            std::string currIdxReg = llvm.__emitLoad(idxType, idxReg, out);
            std::string conditionReg = llvm.__emitBinaryOp("<", idxType, false, currIdxReg, limitReg, out);
            llvm.__emitCondBr(conditionReg, bodyLabel, endLabel, out);
            llvm.__emitLabel(bodyLabel, out);

            std::string strLoadReg = llvm.__emitLoad("i8*", strHold, out);
            std::string indexExt = llvm.__emitSext("i32", currIdxReg, "i64", out);

            std::string charReg = llvm.__emitCall("i8", "bery_string_char_at", {{"i8*", strLoadReg}, {"i64", indexExt}}, out);
            llvm.__emitStore("i8", charReg, varReg, out);

            breakTracker.push_back(endLabel);
            continueTracker.push_back(updateLabel);
            genBlock(forIn->body.get(), out);
            continueTracker.pop_back();
            breakTracker.pop_back();
            llvm.__emitBr(updateLabel, out);

            llvm.__emitLabel(updateLabel, out);
            std::string upLoadIdx = llvm.__emitLoad(idxType, idxReg, out);
            std::string addIdx = llvm.__emitBinaryOp("+", idxType, false, upLoadIdx, "1", out);
            llvm.__emitStore(idxType, addIdx, idxReg, out);
            llvm.__emitBr(conditionLabel, out);
            llvm.__emitLabel(endLabel, out);

        } else {
            auto* identNode = static_cast<IdentNode*>(forIn->iterableOrStart.get());
            std::string arrName = identNode->name;
            std::string arrPtr = genExpression(forIn->iterableOrStart.get(), "ptr", out);

            int arrSize = symTable.get(arrName).arraySize;
            std::string limitReg = std::to_string(arrSize);

            llvm.__emitBr(conditionLabel, out);
            llvm.__emitLabel(conditionLabel, out);
            std::string currIdxReg = llvm.__emitLoad(idxType, idxReg, out);

            std::string conditionReg = llvm.__emitBinaryOp("<", idxType, false, currIdxReg, limitReg, out);
            llvm.__emitCondBr(conditionReg, bodyLabel, endLabel, out);

            llvm.__emitLabel(bodyLabel, out);

            std::string gepReg = llvm.__emitTypedGEP(lt, arrPtr, {{idxType, currIdxReg}}, true, out);
            std::string valReg = llvm.__emitLoad(lt, gepReg, out);
            llvm.__emitStore(lt, valReg, varReg, out);

            breakTracker.push_back(endLabel);
            continueTracker.push_back(updateLabel);

            genBlock(forIn->body.get(), out);

            continueTracker.pop_back();
            breakTracker.pop_back();
            llvm.__emitBr(updateLabel, out);

            llvm.__emitLabel(updateLabel, out);
            std::string upLoadIdx = llvm.__emitLoad(idxType, idxReg, out);

            std::string addIdx = llvm.__emitBinaryOp("+", idxType, false, upLoadIdx, "1", out);
            llvm.__emitStore(idxType, addIdx, idxReg, out);

            llvm.__emitBr(conditionLabel, out);
            llvm.__emitLabel(endLabel, out);
        }
    }

    symTable.popScope();
    int roots = popGCScope();
    emitGCPops(roots, out);
}