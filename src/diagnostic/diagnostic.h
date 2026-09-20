#pragma once
#include <string>
#include <vector>

enum class Severity { ERROR, WARNING };

struct Diagnostic {
    std::string code;
    Severity severity;
    int line;
    int column;
    std::string lexeme;
    std::vector<std::string> contexts;
    std::string hint;
};