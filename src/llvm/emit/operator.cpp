

#include "../LLVMHelper.h"

#include <unordered_map>

std::string LLVMHelper::__emitBinaryOp(const std::string& op, const std::string& llvmType, bool isFloat, 
    const std::string& leftRegister, const std::string& rightRegister, std::ostream& outputStream) {
    
    // hasmhamp for faster lookup for binary operators
    // mapped to the LLVM isntructions categorising
    // it in either floating point operation or integer.
    static const std::unordered_map<std::string, std::pair<std::string, std::string>> operatorMap = {

        // binary operators => arithemetic
        {"+", {"fadd",  "add"}}, {"-", {"fsub","sub"}},
        {"*",  {"fmul", "mul"}}, {"/",  {"fdiv", "sdiv"}},

        // binary operators => only works on integers
            // modulo
            {"%",  {"", "srem"}},

            // shifting
            {"<<", {"", "shl"}},
            {">>", {"", "ashr"}},
        
            // bitwise 
            {"&",  {"", "and"}},
            {"|",  {"", "or"}},
            {"^",  {"", "xor"}},

        // binary ooperators => comparison
        {"==", {"fcmp oeq", "icmp eq"}},
        {"!=", {"fcmp one", "icmp ne"}},
        {">",  {"fcmp ogt", "icmp sgt"}},
        {"<",  {"fcmp olt", "icmp slt"}},
        {">=", {"fcmp oge", "icmp sge"}},
        {"<=", {"fcmp ole", "icmp sle"}},
    };

    auto it = operatorMap.find(op);
    if(it ==operatorMap.end())  return "0";

    
    // Instruction choosing based on flag isFloat; 
    // from the operatorMap declared as Hashmap
    // storing the pair of <float instruction, integer instruction>
    std::string instruction = isFloat? it->second.first : it->second.second;

    if (instruction.empty()) return "0";


    std::string resultReg = __uniqueReg();
    outputStream << "   " << resultReg << " = " << instruction << " " << llvmType << " " << leftRegister << ", " << rightRegister << "\n";
    return resultReg;
}

std::string LLVMHelper::__emitPow(const std::string& llvmType, bool isFloat, const std::string& lReg, const std::string& rReg, std::ostream& out) {
    __declareExtern("declare double @llvm.pow.f64(double, double)", "llvm.pow.f64");
    if (isFloat) {
        if (llvmType == "float") {
            std::string left = __emitConvert("fpext", "float", lReg, "double", out);
            std::string right = __emitConvert("fpext", "float", rReg, "double", out);
            std::string power = __emitCall("double", "llvm.pow.f64", {{"double", left}, {"double", right}}, out);
            return __emitConvert("fptrunc", "double", power, "float", out);
        }
        return __emitCall("double", "llvm.pow.f64", {{"double", lReg}, {"double", rReg}}, out);
    }
    std::string left = __emitConvert("sitofp", llvmType, lReg, "double", out);
    std::string right = __emitConvert("sitofp", llvmType, rReg, "double", out);
    std::string power = __emitCall("double", "llvm.pow.f64", {{"double", left}, {"double", right}}, out);
    return __emitConvert("fptosi", "double", power, llvmType, out);
}