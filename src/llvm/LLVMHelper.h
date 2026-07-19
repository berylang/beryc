#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <unordered_set>
#include <ostream>

class LLVMHelper {


private:
    int __str_counter = 0;
    int __reg_counter = 0;
    std::unordered_set<std::string> declaredExterns;

    
public:
    LLVMHelper();

    // @types
    std::string __llvmType(const std::string& beryType);
    int __alignOf(const std::string& llvmType);

    // @naming
    std::string __uniqueReg();
    std::string __uinqueLabel(const std::string& prefix);
    int __uniqueId();

    // @constants
    std::string __escapeString(const std::string& raw);
    std::string __formatDoubleConstant(double value);
    std::string __emitGlobalStringConstant(const std::string& raw);
    std::string __formatFloatHexConstant(double value);
    void __emitGlobalVar(const std::string& name, const std::string& llvmType, const std::string& initVal, std::ostream& outputStream);

    // @memory
    std::string __emitAlloca(const std::string& llvmType, std::ostream&  outputStream);
    std::string __emitLoad(const std::string& llvmType,  const std::string& pointer, std::ostream& outputStream);
    std::string __emitBitcast(const std::string& fromType, const std::string& value, const std::string& toType, std::ostream& outputStream);
    std::string __emitSext(const std::string& fromType, const std::string& value, const std::string& toType, std::ostream& outputStream);
    std::string __emitGEP(const std::string& baseType, const std::string& pointer, const std::vector<std::string>& indices,
                             bool inbounds, std::ostream& outputStream);
    std::string __emitBoxValue(const std::string& llvmType, const std::string& valueRegister,   std::ostream& outputStream);
    std::string __emitConvert(const std::string& instr, const std::string& fromType, const std::string& value, const std::string& toType, std::ostream& outputStream);

    void __emitStore(const std::string& llvmType, const std::string& value, const std::string& pointer, std::ostream& outputStream);

    // @functions
    void __emitFunctionHeader(const std::string& returnType, const std::string& name, const std::vector<std::pair<std::string, std::string>>& params, std::ostream& outputStream);
    void __emitFunctionFooter(std::ostream& outputStream);
    void __emitReturn(const std::string& llvmType, const std::string& value, std::ostream& outputStream);
    void __emitDefaultReturn(const std::string& llvmType, std::ostream& outputStream);

    // @control flow
    void __emitLabel(const std::string& name, std::ostream& outputStream);
    void __emitBr(const std::string& label, std::ostream& outputStream);
    void __emitCondBr(const std::string& condition, const std::string& trueLabel, const std::string& falseLabel, std::ostream& outputStream);

    // @calls
    std::string __emitCall(const std::string& returnType, const std::string& functionName, 
                            const std::vector<std::pair<std::string, std::string>>& arguments, std::ostream& outputStream);

    // @operators
    std::string __emitBinaryOp(const std::string& op, const std::string& llvmType, bool isFloat, 
                                const std::string& leftRegister, const std::string& rightRegister, std::ostream& outputStream);
    std::string __emitPow(const std::string& llvmType, bool isFloat, const std::string& leftRegister, const std::string& rightRegister, std::ostream& outputStream);

    // @struct / class IR
    std::string __structTypeName(const std::string& className);
    void __emitStructType(const std::string& structTypeName, const std::vector<std::string>&  fieldLLVMTypes);
    void __emitClassNameGlobal(const std::string& className);
    std::string __emitClassNameGEP(const std::string& className, int nameLen);
    std::string __emitDestructorBitcast(const std::string& structType, const std::string& className, std::ostream& outputStream);
    std::string __emitTypeRegisterCall(const std::string& className, int nameLen, long long instanceSize, 
                                        const std::string& destructorArg, std::ostream& outputStream);

    // @externs
    void __declareExtern(const std::string& declareText, const std::string& key);

    // @garbage Collectors
    void __emitGCPushCall(const std::string& reg, const std::string& llvmType, std::ostream& outputStream);
    void __emitGCPopCalls(int count, std::ostream& outputStream);

    // @outputStreamput buffers
    std::ostringstream __GLOBAL_STRINGS;
    std::ostringstream __STRUCTURE_declares;
    std::ostringstream __BRE_declares;

};