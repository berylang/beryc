#include "bery_cli.h"
#include "commands/cmd_compile.h"
#include "commands/cmd_run.h"
#include "commands/cmd_version.h"
#include <iostream>
#include <string>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <climits>
#endif

static std::string getRealExeDir() {
#ifdef _WIN32
    char buf[MAX_PATH];
    GetModuleFileNameA(NULL, buf, MAX_PATH);
    std::string path(buf);
#else
    char buf[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len == -1) return ".";
    buf[len] = '\0';
    std::string path(buf);
#endif
    size_t slash = path.find_last_of("/\\");
    return (slash == std::string::npos) ? "." : path.substr(0, slash);
}

static void prependBundledToolchainToPath(const std::string& exeDir) {
#ifdef _WIN32
    std::string bundled = exeDir + "\\..\\toolchain\\bin";
    std::string sep = ";";
#else
    std::string bundled = exeDir + "/../toolchain/bin";
    std::string sep = ":";
#endif
    const char* existing = std::getenv("PATH");
    std::string newPath = bundled + sep + (existing ? existing : "");
#ifdef _WIN32
    _putenv(("PATH=" + newPath).c_str());
#else
    setenv("PATH", newPath.c_str(), 1);
#endif
}

void printUsage() {
    std::cerr <<"Usage:\n";
    std::cerr <<"  bery compile <file.bry>   Compile to native binary\n";
    std::cerr <<"  bery run <file.bry>       Compile and run\n";
    std::cerr <<"  bery --version            Print version\n";
}

int beryMain(int argc, char* argv[]){
    if(argc<2){
        printUsage();
        return 1;
    }
    std::string command = argv[1];
    std::string ExeDir = getRealExeDir();
    prependBundledToolchainToPath(ExeDir);
    if(command=="--version"|| command=="-v"|| command == "--VERSION"){
        return cmd_version();
    }
    if(command=="compile"){
        if(argc<3){
            std::cerr<<"Bery : Error : Missing source path\n";
            return 1;
        }
        std::string out_path;
        return cmdCompile(argv[2],out_path,ExeDir);
    }
    if(command == "run"){
        if(argc<3){
            std::cerr<<"Bery : Error : Missing source path\n";
            return 1;
        }
        return cmdRun(argv[2],ExeDir);
    }
    std::cerr <<"Bery : Error : Unknown command '" << command <<"'\n";
    printUsage();
    return 1;
}