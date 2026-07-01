#include <string>

std::string getExeDir(const char* argv);

int cmdCompile(const std::string& sourcePath, std::string& outBinaryPath, const std::string& exeDir);