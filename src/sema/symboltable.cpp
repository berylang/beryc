#include "symboltable.h"
#include <stdexcept>
#include <iostream>

SymbolTable::SymbolTable() {
    scopes.push_back(std::unordered_map<std::string, Symbol>());
}

void SymbolTable::pushScope() {
    scopes.push_back(std::unordered_map<std::string, Symbol>());
}

void SymbolTable::popScope() {
    if (scopes.size() > 1) { 
        scopes.pop_back();
    }
}

void SymbolTable::add(const std::string& name, Symbol sym) {
    scopes.back()[name] = sym;
}

bool SymbolTable::existsInCurrentScope(const std::string& name) {
    return scopes.back().find(name) != scopes.back().end();
}
bool SymbolTable::exists(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    return false;
}

Symbol& SymbolTable::get(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return it->at(name); 
        }
    }
    std::cerr << "Bery:'internal compiler error'";
    std::abort(); 
}