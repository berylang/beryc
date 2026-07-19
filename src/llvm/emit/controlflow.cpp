#include "../LLVMHelper.h"

void LLVMHelper::__emitLabel(const std::string& name, std::ostream& out) {out << "\n"<< name << ":\n";}

void LLVMHelper::__emitBr(const std::string& label, std::ostream& out) {
    out << "    br label %" << label << "\n";
}

void LLVMHelper::__emitCondBr(const std::string& cond, const std::string& trueLabel, 
    const std::string& falseLabel,std::ostream& out) {
    out << "    br i1 " << cond << ", label %" << trueLabel << ", label %"<< falseLabel << "\n";
}