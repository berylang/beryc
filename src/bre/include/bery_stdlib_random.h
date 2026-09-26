#pragma once
#include "bery_string.h"
#include "bery_array.h"
#include <cstdint>

extern "C" {
    void __bery_random_seed(int64_t seedValue);

    int32_t __bery_random_int();
    int32_t __bery_random_int_range(int32_t minV, int32_t maxV);
    int64_t __bery_random_bigint();

    double  __bery_random_double();
    double  __bery_random_double_range(double minV, double maxV);

    bool __bery_random_bool();
    bool  __bery_random_bool_chance(double probability);

    int32_t __bery_random_choice_int(BeryArray* arr );

    double __bery_random_choice_double(BeryArray* arr);
    bool __bery_random_choice_bool(BeryArray* arr);
    BeryString* __bery_random_choice_string(BeryArray* arr);

    void __bery_random_shuffle_int(BeryArray* arr);
    void __bery_random_shuffle_double(BeryArray* arr);
    void __bery_random_shuffle_bool(BeryArray* arr);
    void __bery_random_shuffle_string(BeryArray* arr);



    BeryString* __bery_random_alpha_numeric(int32_t length);
    BeryString* __bery_random_uuid();
}