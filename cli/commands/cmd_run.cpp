#include "cmd_run.h"
#include "cmd_compile.h"
#include <cstdlib>
#include <cstdio>
#include <string>


int cmdRun(const std::string& sourcePath, const std::string& exeDir) {
    std::string binaryPath;
    int r = cmdCompile(sourcePath, binaryPath, exeDir);
    if (r != 0) return r;

    std::string runCmd = "\"" + binaryPath + "\"";
    int exitCode = system(runCmd.c_str());
    remove(binaryPath.c_str());
    return exitCode;
}