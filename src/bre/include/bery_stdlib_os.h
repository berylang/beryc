#pragma once
#include "bery_string.h"
#include "bery_array.h"

extern "C" {
    BeryString* __bery_os_get_env(BeryString* name);
    bool __bery_os_set_env(BeryString* name, BeryString* value);

    BeryString* __bery_os_get_cwd();
    bool __bery_os_set_cwd(BeryString* path);

    BeryArray* __bery_os_get_args();

    BeryString* __bery_os_platform();
    BeryString* __bery_os_hostname();
}