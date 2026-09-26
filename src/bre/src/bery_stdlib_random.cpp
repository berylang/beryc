#include "../include/bery_stdlib_random.h"
#include "../include/bery_alloc.h"
#include <random>
#include <cstring>
#include <cstdio>
#include <algorithm>

namespace {
    std::mt19937_64& engine() {
        static std::mt19937_64 eng{ 
            std::random_device{}() 
        };
        return eng;
    }

    
    BeryString* makeBeryString(const char* data, size_t length) {
        BeryString* s = static_cast<BeryString*>(bery_alloc(sizeof(BeryString), g_beryStringTypeId));
        s->length = length;
        s->data = static_cast<char*>(malloc(length + 1));
        if (length>0)
            memcpy(s->data, data, length);
        s->data[length]='\0';
        return s;
    }


    int32_t unboxInt(void* box) { return *static_cast<int32_t*>(box); }
    double unboxDouble(void* box) { return *static_cast<double*>(box); }
    bool unboxBool(void* box) { return *static_cast<bool*>(box); }
    BeryString* unboxString(void* box) { return *static_cast<BeryString**>(box); }

    size_t randomIndex(size_t length) {
        std::uniform_int_distribution<size_t> dist(0, length-1);
        return dist(engine());
    }

    void shuffleInPlace(BeryArray* arr) {
        if (arr->length < 2) return;
        for (size_t i = arr->length - 1; i > 0; --i) {
            size_t j = randomIndex(i + 1);
            std::swap(arr->data[i], arr->data[j]);
        }
    }
}

void __bery_random_seed(int64_t seedValue) {
    engine().seed(static_cast<uint64_t>(seedValue));
}

int32_t __bery_random_int() {
    std::uniform_int_distribution<int32_t> dist(INT32_MIN, INT32_MAX);
    return dist(engine());
}

int32_t __bery_random_int_range(int32_t minV, int32_t maxV) {
    if (minV > maxV) std::swap(minV, maxV);
    std::uniform_int_distribution<int32_t> dist(minV, maxV);
    return dist(engine());
}

int64_t __bery_random_bigint() {
    std::uniform_int_distribution<int64_t> dist(INT64_MIN, INT64_MAX);
    return dist(engine());
}

double __bery_random_double() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(engine());
}

double __bery_random_double_range(double minV, double maxV) {
    if (minV > maxV) std::swap(minV, maxV);
    if (minV == maxV) return minV;
    std::uniform_real_distribution<double> dist(minV, maxV);
    return dist(engine());
}

bool __bery_random_bool() {
    std::uniform_int_distribution<int32_t> dist(0, 1);
    return dist(engine()) == 1;
}

bool __bery_random_bool_chance(double probability) {
    if (probability <= 0.0) return false;
    if (probability >= 1.0) return true;
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(engine()) < probability;
}

int32_t __bery_random_choice_int(BeryArray* arr) {
    if (arr->length == 0) return 0;
    return unboxInt(arr->data[randomIndex(arr->length)]);
}

double __bery_random_choice_double(BeryArray* arr) {
    if (arr->length == 0) return 0.0;
    return unboxDouble(arr->data[randomIndex(arr->length)]);
}

bool __bery_random_choice_bool(BeryArray* arr) {
    if (arr->length == 0) return false;
    return unboxBool(arr->data[randomIndex(arr->length)]);
}

BeryString* __bery_random_choice_string(BeryArray* arr) {
    if (arr->length == 0) return makeBeryString("", 0);
    return unboxString(arr->data[randomIndex(arr->length)]);
}

void __bery_random_shuffle_int(BeryArray* arr){ shuffleInPlace(arr);}
void __bery_random_shuffle_double(BeryArray* arr) {shuffleInPlace(arr);}
void __bery_random_shuffle_bool(BeryArray* arr){shuffleInPlace(arr);}
void __bery_random_shuffle_string(BeryArray* arr) {shuffleInPlace(arr); }

BeryString* __bery_random_alpha_numeric(int32_t length) {
    if (length <= 0) return makeBeryString("", 0);
    static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    constexpr size_t alphabetSize = sizeof(alphabet) - 1;
    std::uniform_int_distribution<size_t> dist(0, alphabetSize - 1);

    std::string out(static_cast<size_t>(length), '\0');
    for (int32_t i = 0; i < length; ++i) {
        out[static_cast<size_t>(i)] = alphabet[dist(engine())];
    }
    return makeBeryString(out.data(), out.size());
}

BeryString* __bery_random_uuid() {
    static const char hexDigits[] = "0123456789abcdef";
    static const char variantDigits[] = "89ab";
    std::uniform_int_distribution<int> hexDist(0, 15);
    std::uniform_int_distribution<int> variantDist(0, 3);

    char buf[37];
    size_t pos = 0;
    for (int i = 0; i < 8; ++i) buf[pos++] = hexDigits[hexDist(engine())];

    buf[pos++] = '-';
    
    for (int i = 0; i < 4; ++i) buf[pos++] = hexDigits[hexDist(engine())];
    buf[pos++] = '-';
    buf[pos++] = '4';
    
    for (int i = 0; i < 3; ++i) buf[pos++] = hexDigits[hexDist(engine())];
    buf[pos++] = '-';
    buf[pos++] = variantDigits[variantDist(engine())];
    
    for (int i = 0; i < 3; ++i) buf[pos++] = hexDigits[hexDist(engine())];
    buf[pos++] = '-';
    
    for (int i = 0; i < 12; ++i) buf[pos++] = hexDigits[hexDist(engine())];

    return makeBeryString(buf, pos);
}