#include "../include/bery_stdlib_core.h"
#include "../include/bery_native.h"
#include <cstdlib>

void __bery_panic(BeryString* message) {
    __bery_print_cstr("Bery: panic: ");
    bery_print_string(message);
    bery_println();
    bery_output_flush();
    std::exit(1);
}

void __bery_exit(int32_t code) {
    bery_output_flush();
    std::exit(code);
}