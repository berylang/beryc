#pragma once
#include <cstdint>
#include <cstddef>

struct BeryString {
    size_t length;
    char* data;
};

extern "C" {
    void bery_string_init_type();

    BeryString* bery_string_from_literal(const char* cstr);
    BeryString* bery_string_copy(BeryString* src);
    BeryString* bery_string_concat(BeryString* a, BeryString* b);
    bool bery_string_equals(BeryString* a, BeryString* b);
    size_t bery_string_length(BeryString* a);
    BeryString* bery_string_substring(BeryString* str, size_t start, size_t end);
    char bery_string_char_at(BeryString* str, size_t index);


}

extern uint32_t g_beryStringTypeId;