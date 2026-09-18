#include "../include/bery_stdlib_time.h"
#include <chrono>
#include <ctime>

namespace {
    using Clock = std::chrono::steady_clock;

    Clock::time_point g_start;
    Clock::time_point g_stop;
    bool g_hasStarted = false;
    bool g_isRunning=false;
}

void __bery_time_start() {
    g_start= Clock::now();
    g_hasStarted = true;
    g_isRunning =true;
}

void __bery_time_stop() {
    if (g_isRunning) {
        g_stop= Clock::now();
        g_isRunning = false;
    }
}

double __bery_time_elapsed() {
    if (!g_hasStarted) return 0.0;

    Clock::time_point end = g_isRunning ? Clock::now():g_stop;
    std::chrono::duration<double> diff = end -g_start;
    return diff.count();
}

void __bery_time_delay_sec(double seconds) {
    if (seconds <= 0.0) return;

    struct timespec ts;
    ts.tv_sec  = static_cast<time_t>(seconds);
    ts.tv_nsec = static_cast<long>((seconds - static_cast<double>(ts.tv_sec)) * 1e9);
    nanosleep(&ts, nullptr);
}

void __bery_time_delay_ms(int32_t milliseconds) {
    if (milliseconds <= 0) return;
    struct timespec ts;
    ts.tv_sec  = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;
    nanosleep(&ts, nullptr);
}