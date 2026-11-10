
#include "../LLVMHelper.h"

std::string LLVMHelper::__uniqueReg() {
    return "%bery." +std::to_string(__uniqueId());
}

int LLVMHelper::__uniqueId() {
    return ++__reg_counter;
}
std::string LLVMHelper::__uniqueLabel(const std::string& prefix) {
    return prefix + "_" +std::to_string(__uniqueId());
}

std::string LLVMHelper::__namedReg(const std::string& prefix) {
    return "%" + __safeName(prefix) + "_" + std::to_string(__uniqueId());
}

std::string LLVMHelper::__labelWithId(const std::string& prefix, int id) {
    return prefix + "_" + std::to_string(id);
}

std::string LLVMHelper::__indexedLabel(const std::string& prefix, int index, int id) {
    return prefix + "_" + std::to_string(index) + "_" +std::to_string(id);
}

static const char* const HEX_DIGITS = "0123456789ABCDEF";

std::string LLVMHelper::__safeName(const std::string& name) {
    std::string out;
    for (unsigned char c : name) {
        if (c < 0x80) {
            out += static_cast<char>(c);
        } else {
            out += '$';
            out += HEX_DIGITS[c >> 4];
            out += HEX_DIGITS[c & 0x0F];
        }
    }
    return out;
}