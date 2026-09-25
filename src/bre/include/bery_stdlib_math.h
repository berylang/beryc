#pragma once

#include <cstdint>

extern "C" {
   // BASIC OPERATIONS
    int32_t __bery_math_abs_int(int32_t value);
    int64_t __bery_math_abs_bigint(int64_t value);
    float __bery_math_abs_float(float value);
    double __bery_math_abs_double(double value);

    int32_t __bery_math_min_int(int32_t a, int32_t b);
    int64_t __bery_math_min_bigint(int64_t a, int64_t b);
    float __bery_math_min_float(float a, float b);
    double __bery_math_min_double(double a, double b);

    int32_t __bery_math_max_int(int32_t a, int32_t b);
    int64_t __bery_math_max_bigint(int64_t a, int64_t b);
    float __bery_math_max_float(float a, float b);
    double __bery_math_max_double(double a, double b);

    int32_t __bery_math_mod_int(int32_t a, int32_t b);
    int64_t __bery_math_mod_bigint(int64_t a, int64_t b);

    // POWER AND ROOT FUNCTIONS
    double __bery_math_sqrt_int(int32_t value);
    double __bery_math_sqrt_bigint(int64_t value);
    float __bery_math_sqrt_float(float value);
    double __bery_math_sqrt_double(double value);

    double __bery_math_cbrt_int(int32_t value);
    double __bery_math_cbrt_bigint(int64_t value);
    float __bery_math_cbrt_float(float value);
    double __bery_math_cbrt_double(double value);

    double __bery_math_pow_int(int32_t base, int32_t exponent);
    double __bery_math_pow_bigint(int64_t base, int64_t exponent);
    float __bery_math_pow_float(float base, float exponent);
    double __bery_math_pow_double(double base, double exponent);

    double __bery_math_hypot_int(int32_t a, int32_t b);
    double __bery_math_hypot_bigint(int64_t a, int64_t b);
    float __bery_math_hypot_float(float a, float b);
    double __bery_math_hypot_double(double a, double b);

    // EXPONENTIAL AND LOGARITHMIC FUNCTIONS
    double __bery_math_exp_int(int32_t value);
    double __bery_math_exp_bigint(int64_t value);
    float __bery_math_exp_float(float value);
    double __bery_math_exp_double(double value);

    double __bery_math_exp2_int(int32_t value);
    double __bery_math_exp2_bigint(int64_t value);
    float __bery_math_exp2_float(float value);
    double __bery_math_exp2_double(double value);

    double __bery_math_expm1_int(int32_t value);
    double __bery_math_expm1_bigint(int64_t value);
    float __bery_math_expm1_float(float value);
    double __bery_math_expm1_double(double value);

    double __bery_math_log_int(int32_t value);
    double __bery_math_log_bigint(int64_t value);
    float __bery_math_log_float(float value);
    double __bery_math_log_double(double value);

    double __bery_math_log10_int(int32_t value);
    double __bery_math_log10_bigint(int64_t value);
    float __bery_math_log10_float(float value);
    double __bery_math_log10_double(double value);

    double __bery_math_log2_int(int32_t value);
    double __bery_math_log2_bigint(int64_t value);
    float __bery_math_log2_float(float value);
    double __bery_math_log2_double(double value);

    double __bery_math_log1p_int(int32_t value);
    double __bery_math_log1p_bigint(int64_t value);
    float __bery_math_log1p_float(float value);
    double __bery_math_log1p_double(double value);

    // ROUNDING FUNCTIONS
    double __bery_math_floor_int(int32_t value);
    double __bery_math_floor_bigint(int64_t value);
    float __bery_math_floor_float(float value);
    double __bery_math_floor_double(double value);

    double __bery_math_ceil_int(int32_t value);
    double __bery_math_ceil_bigint(int64_t value);
    float __bery_math_ceil_float(float value);
    double __bery_math_ceil_double(double value);

    double __bery_math_trunc_int(int32_t value);
    double __bery_math_trunc_bigint(int64_t value);
    float __bery_math_trunc_float(float value);
    double __bery_math_trunc_double(double value);

    double __bery_math_round_int(int32_t value);
    double __bery_math_round_bigint(int64_t value);
    float __bery_math_round_float(float value);
    double __bery_math_round_double(double value);

    int32_t __bery_math_lround_int(int32_t value);
    int64_t __bery_math_lround_bigint(int64_t value);
    int32_t __bery_math_lround_float(float value);
    int32_t __bery_math_lround_double(double value);

    int64_t __bery_math_llround_int(int32_t value);
    int64_t __bery_math_llround_bigint(int64_t value);
    int64_t __bery_math_llround_float(float value);
    int64_t __bery_math_llround_double(double value);


    // INTEGER FUNCTIONS
    int32_t __bery_math_gcd_int(int32_t a, int32_t b);
    int64_t __bery_math_gcd_bigint(int64_t a, int64_t b);

    int32_t __bery_math_lcm_int(int32_t a, int32_t b);
    int64_t __bery_math_lcm_bigint(int64_t a, int64_t b);

    int64_t __bery_math_factorial_int(int32_t value);
    int64_t __bery_math_factorial_bigint(int64_t value);

    // NUMERIC FUNCTIONS
    int32_t __bery_math_midpoint_int(int32_t a, int32_t b);
    int64_t __bery_math_midpoint_bigint(int64_t a, int64_t b);
    float __bery_math_midpoint_float(float a, float b);
    double __bery_math_midpoint_double(double a, double b);

    float __bery_math_lerp_float(float a, float b, float t);
    double __bery_math_lerp_double(double a, double b, double t);


    // FLOATING-POINT FUNCTIONS
    float __bery_math_fmod_float(float a, float b);
    double __bery_math_fmod_double(double a, double b);

    float __bery_math_remainder_float(float a, float b);
    double __bery_math_remainder_double(double a, double b);

    float __bery_math_fma_float(float a, float b, float c);
    double __bery_math_fma_double(double a, double b, double c);

    float __bery_math_fmin_float(float a, float b);
    double __bery_math_fmin_double(double a, double b);

    float __bery_math_fmax_float(float a, float b);
    double __bery_math_fmax_double(double a, double b);

    float __bery_math_fdim_float(float a, float b);
    double __bery_math_fdim_double(double a, double b);

    float __bery_math_copysign_float(float a, float b);
    double __bery_math_copysign_double(double a, double b);

    float __bery_math_ldexp_float(float value, int32_t exponent);
    double __bery_math_ldexp_double(double value, int32_t exponent);

    float __bery_math_scalbn_float(float value, int32_t exponent);
    double __bery_math_scalbn_double(double value, int32_t exponent);

    float __bery_math_scalbln_float(float value, int64_t exponent);
    double __bery_math_scalbln_double(double value, int64_t exponent);

    int32_t __bery_math_ilogb_float(float value);
    int32_t __bery_math_ilogb_double(double value);

    float __bery_math_logb_float(float value);
    double __bery_math_logb_double(double value);

    float __bery_math_nextafter_float(float from, float to);
    double __bery_math_nextafter_double(double from, double to);

    double __bery_math_nexttoward_float(float from, double to);
    double __bery_math_nexttoward_double(double from, double to);

    // TRIGONOMETRIC FUNCTIONS
    double __bery_math_sin_int(int32_t value);
    double __bery_math_sin_bigint(int64_t value);
    float __bery_math_sin_float(float value);
    double __bery_math_sin_double(double value);

    double __bery_math_cos_int(int32_t value);
    double __bery_math_cos_bigint(int64_t value);
    float __bery_math_cos_float(float value);
    double __bery_math_cos_double(double value);

    double __bery_math_tan_int(int32_t value);
    double __bery_math_tan_bigint(int64_t value);
    float __bery_math_tan_float(float value);
    double __bery_math_tan_double(double value);

    double __bery_math_asin_int(int32_t value);
    double __bery_math_asin_bigint(int64_t value);
    float __bery_math_asin_float(float value);
    double __bery_math_asin_double(double value);

    double __bery_math_acos_int(int32_t value);
    double __bery_math_acos_bigint(int64_t value);
    float __bery_math_acos_float(float value);
    double __bery_math_acos_double(double value);

    double __bery_math_atan_int(int32_t value);
    double __bery_math_atan_bigint(int64_t value);
    float __bery_math_atan_float(float value);
    double __bery_math_atan_double(double value);

    double __bery_math_atan2_int(int32_t y, int32_t x);
    double __bery_math_atan2_bigint(int64_t y, int64_t x);
    float __bery_math_atan2_float(float y, float x);
    double __bery_math_atan2_double(double y, double x);


    // HYPERBOLIC FUNCTIONS
    double __bery_math_sinh_int(int32_t value);
    double __bery_math_sinh_bigint(int64_t value);
    float __bery_math_sinh_float(float value);
    double __bery_math_sinh_double(double value);

    double __bery_math_cosh_int(int32_t value);
    double __bery_math_cosh_bigint(int64_t value);
    float __bery_math_cosh_float(float value);
    double __bery_math_cosh_double(double value);

    double __bery_math_tanh_int(int32_t value);
    double __bery_math_tanh_bigint(int64_t value);
    float __bery_math_tanh_float(float value);
    double __bery_math_tanh_double(double value);

    double __bery_math_asinh_int(int32_t value);
    double __bery_math_asinh_bigint(int64_t value);
    float __bery_math_asinh_float(float value);
    double __bery_math_asinh_double(double value);

    double __bery_math_acosh_int(int32_t value);
    double __bery_math_acosh_bigint(int64_t value);
    float __bery_math_acosh_float(float value);
    double __bery_math_acosh_double(double value);

    double __bery_math_atanh_int(int32_t value);
    double __bery_math_atanh_bigint(int64_t value);
    float __bery_math_atanh_float(float value);
    double __bery_math_atanh_double(double value);


    // SPECIAL FUNCTIONS
    double __bery_math_erf_int(int32_t value);
    double __bery_math_erf_bigint(int64_t value);
    float __bery_math_erf_float(float value);
    double __bery_math_erf_double(double value);

    double __bery_math_erfc_int(int32_t value);
    double __bery_math_erfc_bigint(int64_t value);
    float __bery_math_erfc_float(float value);
    double __bery_math_erfc_double(double value);

    double __bery_math_lgamma_int(int32_t value);
    double __bery_math_lgamma_bigint(int64_t value);
    float __bery_math_lgamma_float(float value);
    double __bery_math_lgamma_double(double value);

    double __bery_math_tgamma_int(int32_t value);
    double __bery_math_tgamma_bigint(int64_t value);
    float __bery_math_tgamma_float(float value);
    double __bery_math_tgamma_double(double value);

    // MATHEMATICAL CONSTANTS
    double __bery_math_pi();
    double __bery_math_e();
    double __bery_math_tau();
    double __bery_math_log2e();
    double __bery_math_log10e();
    double __bery_math_inv_pi();
    double __bery_math_inv_sqrt_pi();
    double __bery_math_ln2();
    double __bery_math_ln10();
    double __bery_math_sqrt2();
    double __bery_math_sqrt3();
    double __bery_math_inv_sqrt3();
    double __bery_math_euler_gamma();
    double __bery_math_phi();
}