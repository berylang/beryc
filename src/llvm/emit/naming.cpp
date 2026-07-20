
#include "../LLVMHelper.h"

std::string LLVMHelper::__uniqueReg() {
    return "%bery." +std::to_string(__uniqueId());
}


std::string LLVMHelper::__uinqueLabel(const std::string& prefix) {
    return prefix + "_" +std::to_string(__uniqueId());
}

int LLVMHelper::__uniqueId() {
    return ++__reg_counter;
}