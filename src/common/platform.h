#pragma once

#ifdef _WIN32
    #define BERY_WINDOWS
    #define BERY_EXEC_PREFIX ""
    #define BERY_PATH_SEP "\\"
#elif __APPLE__
    #define BERY_MACOS
    #define BERY_EXEC_PREFIX "./"
    #define BERY_PATH_SEP "/"
#elif __linux__
    #define BERY_LINUX
    #define BERY_EXEC_PREFIX "./"
    #define BERY_PATH_SEP "/"
#endif