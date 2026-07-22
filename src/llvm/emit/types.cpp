#include "../LLVMHelper.h"

std::string LLVMHelper::__llvmType(const std::string& beryType) {
    if (beryType == "int") return "i32";

    if (beryType == "bigint") return "i64";

    if (beryType == "bool") return "i1";


    if (beryType == "float") return "float";
    if (beryType == "double") return "double";

    if (beryType == "char") return "i8";
    if(beryType  == "string") return "i8*";

    if(beryType.size() > 6  && beryType.substr(0, 6) == "array<")
        return "i8*";
        
    // for classes it needs to be classname
    // or pointer, so it's blank for now.
    return "";
}

int LLVMHelper::__alignOf(const std::string& llvmType) {
    if (llvmType == "i1" || llvmType == "i8") {
        return 1; 
    }
    if (llvmType == "i32" || llvmType == "float") {
        return 4;
    }
    if (llvmType == "i64" || llvmType == "double" ) {
        return 8;
    }
    if (!llvmType.empty() && llvmType.back() == '*') {
        return 8;
    }

    
    return 4;
}


std::string LLVMHelper::__pointerType(const std::string& baseType) {
    return baseType + "*";
}

std::string LLVMHelper::__arrayBeryType(const std::string& elementType) {
    return "array<" + elementType + ">";
}

std::string LLVMHelper::__arrayTypeSignature(const std::string& elementBeryType, int dimensions) {
    std::string sig = elementBeryType;
    for (int i = 0; i < dimensions; ++i) sig += "[]";
    return sig;
}

std::string LLVMHelper::__nestedArrayType(const std::string& elementLLVMType, const std::vector<int>& dimensions) {
    std::string t = elementLLVMType;
    for (auto it = dimensions.rbegin(); it != dimensions.rend(); ++it) {
        t = "[" + std::to_string(*it) + " x " + t + "]";
    }

    return t;
}