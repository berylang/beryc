#pragma once
#include <cstdint>

extern "C" {
    void __bery_time_start();
    void __bery_time_stop();
    double __bery_time_elapsed();

    void __bery_time_delay_ms(int32_t milliseconds);
    void __bery_time_delay_sec(double seconds);
}