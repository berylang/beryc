#include "../include/bery_stdlib_os.h"
#include "../include/bery_alloc.h"
#include <cstdlib>
#include <cstring>
#include <string>

#if defined(_WIN32)
    #include <direct.h>
    #include <windows.h>
#elif defined(__APPLE__)
    #include <unistd.h>
    #include <crt_externs.h>
#else
    #include <unistd.h>
    #include <fstream>
#endif

namespace {
    constexpr size_t kPathBufferSize = 4096;
    constexpr size_t kHostNameBufferSize = 256;

    void* boxPointer(void* ptr) {
        void* box = bery_alloc(sizeof(void*), 0);
        std::memcpy(box, &ptr, sizeof(void*));
        return box;
    }
}

BeryString* __bery_os_get_env(BeryString* name) {
    const char* value = std::getenv(name->data);
    return bery_string_from_literal(value ? value : "");
}

bool __bery_os_set_env(BeryString* name, BeryString* value) {
#if defined(_WIN32)
    return _putenv_s(name->data, value->data) == 0;
#else
    return setenv(name->data, value->data, 1) == 0;
#endif
}

BeryString* __bery_os_get_cwd() {
    char buf[kPathBufferSize];
#if defined(_WIN32)
    if (_getcwd(buf, static_cast<int>(sizeof(buf))) == nullptr) return bery_string_from_literal("");
#else
    if (getcwd(buf, sizeof(buf)) == nullptr) return bery_string_from_literal("");
#endif
    return bery_string_from_literal(buf);
}

bool __bery_os_set_cwd(BeryString* path) {
#if defined(_WIN32)
    return _chdir(path->data) == 0;
#else
    return chdir(path->data) == 0;
#endif
}

BeryArray* __bery_os_get_args() {
#if defined(__APPLE__)
    int argc = *_NSGetArgc();
    char** argv = *_NSGetArgv();
    BeryArray* arr = bery_array_new(argc > 0 ? static_cast<size_t>(argc) : 1);
    for (int i = 0; i < argc; ++i) {
        bery_array_push(arr, boxPointer(bery_string_from_literal(argv[i])));
    }
    return arr;
#elif defined(_WIN32)
    BeryArray* arr = bery_array_new(__argc > 0 ? static_cast<size_t>(__argc) : 1);
    for (int i = 0; i < __argc; ++i) {
        bery_array_push(arr, boxPointer(bery_string_from_literal(__argv[i])));
    }
    return arr;
#else
    BeryArray* arr = bery_array_new(8);
    std::ifstream cmdline("/proc/self/cmdline", std::ios::binary);
    if (cmdline) {
        std::string arg;
        char ch;
        while (cmdline.get(ch)) {
            if (ch == '\0') {
                bery_array_push(arr, boxPointer(bery_string_from_literal(arg.c_str())));
                arg.clear();
            } else {
                arg.push_back(ch);
            }
        }
        if (!arg.empty()) {
            bery_array_push(arr, boxPointer(bery_string_from_literal(arg.c_str())));
        }
    }
    return arr;
#endif
}

BeryString* __bery_os_platform() {
#if defined(_WIN32)
    return bery_string_from_literal("windows");
#elif defined(__APPLE__)
    return bery_string_from_literal("macos");
#elif defined(__linux__)
    return bery_string_from_literal("linux");
#else
    return bery_string_from_literal("unknown");
#endif
}

BeryString* __bery_os_hostname() {
    char buf[kHostNameBufferSize];
#if defined(_WIN32)
    DWORD size = static_cast<DWORD>(sizeof(buf));
    if (!GetComputerNameA(buf, &size)) return bery_string_from_literal("");
    return bery_string_from_literal(buf);
#else
    if (gethostname(buf, sizeof(buf)) != 0) return bery_string_from_literal("");
    buf[sizeof(buf) - 1] = '\0';
    return bery_string_from_literal(buf);
#endif
}