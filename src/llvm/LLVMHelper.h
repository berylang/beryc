#pragma once


#include <string>
#include <fstream>

class LLVMHelper {
public:
    LLVMHelper();

    std::string llvmType(const std::string& beryType);
    void declareFunc(std::string type, std::string funcDef);

};