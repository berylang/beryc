#pragma once
#include "bery_string.h"
#include <cstdint>

extern "C" {
    void __bery_panic(BeryString* message);
    void __bery_exit(int32_t code);
}