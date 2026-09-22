#include "../include/bery_stdlib_fs.h"
#include "../include/bery_alloc.h"
#include <filesystem>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <string>

namespace fs = std::filesystem;

namespace {
    BeryString* makeBeryString(const char* data, size_t length) {
        BeryString* s = static_cast<BeryString*>(bery_alloc(sizeof(BeryString), g_beryStringTypeId));
        s->length = length;
        s->data = static_cast<char*>(malloc(length + 1));
        if (length > 0) {
            memcpy(s->data, data, length);
        }
        s->data[length] = '\0';
        return s;
    }
    void* boxPointer(void* ptr) {
        void* box = bery_alloc(sizeof(void*), 0);
        std::memcpy(box, &ptr, sizeof(void*));
        return box;
    }
}

bool __bery_fs_exists(BeryString* path) {
    std::error_code ec;
    return fs::exists(path->data, ec);
}

bool __bery_fs_is_file(BeryString* path) {
    std::error_code ec;
    return fs::is_regular_file(path->data, ec);
}

bool __bery_fs_is_dir(BeryString* path) {
    std::error_code ec;
    return fs::is_directory(path->data, ec);
}

bool __bery_fs_create(BeryString* path) {
    std::ofstream out(path->data, std::ios::app | std::ios::binary);
    return out.good();
}

bool __bery_fs_create_dir(BeryString* path) {
    std::error_code ec;
    if (fs::is_directory(path->data, ec)) return true;
    return fs::create_directory(path->data, ec) && !ec;
}

bool __bery_fs_remove(BeryString* path) {
    std::error_code ec;
    if (!fs::is_regular_file(path->data, ec)) return false;
    return fs::remove(path->data, ec) && !ec;
}

bool __bery_fs_remove_dir(BeryString* path) {
    std::error_code ec;
    if (!fs::is_directory(path->data, ec)) return false;
    return fs::remove(path->data, ec) && !ec;
}

bool __bery_fs_rename(BeryString* oldPath, BeryString* newPath) {
    std::error_code ec;
    fs::rename(oldPath->data, newPath->data, ec);
    return !ec;
}

bool __bery_fs_copy(BeryString* srcPath, BeryString* destPath) {
    std::error_code ec;
    fs::copy_file(srcPath->data, destPath->data, fs::copy_options::overwrite_existing, ec);
    return !ec;
}

BeryString* __bery_fs_read(BeryString* path) {
    std::ifstream in(path->data, std::ios::binary | std::ios::ate);
    if (!in) return makeBeryString("", 0);

    std::streamsize size = in.tellg();
    if (size < 0) return makeBeryString("", 0);
    in.seekg(0, std::ios::beg);

    std::string buffer(static_cast<size_t>(size), '\0');
    if (size > 0 && !in.read(&buffer[0], size)) return makeBeryString("", 0);

    return makeBeryString(buffer.data(), buffer.size());
}

bool __bery_fs_write(BeryString* path, BeryString* content) {
    std::ofstream out(path->data, std::ios::binary | std::ios::trunc);
    if (!out) return false;
    if (content->length > 0) {
        out.write(content->data, static_cast<std::streamsize>(content->length));
    }
    return out.good();
}

bool __bery_fs_append(BeryString* path, BeryString* content) {
    std::ofstream out(path->data, std::ios::binary | std::ios::app);
    if (!out) return false;
    if (content->length > 0) {
        out.write(content->data, static_cast<std::streamsize>(content->length));
    }
    return out.good();
}

int64_t __bery_fs_size(BeryString* path) {
    std::error_code ec;
    uintmax_t sz = fs::file_size(path->data, ec);
    if (ec) return -1;
    return static_cast<int64_t>(sz);
}

BeryArray* __bery_fs_list(BeryString* path) {
    BeryArray* arr = bery_array_new(8);

    std::error_code ec;
    if (!fs::is_directory(path->data, ec)) return arr;

    for (auto it = fs::directory_iterator(path->data, ec); !ec && it != fs::directory_iterator(); it.increment(ec)) {
        std::string name = it->path().filename().string();
        bery_array_push(arr, boxPointer(makeBeryString(name.data(), name.size())));
    }

    return arr;
}