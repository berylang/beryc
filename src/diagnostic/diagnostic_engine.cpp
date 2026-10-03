#include "diagnostic_engine.h"
#include "diagnostic_codes.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include <cstdlib>
#ifdef _WIN32
  #include <io.h>
  #define BERY_ISATTY(fd) _isatty(fd)
  #define BERY_FILENO(f)  _fileno(f)
#else
  #include <unistd.h>
  #define BERY_ISATTY(fd) isatty(fd)
  #define BERY_FILENO(f)  fileno(f)
#endif
#include <cstdio>

static bool useColor(FILE* stream) {
    if (std::getenv("NO_COLOR")) return false;
    return BERY_ISATTY(BERY_FILENO(stream)) != 0;
}

static std::string sgr(const char* code, FILE* stream = stdout) {
    return useColor(stream) ? std::string("\033[") + code + "m" : std::string();
}

DiagnosticEngine::DiagnosticEngine(const std::string& source, const std::string& filename)
    : filename(filename) {
    splitSource(source);
}
void DiagnosticEngine::splitSource(const std::string& source) {
    std::stringstream ss(source);
    std::string line;
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        sourceLines.push_back(line);
    }
}
void DiagnosticEngine::fatal(const std::string& message, const std::string& hint) {
    const std::string RESET = sgr("0", stderr);
    const std::string BOLD  = sgr("1", stderr);
    const std::string DIM   = sgr("2", stderr);

    std::cerr << sgr("1;31", stderr) << "error" << RESET << BOLD << ": " << message << RESET << "\n";
    if (!hint.empty())
        std::cerr << DIM << "   = hint: " << RESET << hint << "\n";
}

static std::string formatTemplate(const std::string& tmpl, const std::vector<std::string>& contexts) {
    std::string out = tmpl;
    size_t pos = 0;
    for (const auto& ctx : contexts) {
        pos = out.find("{}", pos);
        if (pos == std::string::npos) break;
        out.replace(pos, 2, ctx);
        pos += ctx.size();
    }
    return out;
}

void DiagnosticEngine::report(const std::string& code, int line, int column, int length, const std::string& context) {
    report(code, line, column, length, context.empty() ? std::vector<std::string>{} : std::vector<std::string>{context});
}


void DiagnosticEngine::report(const std::string& code, int line, int column, int length, const std::vector<std::string>& contexts) {
    auto it = DiagnosticRegistry.find(code);
    if (it == DiagnosticRegistry.end()) {
        std::cerr << "[internal] Unknown diagnostic code: " << code << "\n";
        return;
    }

    Diagnostic d;
    d.code = code;
    d.severity = it->second.severity;
    d.line = line;
    d.column = column;
    d.length = length; 
    d.contexts = contexts;

    diagnostics.push_back(d);
    if (d.severity == Severity::ERROR) errorCount++;
    else warningCount++;
}

bool DiagnosticEngine::hasErrors() const { return errorCount > 0; }
bool DiagnosticEngine::hasWarnings() const { return warningCount > 0;}

void DiagnosticEngine::printOne(const Diagnostic& d) {
    const auto& info = DiagnosticRegistry.at(d.code);
    const bool isErr = (d.severity == Severity::ERROR);

    const std::string RESET = sgr("0");
    const std::string BOLD  = sgr("1");
    const std::string DIM   = sgr("2");
    const std::string SEV   = isErr ? sgr("1;31") : sgr("1;33");   // bold red / bold yellow

    std::string message = formatTemplate(info.messageTemplate, d.contexts);

    std::cout << SEV << (isErr ? "error" : "warning") << "[" << d.code << "]" << RESET
              << BOLD << ": " << message << RESET << "\n";
    std::cout << DIM << "  --> " << RESET << filename << ":" << d.line << ":" << d.column << "\n";

    if (d.line >= 1 && !sourceLines.empty()) {
        int idx = std::min(d.line - 1, (int)sourceLines.size() - 1);
        const std::string& src = sourceLines[idx];
        int w = (int)std::to_string(idx + 1).size();
        std::string gutter(w, ' ');

        int lineLen  = (int)src.size();
        int col0     = (d.line - 1 > idx) ? lineLen : std::max(0, d.column - 1);
        int room     = std::max(1, lineLen - col0);
        int caretLen = std::clamp(d.length, 1, room);

        std::string pad;                                  // keep tabs as tabs so the caret lines up
        for (int k = 0; k < col0; ++k)
            pad += (k < lineLen && src[k] == '\t') ? '\t' : ' ';

        std::cout << DIM << " " << gutter << " |" << RESET << "\n";
        std::cout << DIM << " " << std::setfill(' ') << std::setw(w) << (idx + 1) << " | " << RESET << src << "\n";
        std::cout << DIM << " " << gutter << " | " << RESET << pad << SEV << std::string(caretLen, '^') << RESET << "\n";
    }

    if (!info.hint.empty()) {
        std::cout << DIM << "   = hint: " << RESET << formatTemplate(info.hint, d.contexts) << "\n";
    }
    std::cout << "\n";
}

void DiagnosticEngine::printSummary(const std::string& stage) {
    auto plural = [](int n, const std::string& word) {
        return std::to_string(n) + " " + word + (n == 1 ? "" : "s");
    };
    const std::string RESET = sgr("0");
    const std::string BOLD  = sgr("1");

    if (errorCount > 0) {
        std::string what = stage.empty() ? "error" : stage + " error";
        std::cout << sgr("1;31") << "error" << RESET << BOLD
                  << ": aborting due to " << plural(errorCount, what);
        if (warningCount > 0) std::cout << "; " << plural(warningCount, "warning") << " emitted";
        std::cout << RESET << "\n";
    } else if (warningCount > 0) {
        std::cout << sgr("1;33") << "warning" << RESET << BOLD
                  << ": " << plural(warningCount, "warning") << " emitted" << RESET << "\n";
    }
}

void DiagnosticEngine::printAll(const std::string& stage) {
    if (diagnostics.empty()) return;
    std::stable_sort(diagnostics.begin(), diagnostics.end(),
        [](const Diagnostic& a, const Diagnostic& b) {
            if (a.line != b.line) return a.line < b.line;
            return a.column < b.column;
        });
    for (const auto& d : diagnostics) printOne(d);
    printSummary(stage);
}