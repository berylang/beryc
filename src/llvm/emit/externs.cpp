

#include "../LLVMHelper.h"

void LLVMHelper::__declareExtern(const std::string& declareText, const std::string& key) {
    if (declaredExterns.count(key)) return;
    declaredExterns.insert(key);
    __BRE_declares << declareText << "\n";
}
