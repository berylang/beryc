#pragma once
#include "bery_string.h"
#include "bery_array.h"
#include <cstdint>

extern "C" {
    bool __bery_fs_exists(BeryString* path);
    bool __bery_fs_is_file(BeryString* path);
    bool __bery_fs_is_dir(BeryString* path);

    bool __bery_fs_create(BeryString* path);
    bool __bery_fs_create_dir(BeryString* path);

    bool __bery_fs_remove(BeryString* path);
    bool __bery_fs_remove_dir(BeryString* path);

    bool __bery_fs_rename(BeryString* oldPath, BeryString* newPath);
    bool __bery_fs_copy(BeryString* srcPath, BeryString* destPath);

    BeryString* __bery_fs_read(BeryString* path);
    bool __bery_fs_write(BeryString* path, BeryString* content);
    bool __bery_fs_append(BeryString* path, BeryString* content);

    int64_t     __bery_fs_size(BeryString* path);
    BeryArray*  __bery_fs_list(BeryString* path);
}