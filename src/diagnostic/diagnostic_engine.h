#pragma once
#include <vector>
#include <string>
#include "diagnostic.h"

class DiagnosticEngine {
public:
    DiagnosticEngine(const std::string& source, const std::string& filename);
    static void fatal(const std::string& message, const std::string& hint = "");
    void report(const std::string& code, int line, int column, int length, const std::string& context = "");
    void report(const std::string& code, int line, int column, int length, const std::vector<std::string>& contexts);
    bool hasErrors() const;
    bool hasWarnings() const;
    void printAll(const std::string& stage = "");

private:
    std::string filename;
    std::vector<Diagnostic> diagnostics;
    std::vector<std::string> sourceLines;
    int errorCount = 0;
    int warningCount = 0;

    void splitSource(const std::string& source);
    void printOne(const Diagnostic& d);
    void printSummary(const std::string& stage);
};