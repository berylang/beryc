#include "codegen.h"
#include "../parser/ast/programnode.h"
#include "../parser/ast/vardecl.h"
#include "../parser/ast/literals.h"
#include "../parser/ast/arraydeclare.h"
#include <cstring>
#include <unordered_map>
#include <sstream>
#include <cstdint>

CodeGen::CodeGen(ASTNode* root)
   : root(root), regCounter(0) {}

void CodeGen::generate(const std::string& outputPath) {
   auto* program = static_cast<ProgramNode*>(root);
   std::ostringstream body;
   body << "define i32 @main() {\n";
   body << "entry:\n";

      if (program->runBlock) {
       for (auto& node : program->runBlock->statements) {
           if (node->type == NodeType::VAR_DECL)
               genVarDecl(node.get(), body);
           else if (node->type == NodeType::ARRAY_DECL)
               genArrayDecl(node.get(), body);
       }
   }
   body << "    ret i32 0\n";
   body << "}\n";
   std::ofstream out(outputPath);

   for (auto& node : program->globals) {
       if (node->type == NodeType::VAR_DECL) {
           auto* decl = static_cast<VarDeclNode*>(node.get());
           std::string lt = llvmType(decl->varType);
           std::string initVal = "0";
           if (decl->value) {
               if (decl->value->type == NodeType::INT_LIT) {
                initVal = std::to_string(static_cast<IntLitNode*>(decl->value.get())->value);
               } 
               else if (decl->value->type == NodeType::DECIMAL_LIT ){
                   initVal = std::to_string(static_cast<DecimalLitNode*>(decl->value.get())->value);
               }
               else if (decl->value->type == NodeType::BOOL_LIT) {
                initVal = static_cast<BoolLitNode*>(decl->value.get())->value? "1" :"0";
               }
           }
           out << "@" << decl->name << " = global " << lt << " " << initVal << "\n";
       } 
       else if (node->type == NodeType::ARRAY_DECL) {
           auto* decl = static_cast<ArrayDeclNode*>(node.get());
           std::string lt = llvmType(decl->elementType);
           int resolvedSize = decl->size >= 0 ? decl->size : (int)decl->initializers.size();
           std::string arrType = "[" + std::to_string(resolvedSize) + " x " + lt + "]";
           
           std::string initVal;
           if (decl->initializers.empty()) {
               initVal = "zeroinitializer";
           } else {
               initVal = "[";
               for (size_t i = 0; i < decl->initializers.size(); ++i) {
                   std::string elemVal = "0";
                   auto* initNode = decl->initializers[i].get();
                   if (initNode->type == NodeType::INT_LIT) {
                       elemVal = std::to_string(static_cast<IntLitNode*>(initNode)->value);
                   } else if (initNode->type == NodeType::DECIMAL_LIT) {
                       elemVal = std::to_string(static_cast<DecimalLitNode*>(initNode)->value);
                   } else if (initNode->type == NodeType::BOOL_LIT) {
                       elemVal = static_cast<BoolLitNode*>(initNode)->value ? "1" : "0";
                   }
                   initVal += lt + " " + elemVal;
                   if (i + 1 < decl->initializers.size()) {
                       initVal += ", ";
                   }
               }
               if (decl->initializers.size() < (size_t)resolvedSize) {
                   for (size_t i = decl->initializers.size(); i < (size_t)resolvedSize; ++i) {
                       initVal += ", " + lt + " 0";
                   }
               }
               initVal += "]";
           }
           out << "@" << decl->name << " = global " << arrType << " " << initVal << "\n";
       }
   }
   out << "\n" << body.str();
}

void CodeGen::genVarDecl(ASTNode* node, std::ostream& out) {
   auto* decl = static_cast<VarDeclNode*>(node);
   std::string lt = llvmType(decl->varType);
   out << "    %" << decl->name << " = alloca " << lt << "\n";
   if (!decl->value) return;
   std::string valReg = genLiteral(decl->value.get(), decl->varType, out);
   out << "    store " << lt << " " << valReg << ", " << lt << "* %" << decl->name << "\n";
}

void CodeGen::genArrayDecl(ASTNode* node, std::ostream& out) {
   auto* decl = static_cast<ArrayDeclNode*>(node);
   std::string lt = llvmType(decl->elementType);
   int resolvedSize = decl->size >= 0 ? decl->size : (int)decl->initializers.size();
   std::string arrType = "[" + std::to_string(resolvedSize) + " x " + lt + "]";
   
   out << "    %" << decl->name << " = alloca " << arrType << "\n";
   if (decl->initializers.empty()) return;

   for (size_t i = 0; i < decl->initializers.size(); ++i) {
       std::string valReg = genLiteral(decl->initializers[i].get(), decl->elementType, out);
       std::string ptrReg = newReg();
       out << "    " << ptrReg << " = getelementptr " << arrType << ", " << arrType << "* %" << decl->name << ", i32 0, i32 " << i << "\n";
       out << "    store " << lt << " " << valReg << ", " << lt << "* " << ptrReg << "\n";
   }
}


std::string CodeGen::genLiteral(ASTNode* node, const std::string& varType, std::ostream& out) {
   std::string reg = newReg();
   if (node->type == NodeType::INT_LIT) {
       auto* lit = static_cast<IntLitNode*>(node);
       out << "    " << reg << " = add " << llvmType(varType) << " 0, " << lit->value << "\n";
       return reg;
   }

   if (node->type == NodeType::DECIMAL_LIT) {
       auto* lit = static_cast<DecimalLitNode*>(node);
       out << "    " << reg << " = fadd " << llvmType(varType) << " 0.0, " << lit->value << "\n";
       return reg;
   }

   if (node->type == NodeType::BOOL_LIT) {
      auto* lit = static_cast<BoolLitNode*>(node);
      out << "    " << reg << " = add i1 0, " << (lit->value? 1:0) << "\n";
      return reg;
   }
   return "0";
}

std::string CodeGen::llvmType(const std::string& t) {
   if (t == "int") return "i32";
   if (t == "bigint") return "i64";
   if (t == "bool") return "i1";
   if (t == "float") return "float";
   if (t == "double") return "double";
   return "i32";
}
std::string CodeGen::newReg() {
   return "%" + std::to_string(++regCounter);
}