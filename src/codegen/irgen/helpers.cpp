#include "../codegen.h"
#include <string>
#include <iomanip>
#include "../../parser/ast/literals.h"
#include "../../parser/ast/expressions.h"
#include "../../parser/ast/vardecl.h"


std::string CodeGen::llvmType(const std::string& t) {
    std::string base = llvm.__llvmType(t);
    if (!base.empty()) return base;
    if (classLayouts.count(t)) return classLayouts.at(t).llvmStructType + "*";
    return "i32";
}


std::string CodeGen::extractConstant(ASTNode* node) {
    if (!node) return "0";
    if (node->type == NodeType::INT_LIT) {
        return std::to_string(static_cast<IntLitNode*>(node)->value);
    } else if (node->type == NodeType::DECIMAL_LIT) {
        return llvm.__formatDoubleConstant(static_cast<DecimalLitNode*>(node)->value);
    }
    else if (node->type == NodeType::BOOL_LIT) {
        return static_cast<BoolLitNode*>(node)->value ? "1" : "0";
    } 
    else if (node->type == NodeType::CHAR_LIT) {
        return std::to_string(static_cast<CharLitNode*>(node)->value);
    }else if (node->type == NodeType::STRING_LIT) {
        return llvm.__emitGlobalStringConstant(static_cast<StringLitNode*>(node)->value);
    }
    else if (node->type == NodeType::NULL_LIT) {
        return "null";
    }
    else if (node->type == NodeType::UNARY_EXPR) {
        auto* unary = static_cast<UnaryExprNode*>(node);
        if (unary->optr == "-") {
            return "-" + extractConstant(unary->operand.get());
        }
    }
    return "0";
}


void CodeGen::genStatement(ASTNode* stmt, std::ostream& out) {
    if (!stmt) return;
    
    if (stmt->type == NodeType::VAR_DECL) genVarDecl(stmt, out);
    else if (stmt->type == NodeType::ARRAY_DECL) genArrayDecl(stmt, out);
    else if (stmt->type == NodeType::ASSIGNMENT_EXPR || stmt->type == NodeType::UNARY_EXPR ||stmt->type == NodeType::CALL_EXPR) genExpression(stmt, "any", out);
    else if (stmt->type == NodeType::IF_STMT) genIfStmt(stmt, out);
    else if (stmt->type == NodeType::WHILE_STMT) genWhileStmt(stmt, out);
    else if (stmt->type == NodeType::DOWHILE_STMT) genDoWhileStmt(stmt, out);
    else if (stmt->type == NodeType::SWITCH_STMT) genSwitchStmt(stmt, out);
    else if (stmt->type == NodeType::BREAK_STMT) genBreakStmt(stmt, out);
    else if (stmt->type == NodeType::BLOCK) genBlock(stmt, out);
    else if (stmt->type == NodeType::PASS_STMT) {}
    else if (stmt->type == NodeType::CONTINUE_STMT) genContinueStmt(stmt, out);
    else if (stmt->type == NodeType::RETURN_STMT) genReturnStmt(stmt, out);
    else if (stmt->type == NodeType::ENUM_DECL) {
        auto* enumDecl = static_cast<EnumDeclNode*>(stmt);
        int currentValue = 0;
        
        for (const auto& val : enumDecl->values) {
            std::string mangledName = enumDecl->name + "." + val;
            std::string lt = "i32";
            std::string safeRegName = enumDecl->name + "_" + val; 
            std::string memReg = "%" + safeRegName + "_" + std::to_string(llvm.__uniqueId());
            
            Symbol sym;
            sym.symbolType = SymbolType::VARIABLE;
            sym.type = "int";
            sym.isConst = true;
            sym.isInitialized = true;
            sym.line = enumDecl->line;
            sym.llvmRegister = memReg;
            sym.llvmAllocType = lt;
            symTable.add(mangledName, sym);
            out << "    " << memReg << " = alloca " << lt << "\n";
            llvm.__emitStore(lt, std::to_string(currentValue++), memReg, out);
        }
    }
    else if (stmt->type == NodeType::FOR_STMT) genForStmt(stmt, out);
    else if (stmt->type == NodeType::FOR_IN_STMT) genForInStmt(stmt, out);
}