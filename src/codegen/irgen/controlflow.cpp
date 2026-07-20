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
    std::string condReg = genExpression(ifStmt->conditions.get(), "bool", out);
    int blockId = llvm.__uniqueId();
    std::string thenLbl = "if_then_" + std::to_string(blockId);
    std::string elseLbl = "if_else_" + std::to_string(blockId);
    std::string endLbl = "if_end_" + std::to_string(blockId);

    if (ifStmt->elseBranch) llvm.__emitCondBr(condReg, thenLbl, elseLbl, out);
    else llvm.__emitCondBr(condReg, thenLbl, endLbl, out);
    llvm.__emitLabel(thenLbl, out);
    genBlock(ifStmt->ifBranch.get(), out);
    llvm.__emitBr(endLbl, out);

    if (ifStmt->elseBranch) {
        llvm.__emitLabel(elseLbl, out);
        if (ifStmt->elseBranch->type == NodeType::IF_STMT) genIfStmt(ifStmt->elseBranch.get(), out);
        else genBlock(ifStmt->elseBranch.get(), out);
        llvm.__emitBr(endLbl, out);
    }
    llvm.__emitLabel(endLbl, out);
}

void CodeGen::genWhileStmt(ASTNode* node, std::ostream& out) {
    auto* whilestmt = static_cast<WhileStmtNode*>(node);
    int blockid = llvm.__uniqueId();
    std::string conditionLabel = "while_cond_" + std::to_string(blockid);
    std::string bodyLabel = "while_body_" + std::to_string(blockid);
    std::string endLabel = "while_end_" + std::to_string(blockid);

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

    std::string bodyLbl = "dowhile_body_" + std::to_string(blockId);
    std::string condLbl = "dowhile_cond_" + std::to_string(blockId);
    std::string endLbl = "dowhile_end_" + std::to_string(blockId);

    breakTracker.push_back(endLbl);
    continueTracker.push_back(condLbl);
    llvm.__emitBr(bodyLbl, out);
    llvm.__emitLabel(bodyLbl, out);
    genBlock(dowhile->body.get(), out);
    continueTracker.pop_back();
    llvm.__emitBr(condLbl, out);
    llvm.__emitLabel(condLbl, out);
    std::string condReg = genExpression(dowhile->condition.get(), "bool", out);

    llvm.__emitCondBr(condReg, bodyLbl, endLbl, out);
    llvm.__emitLabel(endLbl, out);

    breakTracker.pop_back();
}
void CodeGen::genSwitchStmt(ASTNode* node, std::ostream& out) {
    auto* sw = static_cast<SwitchStmtNode*>(node);

    std::string condReg = genExpression(sw->condition.get(), "any", out);
    std::string condType = sw->condition->resolvedType; 
    if (condType.empty() || condType == "unknown") { condType = "int"; } 
    std::string llvmCondType = llvmType(condType);

    int blockId = llvm.__uniqueId();
    std::string endLbl = "switch_end_" + std::to_string(blockId);

    breakTracker.push_back(endLbl);
    std::string nextCmpLbl = "switch_cmp_0_" + std::to_string(blockId);
    llvm.__emitBr(nextCmpLbl, out);

    for (size_t i = 0; i < sw->cases.size(); i++) {
        auto& c = sw->cases[i];

        std::string bodyLbl = "switch_body_" + std::to_string(i) + "_" + std::to_string(blockId);
        std::string nextCmp = (i + 1 < sw->cases.size()) ? "switch_cmp_" + std::to_string(i + 1) + "_" + std::to_string(blockId) : (sw->hasDefault ? "switch_default_" + std::to_string(blockId) : endLbl);
        llvm.__emitLabel(nextCmpLbl, out);
        std::string caseReg = genExpression(c.value.get(), "any", out);
        std::string isMatch = llvm.__emitBinaryOp("==", llvmCondType, false, condReg, caseReg, out);
        llvm.__emitCondBr(isMatch, bodyLbl, nextCmp, out);

        nextCmpLbl = nextCmp;

        llvm.__emitLabel(bodyLbl, out);
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
            std::string nextBody = (i + 1 < sw->cases.size()) ? "switch_body_" + std::to_string(i + 1) + "_" + std::to_string(blockId) : (sw->hasDefault ? "switch_default_body_" + std::to_string(blockId) : endLbl);
            llvm.__emitBr(nextBody, out);
        }
    }

    if (sw->hasDefault) {
        llvm.__emitLabel("switch_default_" + std::to_string(blockId), out);
        llvm.__emitBr("switch_default_body_" + std::to_string(blockId), out);
        llvm.__emitLabel("switch_default_body_" + std::to_string(blockId), out);
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
            llvm.__emitBr(endLbl, out);
        }
    } else {
        llvm.__emitLabel(nextCmpLbl, out);
        llvm.__emitBr(endLbl, out);
    }

    llvm.__emitLabel(endLbl, out);
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
    std::string condLbl = "for_cond_" + std::to_string(blockid);
    std::string bodyLbl = "for_body_" + std::to_string(blockid);
    std::string updateLbl = "for_update_" + std::to_string(blockid);
    std::string endLbl = "for_end_" + std::to_string(blockid);

    llvm.__emitBr(condLbl, out);
    llvm.__emitLabel(condLbl, out);
    if (forStmt->condition) {
        std::string condReg = genExpression(forStmt->condition.get(), "bool", out);
        llvm.__emitCondBr(condReg, bodyLbl, endLbl, out);
    } else {
        llvm.__emitBr(bodyLbl, out);
    }
    llvm.__emitLabel(bodyLbl, out);

    breakTracker.push_back(endLbl);
    continueTracker.push_back(updateLbl);

    genBlock(forStmt->body.get(), out);

    continueTracker.pop_back();
    breakTracker.pop_back();

    llvm.__emitBr(updateLbl, out);
    llvm.__emitLabel(updateLbl, out);
    for (auto& updateExpr : forStmt->update) {
        genExpression(updateExpr.get(), "any", out);
    }
    llvm.__emitBr(condLbl, out);
    llvm.__emitLabel(endLbl, out);
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
        std::string varReg = "%" + forIn->varName + "_" + std::to_string(llvm.__uniqueId());
        Symbol sym;
        sym.symbolType = SymbolType::VARIABLE;
        sym.type = forIn->varType;
        sym.isInitialized = true;
        sym.line = forIn->line;
        sym.llvmRegister = varReg;
        sym.llvmAllocType = lt;
        symTable.add(forIn->varName, sym);
        out << "    " << varReg << " = alloca " << lt << "\n";
        std::string startReg = genExpression(forIn->iterableOrStart.get(), lt, out);
        llvm.__emitStore(lt, startReg, varReg, out);

        std::string limitReg = genExpression(forIn->rangeEnd.get(), lt, out);
        std::string stepReg = isFloat ? "1.0" : "1";
        if (forIn->step) {
            stepReg = genExpression(forIn->step.get(), lt, out);
        }
        int blockid = llvm.__uniqueId();
        std::string condLbl = "forin_cond_" + std::to_string(blockid);
        std::string bodyLbl = "forin_body_" + std::to_string(blockid);
        std::string updateLbl = "forin_update_" + std::to_string(blockid);
        std::string endLbl = "forin_end_" + std::to_string(blockid);

        llvm.__emitBr(condLbl, out);
        llvm.__emitLabel(condLbl, out);
        std::string currValReg = llvm.__emitLoad(lt, varReg, out);
        std::string condReg = llvm.__emitBinaryOp("<=", lt, isFloat, currValReg, limitReg, out);

        llvm.__emitCondBr(condReg, bodyLbl, endLbl, out);
        llvm.__emitLabel(bodyLbl, out);
        breakTracker.push_back(endLbl);
        continueTracker.push_back(updateLbl);
        genBlock(forIn->body.get(), out);
        continueTracker.pop_back();
        breakTracker.pop_back();
        llvm.__emitBr(updateLbl, out);
        llvm.__emitLabel(updateLbl, out);
        std::string upLoadReg = llvm.__emitLoad(lt, varReg, out);
        std::string addReg = llvm.__emitBinaryOp("+", lt, isFloat, upLoadReg, stepReg, out);
        llvm.__emitStore(lt, addReg, varReg, out);
        llvm.__emitBr(condLbl, out);
        llvm.__emitLabel(endLbl, out);

    } else {
        std::string iterType = forIn->iterableOrStart->resolvedType;
        std::string lt = llvmType(forIn->varType);

        std::string idxType = "i32";
        std::string varReg = "%" + forIn->varName + "_" + std::to_string(llvm.__uniqueId());
        Symbol sym;
        sym.symbolType = SymbolType::VARIABLE;
        sym.type = forIn->varType;
        sym.isInitialized = true;
        sym.line = forIn->line;
        sym.llvmRegister = varReg;
        sym.llvmAllocType = lt;
        symTable.add(forIn->varName, sym);
        out << "    " << varReg << " = alloca " << lt << "\n";

        std::string idxReg = "%forin_idx_" + std::to_string(llvm.__uniqueId());
        out << "    " << idxReg << " = alloca " << idxType << "\n";
        llvm.__emitStore(idxType, "0", idxReg, out);

        int blockid = llvm.__uniqueId();
        std::string condLbl = "forin_cond_" + std::to_string(blockid);
        std::string bodyLbl = "forin_body_" + std::to_string(blockid);
        std::string updateLbl = "forin_update_" + std::to_string(blockid);
        std::string endLbl = "forin_end_" + std::to_string(blockid);

        if (iterType.size() > 6 && iterType.substr(0, 6) == "array<") {
            std::string elementType = iterType.substr(6, iterType.size() - 7);
            std::string elementLlvmType = llvmType(elementType);
            llvm.__declareExtern("declare i64 @bery_array_length(i8*)", "bery_array_length");
            llvm.__declareExtern("declare i8* @bery_array_get(i8*, i64*)", "bery_array_get");
            std::string arrReg = genExpression(forIn->iterableOrStart.get(), iterType, out);
            std::string arrHold = "%forin_arr_" + std::to_string(llvm.__uniqueId());

            out << "    " << arrHold << " = alloca i8*\n";
            llvm.__emitStore("i8*", arrReg, arrHold, out);
            emitGCPush(arrHold, "i8*", out);

            std::string lenReg64 = llvm.__emitCall("i64", "bery_array_length", {{"i8*", arrReg}}, out);
            std::string limitReg = llvm.__emitConvert("trunc", "i64", lenReg64, "i32", out);
            llvm.__emitBr(condLbl, out);
            llvm.__emitLabel(condLbl, out);

            std::string currIdxReg = llvm.__emitLoad(idxType, idxReg, out);
            std::string condReg = llvm.__emitBinaryOp("<", idxType, false, currIdxReg, limitReg, out);
            llvm.__emitCondBr(condReg, bodyLbl, endLbl, out);
            llvm.__emitLabel(bodyLbl, out);

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

            breakTracker.push_back(endLbl);
            continueTracker.push_back(updateLbl);
            genBlock(forIn->body.get(), out);
            continueTracker.pop_back();
            breakTracker.pop_back();
            llvm.__emitBr(updateLbl, out);

            llvm.__emitLabel(updateLbl, out);
            std::string upLoadIdx = llvm.__emitLoad(idxType, idxReg, out);
            std::string addIdx = llvm.__emitBinaryOp("+", idxType, false, upLoadIdx, "1", out);
            llvm.__emitStore(idxType, addIdx, idxReg, out);
            llvm.__emitBr(condLbl, out);
            llvm.__emitLabel(endLbl, out);

        } else if (iterType == "string") {
            llvm.__declareExtern("declare i64 @bery_string_length(i8*)", "bery_string_length");
            llvm.__declareExtern("declare i8* @bery_string_char_at(i8*, i64*)", "bery_string_char_at");

            std::string strReg = genExpression(forIn->iterableOrStart.get(), "string", out);
            std::string strHold = "%forin_str_" + std::to_string(llvm.__uniqueId());

            out << "    " << strHold << " = alloca i8*\n";
            llvm.__emitStore("i8*", strReg, strHold, out);
            emitGCPush(strHold, "i8*", out);

            std::string lenReg64 = llvm.__emitCall("i64", "bery_string_length", {{"i8*", strReg}}, out);
            std::string limitReg = llvm.__emitConvert("trunc", "i64", lenReg64, "i32", out);
            llvm.__emitBr(condLbl, out);
            llvm.__emitLabel(condLbl, out);

            std::string currIdxReg = llvm.__emitLoad(idxType, idxReg, out);
            std::string condReg = llvm.__emitBinaryOp("<", idxType, false, currIdxReg, limitReg, out);
            llvm.__emitCondBr(condReg, bodyLbl, endLbl, out);
            llvm.__emitLabel(bodyLbl, out);

            std::string strLoadReg = llvm.__emitLoad("i8*", strHold, out);
            std::string indexExt = llvm.__emitSext("i32", currIdxReg, "i64", out);

            std::string charReg = llvm.__emitCall("i8", "bery_string_char_at", {{"i8*", strLoadReg}, {"i64", indexExt}}, out);
            llvm.__emitStore("i8", charReg, varReg, out);

            breakTracker.push_back(endLbl);
            continueTracker.push_back(updateLbl);
            genBlock(forIn->body.get(), out);
            continueTracker.pop_back();
            breakTracker.pop_back();
            llvm.__emitBr(updateLbl, out);

            llvm.__emitLabel(updateLbl, out);
            std::string upLoadIdx = llvm.__emitLoad(idxType, idxReg, out);
            std::string addIdx = llvm.__emitBinaryOp("+", idxType, false, upLoadIdx, "1", out);
            llvm.__emitStore(idxType, addIdx, idxReg, out);
            llvm.__emitBr(condLbl, out);
            llvm.__emitLabel(endLbl, out);

        } else {
            auto* identNode = static_cast<IdentNode*>(forIn->iterableOrStart.get());
            std::string arrName = identNode->name;
            std::string arrPtr = genExpression(forIn->iterableOrStart.get(), "ptr", out);

            int arrSize = symTable.get(arrName).arraySize;
            std::string limitReg = std::to_string(arrSize);

            llvm.__emitBr(condLbl, out);
            llvm.__emitLabel(condLbl, out);
            std::string currIdxReg = llvm.__emitLoad(idxType, idxReg, out);

            std::string condReg = llvm.__emitBinaryOp("<", idxType, false, currIdxReg, limitReg, out);
            llvm.__emitCondBr(condReg, bodyLbl, endLbl, out);

            llvm.__emitLabel(bodyLbl, out);

            std::string gepReg = llvm.__emitGEP(lt, arrPtr, {idxType + " " + currIdxReg}, true, out);
            std::string valReg = llvm.__emitLoad(lt, gepReg, out);
            llvm.__emitStore(lt, valReg, varReg, out);

            breakTracker.push_back(endLbl);
            continueTracker.push_back(updateLbl);

            genBlock(forIn->body.get(), out);

            continueTracker.pop_back();
            breakTracker.pop_back();
            llvm.__emitBr(updateLbl, out);

            llvm.__emitLabel(updateLbl, out);
            std::string upLoadIdx = llvm.__emitLoad(idxType, idxReg, out);

            std::string addIdx = llvm.__emitBinaryOp("+", idxType, false, upLoadIdx, "1", out);
            llvm.__emitStore(idxType, addIdx, idxReg, out);

            llvm.__emitBr(condLbl, out);
            llvm.__emitLabel(endLbl, out);
        }
    }

    symTable.popScope();
    int roots = popGCScope();
    emitGCPops(roots, out);
}