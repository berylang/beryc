#include "../codegen.h"
#include <stack>

void CodeGen::emitGCPush(const std::string& allocaReg, const std::string& lt, std::ostream& out) {
    llvm.__emitGCPushCall(allocaReg, lt, out);
    gcRootCounter++;
    gcRootScopeStack.top()++;
}

void CodeGen::emitGCPops(int count, std::ostream& out) {
    llvm.__emitGCPopCalls(count, out);
}

void CodeGen::pushGCScope() {
    gcRootScopeStack.push(0);
}

int CodeGen::popGCScope() {
    if (gcRootScopeStack.empty()) return 0;
    int count = gcRootScopeStack.top();
    gcRootScopeStack.pop();
    return count;
}