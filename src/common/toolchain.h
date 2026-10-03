#pragma once

/*
    Bery Compiler Toolchain.

    Finds the compiler tools and builds the shell commands to compile and link a Bery program.
    Bundled toolchain (<exeDir>/../toolchain) is preferred over the system one.
*/

#include "platform.h"
#include <cstdlib>
#include <filesystem>
#include <string>

// Folder name of the toolchain shipped next to bin/ in release archives.
inline const std::string kBundledToolchainDir = "toolchain";

struct BeryToolChain {
    // clang driver: compiles the .ll IR file into an object file.
    std::string clang;

    // clang++ driver: links object + BRE into the final binary.
    std::string clangxx;

    // Sysroot of the bundled toolchain. Empty when not needed.
    std::string sysroot;

    // .o / .obj
    std::string objectExt;

    // "" or ".exe"
    std::string binaryExt;

    // true when the toolchain shipped with Bery is used.
    bool bundled;

    // false if detection failed; the caller must abort compilation.
    bool valid;
};

// Checks if a command exists on the system PATH ('where' on Windows, 'which' on Unix).
inline bool commandExists(const std::string& cmd) {
#ifdef BERY_WINDOWS
    std::string check = "where " + cmd + " >nul 2>&1";
#else
    std::string check = "which " + cmd + " >/dev/null 2>&1";
#endif
    return system(check.c_str()) == 0;
}

inline std::string shellQuote(const std::string& s) {
    return "\"" + s + "\"";
}

// cmd.exe strips the first and last quote of a command that starts with a quote,
// which breaks quoted absolute paths. Wrapping the whole command in one more pair fixes it.
inline int runShell(const std::string& cmd) {
#ifdef BERY_WINDOWS
    return system(("\"" + cmd + "\"").c_str());
#else
    return system(cmd.c_str());
#endif
}

inline std::string exeSuffix() {
#ifdef BERY_WINDOWS
    return ".exe";
#else
    return "";
#endif
}

inline std::string bundledRoot(const std::string& exeDir) {
    return exeDir + BERY_PATH_SEP + ".." + BERY_PATH_SEP + kBundledToolchainDir;
}

inline BeryToolChain detectToolchain(const std::string& exeDir) {
    BeryToolChain tc;
    tc.bundled = false;
    tc.valid = false;
#ifdef BERY_WINDOWS
    tc.objectExt = ".obj";
    tc.binaryExt = ".exe";
#else
    tc.objectExt = ".o";
    tc.binaryExt = "";
#endif

    std::string root = bundledRoot(exeDir);
    std::string bin = root + BERY_PATH_SEP + "bin" + BERY_PATH_SEP;
    std::string bundledClang = bin + "clang" + exeSuffix();
    std::string bundledClangxx = bin + "clang++" + exeSuffix();

    if (std::filesystem::exists(bundledClang) && std::filesystem::exists(bundledClangxx)) {
        tc.clang = bundledClang;
        tc.clangxx = bundledClangxx;
        tc.bundled = true;
        std::string sysroot = root + BERY_PATH_SEP + "sysroot";
        if (std::filesystem::is_directory(sysroot)) tc.sysroot = sysroot;
        tc.valid = true;
        return tc;
    }

    // System fallback: only for contributors building from source.
    if (commandExists("clang") && commandExists("clang++")) {
        tc.clang = "clang";
        tc.clangxx = "clang++";
        tc.valid = true;
    }
    return tc;
}

// Flags

inline std::string targetFlag(const BeryToolChain& tc) {
    if (!tc.bundled) return "";
#ifdef BERY_WINDOWS
    return " --target=x86_64-w64-mingw32";
#elif defined(BERY_LINUX)
    return " --target=x86_64-linux-musl";
#else
    return "";
#endif
}

inline std::string sysrootFlag(const BeryToolChain& tc) {
    return tc.sysroot.empty() ? "" : " --sysroot=" + shellQuote(tc.sysroot);
}

inline std::string picFlag() {
#ifdef BERY_WINDOWS
    return "";
#else
    return " -fPIC";
#endif
}

// Bundled Linux/Windows toolchains produce static binaries, so user programs run anywhere.
inline std::string bundledLinkFlags(const BeryToolChain& tc) {
    if (!tc.bundled) return "";
#ifdef BERY_MACOS
    return "";
#else
    return " -static -fuse-ld=lld";
#endif
}

// Commands

// -O0 keeps the same behavior as before (no IR optimization). Try -O2 after all examples pass.
inline std::string buildCompileCmd(const BeryToolChain& tc, const std::string& irFile, const std::string& objFile) {
    return shellQuote(tc.clang) + " -c -O0 -Wno-override-module" + targetFlag(tc) + sysrootFlag(tc) + picFlag()
        + " " + shellQuote(irFile) + " -o " + shellQuote(objFile);
}

inline std::string buildLinkCmd(const BeryToolChain& tc, const std::string& objFile, const std::string& breLib, const std::string& outBinary) {
    return shellQuote(tc.clangxx) + targetFlag(tc) + sysrootFlag(tc) + bundledLinkFlags(tc)
        + " " + shellQuote(objFile) + " " + shellQuote(breLib) + " -o " + shellQuote(outBinary);
}