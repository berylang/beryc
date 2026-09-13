#include "typechecker.h"

#include "../parser/ast/expressions.h"
#include "../parser/ast/functions.h"
#include "../parser/ast/classes.h"
#include <iostream>
#include <unordered_set>



std::string TypeChecker::checkCallExpr(ASTNode* node) {
    auto* call = static_cast<CallExprNode*>(node);

    static const std::unordered_set<std::string> builtinIO = {"print", "println","inputInt", "inputBigInt", "inputFloat","inputDouble", "inputBool", "inputChar", "inputString"  };

    static const std::unordered_map<std::string, std::string> inputTypes = {
        {"inputInt", "int"}, 
        {"inputBigInt", "bigint"}, {"inputFloat", "float"},
        {"inputDouble", "double"}, {"inputBool", "bool"},   
        {"inputChar", "char"}, {"inputString", "string"}
    };

    if (builtinIO.count(call->callee)) {
        if (call->callee == "print" && call->arguments.size() != 1) {
            diag.report("ERROR366", call->line, 1, "", "");
            
        }
        if (call->callee == "println" && call->arguments.size() > 1) {
            diag.report("ERROR367", call->line, 1, "", "");
            
        }
        if (call->callee != "print" && call->callee != "println" && call->arguments.size() != 1) {
            diag.report("ERROR368", call->line, 1, "", call->callee);
            
        }
        for (auto& arg : call->arguments) {
            analyzeExpression(arg.get());
        }

        auto it = inputTypes.find(call->callee);
        call->resolvedType = (it != inputTypes.end()) ? it->second : "void";
        return call->resolvedType;
    }
    if (call->callee == "super" || call->callee.rfind("super.", 0) == 0) {
        return checkSuperCall(node);
    }
    size_t dot = call->callee.find('.');
    if (dot != std::string::npos) {
        std::vector<std::string> parts = splitDots(call->callee);
        std::string method = parts.back();
        std::vector<std::string> headParts(parts.begin(), parts.end() - 1);
        std::string objType = resolveChainType(headParts, call->line);
        if (objType == "unknown") { call->resolvedType = "unknown"; return call->resolvedType; }

        if (objType == "string") {
            if (method == "substr" || method == "copy") { 
                call->resolvedType = "string"; 
                return call->resolvedType; 
            }
            if (method == "len"){ 
                call->resolvedType = "int";    
                return call->resolvedType; 
            }
        }

        if (objType.size() > 6 && objType.substr(0, 6) == "array<") {
            std::string elemType = objType.substr(6, objType.size() - 7);
            if (method == "push" || method == "pop" || method == "insert" || method == "remove") {
                call->resolvedType = "void"; 
                return call->resolvedType;
            }
            if (method == "len") { 
                call->resolvedType = "int";
                return call->resolvedType; 
                }
            if (method == "get") { 
                call->resolvedType = elemType; 
                return call->resolvedType; 
            }
        }

        auto classIt = classes.find(objType);
        if (classIt != classes.end()) {
            ClassDefNode* cls = classIt->second;
            std::vector<FunctionDefNode*> candidateMethod = getInheritedMethods(cls, method);
            if(candidateMethod.empty()){
                diag.report("ERROR369", call->line, 1, "", objType + "' has no method '" + method);
                
                call->resolvedType = "unknown";
                return call->resolvedType;
            }

            std::vector<std::string> argTypes;
            for(auto& arg : call->arguments){argTypes.push_back(analyzeExpression(arg.get()));}
            FunctionDefNode* methodDef = resolveMethodOverload(candidateMethod, argTypes, method, call->line);
            if (!methodDef) {
                call->resolvedType = "unknown";
                return call->resolvedType;
            }
            if (!checkMemberAccess(methodDef->access, objType, method, "method", call->line)) {
                call->resolvedType = "unknown";
                return call->resolvedType;
            }
            call->resolvedParamTypes.clear();
            for(auto& p : methodDef->parameters){call->resolvedParamTypes.push_back(p.first);}
            
            
            call->resolvedType = methodDef->returnType.empty() ? "void" : methodDef->returnType;
            return call->resolvedType;
        }

        diag.report("ERROR370", call->line, 1, "", method + "' on type '" + objType);
        
        call->resolvedType = "unknown";
        return call->resolvedType;
    }

    if (!currentClass.empty()) {
        auto selfClassIt =classes.find(currentClass);
        if (selfClassIt != classes.end()) {
            std::vector<FunctionDefNode*> candidateFunction = getInheritedMethods(selfClassIt->second, call->callee);
            if(!candidateFunction.empty()){
                std::vector<std::string> argumentTypesName;
                for(auto& argsss : call->arguments){
                    argumentTypesName.push_back(analyzeExpression(argsss.get()));
                }
                FunctionDefNode* f = resolveMethodOverload(candidateFunction, argumentTypesName, call->callee, call->line);
                if(!f){
                    call->resolvedType = "unknown";
                    return call->resolvedType;
                }
                 call->resolvedParamTypes.clear();
                for(auto& p : f->parameters){call->resolvedParamTypes.push_back(p.first);}
                call->resolvedType = f->returnType.empty() ? "void" : f->returnType;
                return call->resolvedType;
            }

            
        }

    }
    auto functionIT = functions.find(call->callee);
    if (functionIT == functions.end()) {
        diag.report("ERROR371", call->line, 1, "", call->callee);
        
        call->resolvedType = "unknown";
        return call->resolvedType;
    }
    std::vector<std::string> argTypes;
    for(auto& hello : call->arguments){
        argTypes.push_back(analyzeExpression(hello.get()));
    }
    const FunctionSignature* signature = resolveFunctionOverload(functionIT->second, argTypes, call->callee, call->line);
    if(!signature){
        call->resolvedType = "unknown";
        return call->resolvedType;
    }
    call->resolvedParamTypes = signature->parameterTypes;
    call->resolvedType = signature->returnType;
    return call->resolvedType;
}


FunctionDefNode* TypeChecker::resolveMethodOverload(const std::vector<FunctionDefNode*>& candidate, const std::vector<std::string>& argTypes, const std::string& label, int line){
    for(auto* a : candidate){
        std::vector<std::string> parameterTypeList;
        for(auto& p : a->parameters){
            parameterTypeList.push_back(p.first);
        }
        if(isParameterTypeExactlyMatching(parameterTypeList, argTypes)){
            return a;
        }
    }
    FunctionDefNode* c = nullptr;
    int matchCount = 0;
    for(auto* f : candidate){
        if(f->parameters.size()!=argTypes.size()){continue;}
        bool thirtyfour = true;
        for(size_t i = 0; i<argTypes.size();i++){
            if(argTypes[i] == "unknown"){continue;}
            if(!isParameterTypePromotable(argTypes[i],f->parameters[i].first)){thirtyfour = false; break;}
        }
        if(thirtyfour){
            matchCount++;
            c = f;
        }
    }
    if(matchCount==1){return c;}
    if(matchCount>1){
        diag.report("ERROR392", line, 1, "", label + "' with " + std::to_string(argTypes.size()));
        
        return nullptr;
    }
    bool isAnyMethodMatchingToThisWhatToThisToThisCallCall = false;
    for(auto* f : candidate){
        if(f->parameters.size() == argTypes.size()){
            isAnyMethodMatchingToThisWhatToThisToThisCallCall = true;
        }
    }
    if(!isAnyMethodMatchingToThisWhatToThisToThisCallCall){
        diag.report("ERROR393", line, 1, "", label + "' accepts the " + std::to_string(argTypes.size()));
    }
    else{diag.report("ERROR394", line, 1, "", label);}
    
    return nullptr;
}


const FunctionSignature* TypeChecker::resolveFunctionOverload(const std::vector<FunctionSignature>& candidate, const std::vector<std::string>& argTypes, const std::string& label, int line){
    for(auto& a : candidate){
        if(isParameterTypeExactlyMatching(a.parameterTypes, argTypes)){
            return &a;
        }
    }
    const FunctionSignature* c = nullptr;
    int matchCount = 0;
    for(auto& f : candidate){
        if(f.parameterTypes.size()!=argTypes.size()){continue;}
        bool thirtyfour = true;
        for(size_t i = 0; i<argTypes.size();i++){
            if(argTypes[i] == "unknown"){continue;}
            if(!isParameterTypePromotable(argTypes[i],f.parameterTypes[i])){thirtyfour = false; break;}
        }
        if(thirtyfour){
            matchCount++;
            c = &f;
        }
    }
    if(matchCount==1){return c;}
    if(matchCount>1){
         diag.report("ERROR395", line, 1, "", label + "' with " + std::to_string(argTypes.size()));
         
         return nullptr;
    }
    bool isAnyMethodMatchingToThisWhatToThisToThisCallCall = false;
    for(auto& f : candidate){
        if(f.parameterTypes.size() == argTypes.size()){
            isAnyMethodMatchingToThisWhatToThisToThisCallCall = true;
        }
    }
    if(!isAnyMethodMatchingToThisWhatToThisToThisCallCall){
        diag.report("ERROR396", line, 1, "", label + "' accepts the " + std::to_string(argTypes.size()));
    }
    else{diag.report("ERROR397", line, 1, "", label);}
    
    return nullptr;
}

bool TypeChecker::isParameterTypePromotable(const std::string& from, const std::string& to){
    if(from==to){return true;}
    if(to=="float" && from=="int"){return true;}
    if(to=="double" && from=="int"){return true;}
    if(to=="double" && from=="float"){return true;}
    if(to=="bigint" && from=="int"){return true;}
    return false;
    

}

bool TypeChecker::isParameterTypeExactlyMatching(const std::vector<std::string>& a, const std::vector<std::string>& b){
    if(a.size()!=b.size()){
        return false;
    }
    for(size_t i = 0;i < a.size(); i++){
        if(a[i]!=b[i]){
            return false;
        }
    }
    return true;
}
