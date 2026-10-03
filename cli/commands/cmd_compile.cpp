#include "cmd_compile.h"
#include "../../src/common/toolchain.h"
#include "../../src/common/platform.h"
#include "../../src/lexer/lexer.h"
#include "../../src/parser/parser.h"
#include "../../src/sema/sema.h"
#include "../../src/codegen/codegen.h"
#include "../../src/importer/importer.h"
#include "../../src/diagnostic/diagnostic_engine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <cstdint>
#include <limits.h>
#ifdef BERY_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOGDI
#include <windows.h>
#elif defined(BERY_MACOS)
#include <mach-o/dyld.h>
#else
#include <unistd.h>
#endif

static std::string stemOf(const std::string& path) {
    size_t slash = path.find_last_of("/\\");
    std::string filename = (slash == std::string::npos) ? path : path.substr(slash + 1);
    size_t dot = filename.find_last_of('.');
    return (dot == std::string::npos) ? filename : filename.substr(0, dot);
}

static std::string dirOf(const std::string& path) {
    size_t slash = path.find_last_of("/\\");
    if (slash == std::string::npos) return ".";
    return path.substr(0, slash);
}


static std::string executablePath(const char* argv0) {
#ifdef BERY_WINDOWS
    char buf[MAX_PATH];
    DWORD n = GetModuleFileNameA(nullptr, buf, MAX_PATH);
    if (n > 0 && n < MAX_PATH) return std::string(buf, n);
#elif defined(BERY_MACOS)
    char buf[PATH_MAX];
    uint32_t size = sizeof(buf);
    if (_NSGetExecutablePath(buf, &size) == 0) return std::filesystem::weakly_canonical(buf).string();
#else
    char buf[PATH_MAX];
    ssize_t n = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (n > 0) return std::string(buf, n);
#endif
    return argv0;
}

std::string getExeDir(const char* argv) {
    std::string dir = std::filesystem::path(executablePath(argv)).parent_path().string();
    return dir.empty() ? "." : dir;
}

static std::string findBRELib(const std::string& exeDir) {
    const char* names[] = { "libbre.a", "bre.lib", nullptr };
    const std::string dirs[] = {
        exeDir,
        exeDir + "/../build",
        "build",
        ".",
        ""
    };
    for (int d = 0; !dirs[d].empty(); ++d) {
        for (int n = 0; names[n]; ++n) {
            std::string candidate = dirs[d] + BERY_PATH_SEP + names[n];
            std::ifstream f(candidate);
            if (f.good()) return candidate;
        }
    }
    return "";
}

static int runFrontend(const std::string& sourcePath, const std::string& irPath, const std::string& exeDir) {
    std::ifstream file(sourcePath);
    if (!file.is_open()) {
        std::cerr <<"Bery: Error: cannot open file '" << sourcePath <<"'\n";
        return 3;
    }

    std::string basePath = dirOf(sourcePath) + BERY_PATH_SEP;
    std::string stdlibPath = exeDir + BERY_PATH_SEP + ".." + BERY_PATH_SEP + "src" + BERY_PATH_SEP + "stdlib" + BERY_PATH_SEP;
    std::stringstream buf;
    buf << file.rdbuf();
    std::string source = buf.str();
    std::string absPath = std::filesystem::absolute(sourcePath).lexically_normal().string();
    DiagnosticEngine diag(source, absPath);

    Lexer lexer(source, diag);
    auto tokens = lexer.tokanize();

    Parser parser(tokens, diag);
    auto ast = parser.parse();

    if (diag.hasErrors() || parser.hasErrors()) {
        diag.printAll();
        std::cerr << "Bery: Compilation halted due to syntax errors.\n";
        return 5;
    }

    Importer importer;
    importer.resolveImports(static_cast<ProgramNode*>(ast.get()), basePath, stdlibPath, diag);

    SemanticAnalyzer sema(ast.get(), diag);
    sema.analyze();

    diag.printAll();
    if (diag.hasErrors()) {
        std::cerr <<"Bery: Compilation halted due to semantic errors.\n";
        return 6;
    }
    CodeGen codegen(ast.get(), sema.symbolTable);
    codegen.generate(irPath);
    return 0;
}

int cmdCompile(const std::string& sourcePath, std::string& outBinaryPath, const std::string& exeDir) {
    BeryToolChain tc = detectToolchain(exeDir);
    if (!tc.valid) {
        std::cerr <<"Bery: Error: no toolchain found.\n\tReinstall Bery, or install clang and clang++.\n";
        return 10;
    }

    std::string breLib = findBRELib(exeDir);
    if (breLib.empty()) {
        std::cerr <<"Bery: Error: cannot find libbre.a.\n\tRun 'cmake --build build' first.\n";
        return 11;
    }

    std::string stem = stemOf(sourcePath);
    std::string dir = dirOf(sourcePath);
    std::string irFile = dir + BERY_PATH_SEP + stem + ".ll";
    std::string objFile = dir + BERY_PATH_SEP + stem + tc.objectExt;
    outBinaryPath = dir + BERY_PATH_SEP + stem + tc.binaryExt;
    int fe = runFrontend(sourcePath, irFile, exeDir);
    if (fe != 0) return fe;

    if (runShell(buildCompileCmd(tc, irFile, objFile)) != 0) {
        std::cerr <<"Bery: Error: compiling IR failed.\n";
        return 12;
    }
    if (runShell(buildLinkCmd(tc, objFile, breLib, outBinaryPath)) != 0) {
        std::cerr <<"Bery: Error: linker failed.\n";
        return 13;
    }
    remove(irFile.c_str());
    remove(objFile.c_str());
    return 0;
}
