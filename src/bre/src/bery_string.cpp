#include "../include/bery_string.h"
#include "../include/bery_alloc.h"
#include "../include/bery_type_registry.h"
#include "../include/bery_object.h"
#include <cstring>
#include <cstdlib>
#include <string>
#include <cstdint>

uint32_t g_beryStringTypeId = 0;

static void stringDestructor(void* payload) {
    BeryString* s = static_cast<BeryString*>(payload);
    free(s->data);
}

void bery_string_init_type() {
    g_beryStringTypeId = bery_type_register("string", sizeof(BeryString), nullptr, 0, stringDestructor);   
}

BeryString* bery_string_from_literal(const char* cstr) {
    size_t len = strlen(cstr);
    BeryString* s = static_cast<BeryString*>(bery_alloc(sizeof(BeryString), g_beryStringTypeId));

    s->length = len;
    s->data = static_cast<char*>(malloc(len + 1));
    memcpy(s->data, cstr, len + 1);
    return s;
}

BeryString* bery_string_concat(BeryString* a, BeryString* b) {
    size_t newLen = a->length + b->length;
    BeryString* s = static_cast<BeryString*>(bery_alloc(sizeof(BeryString), g_beryStringTypeId));

    s->length = newLen;
    s->data = static_cast<char*>(malloc(newLen + 1));
    memcpy(s->data, a->data, a->length);
    memcpy(s->data + a->length, b->data, b->length + 1);
    return s;

}

BeryString* bery_string_copy(BeryString* src) {
    BeryString* s = static_cast<BeryString*>(bery_alloc(sizeof(BeryString), g_beryStringTypeId));
    s->length = src->length;
    s->data = static_cast<char*>(malloc(src->length + 1));
    memcpy(s->data, src->data, src->length + 1);
    return s;
}

bool bery_string_equals(BeryString* a, BeryString* b) {
    if(a->length != b->length) return false;
    return (memcmp(a->data, b->data, a->length) == 0);
}

size_t bery_string_length(BeryString* a) {
    return a->length;
}

BeryString* bery_string_substring(BeryString* str, size_t start, size_t end) {
    size_t newLen = end - start;
    BeryString* s = static_cast<BeryString*>(bery_alloc(sizeof(BeryString), g_beryStringTypeId));
    s->length = newLen;
    s->data = static_cast<char*>(malloc(newLen + 1));
    memcpy(s->data, str->data + start, newLen);
    s->data[newLen] = '\0';
    return s;
}
BeryString* bery_to_string_int(int32_t value) {
    std::string s = std::to_string(value);
    return bery_string_from_literal(s.c_str());
}

BeryString* bery_to_string_bigint(int64_t value) {
    std::string s = std::to_string(value);
    return bery_string_from_literal(s.c_str());
}

BeryString* bery_to_string_float(float value) {
    std::string s = std::to_string(value);
    return bery_string_from_literal(s.c_str());
}

BeryString* bery_to_string_double(double value) {
    std::string s = std::to_string(value);
    return bery_string_from_literal(s.c_str());
}

BeryString* bery_to_string_char(char value) {
    char str[2] = { value, '\0' };
    return bery_string_from_literal(str);
}

BeryString* bery_to_string_bool(bool value) {
    return bery_string_from_literal(value ? "true" : "false");
}
char bery_string_char_at(BeryString* str, size_t index) {
    return str->data[index];
    
}