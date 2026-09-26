#include "../include/bery_stdlib_math.h"

#include <cmath>
#include <cstdint>
#include <algorithm>

extern "C" {

    // BASIC OPERATIONS
    int32_t __bery_math_abs_int(int32_t value) {
        return std::abs(value);
    }
    int64_t __bery_math_abs_bigint(int64_t value) {
        return std::abs(value);
    }
    float __bery_math_abs_float(float value) {
        return std::fabs(value);
    }
    double __bery_math_abs_double(double value) {
        return std::fabs(value);
    }

    int32_t __bery_math_min_int(int32_t a, int32_t b) {
        return std::min(a, b);
    }
    int64_t __bery_math_min_bigint(int64_t a, int64_t b) {
        return std::min(a, b);
    }
    float __bery_math_min_float(float a, float b) {
        return std::fmin(a, b);
    }
    double __bery_math_min_double(double a, double b) {
        return std::fmin(a, b);
    }

    int32_t __bery_math_max_int(int32_t a, int32_t b) {
        return std::max(a, b);
    }
    int64_t __bery_math_max_bigint(int64_t a, int64_t b) {
        return std::max(a, b);
    }
    float __bery_math_max_float(float a, float b) {
        return std::fmax(a, b);
    }
    double __bery_math_max_double(double a, double b) {
        return std::fmax(a, b);
    }

    int32_t __bery_math_mod_int(int32_t a, int32_t b) {
        return a % b;
    }
    int64_t __bery_math_mod_bigint(int64_t a, int64_t b) {
        return a % b;
    }


    // POWER AND ROOT FUNCTIONS
    double __bery_math_sqrt_int(int32_t value) {
        return std::sqrt(static_cast<double>(value));
    }
    double __bery_math_sqrt_bigint(int64_t value) {
        return std::sqrt(static_cast<double>(value));
    }
    float __bery_math_sqrt_float(float value) {
        return std::sqrt(value);
    }
    double __bery_math_sqrt_double(double value) {
        return std::sqrt(value);
    }


    double __bery_math_cbrt_int(int32_t value) {
        return std::cbrt(static_cast<double>(value));
    }
    double __bery_math_cbrt_bigint(int64_t value) {
        return std::cbrt(static_cast<double>(value));
    }
    float __bery_math_cbrt_float(float value) {
        return std::cbrt(value);
    }
    double __bery_math_cbrt_double(double value) {
        return std::cbrt(value);
    }


    double __bery_math_pow_int(int32_t base, int32_t exponent) {
        return std::pow(static_cast<double>(base), static_cast<double>(exponent));
    }
    double __bery_math_pow_bigint(int64_t base, int64_t exponent) {
        return std::pow(static_cast<double>(base), static_cast<double>(exponent));
    }
    float __bery_math_pow_float(float base, float exponent) {
        return std::pow(base, exponent);
    }
    double __bery_math_pow_double(double base, double exponent) {
        return std::pow(base, exponent);
    }

    double __bery_math_hypot_int(int32_t a, int32_t b) {
        return std::hypot(static_cast<double>(a), static_cast<double>(b));
    }
    double __bery_math_hypot_bigint(int64_t a, int64_t b) {
        return std::hypot(static_cast<double>(a), static_cast<double>(b));
    }
    float __bery_math_hypot_float(float a, float b) {
        return std::hypot(a, b);
    }
    double __bery_math_hypot_double(double a, double b) {
        return std::hypot(a, b);
    }


    // EXPONENTIAL AND LOGARITHMIC FUNCTIONS
    double __bery_math_exp_int(int32_t value) {
        return std::exp(static_cast<double>(value));
    }
    double __bery_math_exp_bigint(int64_t value) {
        return std::exp(static_cast<double>(value));
    }
    float __bery_math_exp_float(float value) {
        return std::exp(value);
    }
    double __bery_math_exp_double(double value) {
        return std::exp(value);
    }


    double __bery_math_exp2_int(int32_t value) {
        return std::exp2(static_cast<double>(value));
    }
    double __bery_math_exp2_bigint(int64_t value) {
        return std::exp2(static_cast<double>(value));
    }
    float __bery_math_exp2_float(float value) {
        return std::exp2(value);
    }
    double __bery_math_exp2_double(double value) {
        return std::exp2(value);
    }


    double __bery_math_expm1_int(int32_t value) {
        return std::expm1(static_cast<double>(value));
    }
    double __bery_math_expm1_bigint(int64_t value) {
        return std::expm1(static_cast<double>(value));
    }
    float __bery_math_expm1_float(float value) {
        return std::expm1(value);
    }
    double __bery_math_expm1_double(double value) {
        return std::expm1(value);
    }


    double __bery_math_log_int(int32_t value) {
        return std::log(static_cast<double>(value));
    }
    double __bery_math_log_bigint(int64_t value) {
        return std::log(static_cast<double>(value));
    }
    float __bery_math_log_float(float value) {
        return std::log(value);
    }
    double __bery_math_log_double(double value) {
        return std::log(value);
    }


    double __bery_math_log10_int(int32_t value) {
        return std::log10(static_cast<double>(value));
    }
    double __bery_math_log10_bigint(int64_t value) {
        return std::log10(static_cast<double>(value));
    }
    float __bery_math_log10_float(float value) {
        return std::log10(value);
    }
    double __bery_math_log10_double(double value) {
        return std::log10(value);
    }


    double __bery_math_log2_int(int32_t value) {
        return std::log2(static_cast<double>(value));
    }
    double __bery_math_log2_bigint(int64_t value) {
        return std::log2(static_cast<double>(value));
    }
    float __bery_math_log2_float(float value) {
        return std::log2(value);
    }
    double __bery_math_log2_double(double value) {
        return std::log2(value);
    }


    double __bery_math_log1p_int(int32_t value) {
        return std::log1p(static_cast<double>(value));
    }
    double __bery_math_log1p_bigint(int64_t value) {
        return std::log1p(static_cast<double>(value));
    }
    float __bery_math_log1p_float(float value) {
        return std::log1p(value);
    }
    double __bery_math_log1p_double(double value) {
        return std::log1p(value);
    }


    // ROUNDING FUNCTIONS
    double __bery_math_floor_int(int32_t value) {
        return std::floor(static_cast<double>(value));
    }
    double __bery_math_floor_bigint(int64_t value) {
        return std::floor(static_cast<double>(value));
    }

    float __bery_math_floor_float(float value) {
        return std::floor(value);
    }

    double __bery_math_floor_double(double value) {
        return std::floor(value);
    }


    double __bery_math_ceil_int(int32_t value) {
        return std::ceil(static_cast<double>(value));
    }

    double __bery_math_ceil_bigint(int64_t value) {
        return std::ceil(static_cast<double>(value));
    }

    float __bery_math_ceil_float(float value) {
        return std::ceil(value);
    }

    double __bery_math_ceil_double(double value) {
        return std::ceil(value);
    }


    double __bery_math_trunc_int(int32_t value) {
        return std::trunc(static_cast<double>(value));
    }

    double __bery_math_trunc_bigint(int64_t value) {
        return std::trunc(static_cast<double>(value));
    }

    float __bery_math_trunc_float(float value) {
        return std::trunc(value);
    }

    double __bery_math_trunc_double(double value) {
        return std::trunc(value);
    }


    double __bery_math_round_int(int32_t value) {
        return std::round(static_cast<double>(value));
    }

    double __bery_math_round_bigint(int64_t value) {
        return std::round(static_cast<double>(value));
    }

    float __bery_math_round_float(float value) {
        return std::round(value);
    }

    double __bery_math_round_double(double value) {
        return std::round(value);
    }


    int32_t __bery_math_lround_int(int32_t value) {
        return std::lround(static_cast<double>(value));
    }

    int64_t __bery_math_lround_bigint(int64_t value) {
        return std::llround(static_cast<double>(value));
    }

    int32_t __bery_math_lround_float(float value) {
        return std::lround(value);
    }

    int32_t __bery_math_lround_double(double value) {
        return std::lround(value);
    }


    int64_t __bery_math_llround_int(int32_t value) {
        return std::llround(static_cast<double>(value));
    }

    int64_t __bery_math_llround_bigint(int64_t value) {
        return std::llround(static_cast<double>(value));
    }

    int64_t __bery_math_llround_float(float value) {
        return std::llround(value);
    }

    int64_t __bery_math_llround_double(double value) {
        return std::llround(value);
    }



    // INTEGER FUNCTIONS

    int32_t __bery_math_gcd_int(int32_t a, int32_t b)
    {
        while (b != 0)
        {
            int32_t remainder = a % b;
            a = b;
            b = remainder;
        }

        return a < 0 ? -a : a;
    }


    int64_t __bery_math_gcd_bigint(int64_t a, int64_t b)
    {
        while (b != 0)
        {
            int64_t remainder = a % b;
            a = b;
            b = remainder;
        }

        return a < 0 ? -a : a;
    }


    int32_t __bery_math_lcm_int(int32_t a, int32_t b)
    {
        if (a == 0 || b == 0)
        {
            return 0;
        }

        int32_t gcd = __bery_math_gcd_int(a, b);
        int32_t result = (a / gcd) * b;

        return result < 0 ? -result : result;
    }


    int64_t __bery_math_lcm_bigint(int64_t a, int64_t b)
    {
        if (a == 0 || b == 0)
        {
            return 0;
        }

        int64_t gcd = __bery_math_gcd_bigint(a, b);
        int64_t result = (a / gcd) * b;

        return result < 0 ? -result : result;
    }


    int64_t __bery_math_factorial_int(int32_t value)
    {
        if (value < 0)
        {
            return 0;
        }

        int64_t result = 1;

        for (int32_t i = 2; i <= value; ++i)
        {
            result *= i;
        }

        return result;
    }


    int64_t __bery_math_factorial_bigint(int64_t value)
    {
        if (value < 0)
        {
            return 0;
        }

        int64_t result = 1;

        for (int64_t i = 2; i <= value; ++i)
        {
            result *= i;
        }

        return result;
    }




    // NUMERIC FUNCTIONS

    int32_t __bery_math_midpoint_int(int32_t a, int32_t b)
    {
        return a + (b - a) / 2;
    }


    int64_t __bery_math_midpoint_bigint(int64_t a, int64_t b)
    {
        return a + (b - a) / 2;
    }


    float __bery_math_midpoint_float(float a, float b)
    {
        return a + (b - a) / 2.0f;
    }


    double __bery_math_midpoint_double(double a, double b)
    {
        return a + (b - a) / 2.0;
    }


    float __bery_math_lerp_float(float a, float b, float t)
    {
        return a + (b - a) * t;
    }


    double __bery_math_lerp_double(double a, double b, double t)
    {
        return a + (b - a) * t;
    }


    // FLOATING-POINT FUNCTIONS

    float __bery_math_fmod_float(float a, float b)
    {
        return std::fmod(a, b);
    }

    double __bery_math_fmod_double(double a, double b)
    {
        return std::fmod(a, b);
    }


    float __bery_math_remainder_float(float a, float b)
    {
        return std::remainder(a, b);
    }

    double __bery_math_remainder_double(double a, double b)
    {
        return std::remainder(a, b);
    }


    float __bery_math_fma_float(float a, float b, float c)
    {
        return std::fma(a, b, c);
    }

    double __bery_math_fma_double(double a, double b, double c)
    {
        return std::fma(a, b, c);
    }


    float __bery_math_fmin_float(float a, float b)
    {
        return std::fmin(a, b);
    }

    double __bery_math_fmin_double(double a, double b)
    {
        return std::fmin(a, b);
    }


    float __bery_math_fmax_float(float a, float b)
    {
        return std::fmax(a, b);
    }

    double __bery_math_fmax_double(double a, double b)
    {
        return std::fmax(a, b);
    }


    float __bery_math_fdim_float(float a, float b)
    {
        return std::fdim(a, b);
    }

    double __bery_math_fdim_double(double a, double b)
    {
        return std::fdim(a, b);
    }


    float __bery_math_copysign_float(float a, float b)
    {
        return std::copysign(a, b);
    }

    double __bery_math_copysign_double(double a, double b)
    {
        return std::copysign(a, b);
    }


    float __bery_math_ldexp_float(float value, int32_t exponent)
    {
        return std::ldexp(value, exponent);
    }

    double __bery_math_ldexp_double(double value, int32_t exponent)
    {
        return std::ldexp(value, exponent);
    }


    float __bery_math_scalbn_float(float value, int32_t exponent)
    {
        return std::scalbn(value, exponent);
    }

    double __bery_math_scalbn_double(double value, int32_t exponent)
    {
        return std::scalbn(value, exponent);
    }


    float __bery_math_scalbln_float(float value, int64_t exponent)
    {
        return std::scalbln(value, static_cast<long>(exponent));
    }

    double __bery_math_scalbln_double(double value, int64_t exponent)
    {
        return std::scalbln(value, static_cast<long>(exponent));
    }


    int32_t __bery_math_ilogb_float(float value)
    {
        return std::ilogb(value);
    }

    int32_t __bery_math_ilogb_double(double value)
    {
        return std::ilogb(value);
    }


    float __bery_math_logb_float(float value)
    {
        return std::logb(value);
    }

    double __bery_math_logb_double(double value)
    {
        return std::logb(value);
    }


    float __bery_math_nextafter_float(float from, float to)
    {
        return std::nextafter(from, to);
    }

    double __bery_math_nextafter_double(double from, double to)
    {
        return std::nextafter(from, to);
    }


    double __bery_math_nexttoward_float(float from, double to)
    {
        return std::nexttoward(from, static_cast<long double>(to));
    }

    double __bery_math_nexttoward_double(double from, double to)
    {
        return std::nexttoward(from, static_cast<long double>(to));
    }


    // TRIGONOMETRIC FUNCTIONS
    double __bery_math_sin_int(int32_t value)
    {
        return std::sin(static_cast<double>(value));
    }

    double __bery_math_sin_bigint(int64_t value)
    {
        return std::sin(static_cast<double>(value));
    }

    float __bery_math_sin_float(float value)
    {
        return std::sin(value);
    }

    double __bery_math_sin_double(double value)
    {
        return std::sin(value);
    }


    double __bery_math_cos_int(int32_t value)
    {
        return std::cos(static_cast<double>(value));
    }

    double __bery_math_cos_bigint(int64_t value)
    {
        return std::cos(static_cast<double>(value));
    }

    float __bery_math_cos_float(float value)
    {
        return std::cos(value);
    }

    double __bery_math_cos_double(double value)
    {
        return std::cos(value);
    }


    double __bery_math_tan_int(int32_t value)
    {
        return std::tan(static_cast<double>(value));
    }

    double __bery_math_tan_bigint(int64_t value)
    {
        return std::tan(static_cast<double>(value));
    }

    float __bery_math_tan_float(float value)
    {
        return std::tan(value);
    }

    double __bery_math_tan_double(double value)
    {
        return std::tan(value);
    }


    double __bery_math_asin_int(int32_t value)
    {
        return std::asin(static_cast<double>(value));
    }

    double __bery_math_asin_bigint(int64_t value)
    {
        return std::asin(static_cast<double>(value));
    }

    float __bery_math_asin_float(float value)
    {
        return std::asin(value);
    }

    double __bery_math_asin_double(double value)
    {
        return std::asin(value);
    }


    double __bery_math_acos_int(int32_t value)
    {
        return std::acos(static_cast<double>(value));
    }

    double __bery_math_acos_bigint(int64_t value)
    {
        return std::acos(static_cast<double>(value));
    }

    float __bery_math_acos_float(float value)
    {
        return std::acos(value);
    }

    double __bery_math_acos_double(double value)
    {
        return std::acos(value);
    }


    double __bery_math_atan_int(int32_t value)
    {
        return std::atan(static_cast<double>(value));
    }

    double __bery_math_atan_bigint(int64_t value)
    {
        return std::atan(static_cast<double>(value));
    }

    float __bery_math_atan_float(float value)
    {
        return std::atan(value);
    }

    double __bery_math_atan_double(double value)
    {
        return std::atan(value);
    }


    double __bery_math_atan2_int(int32_t y, int32_t x)
    {
        return std::atan2(static_cast<double>(y), static_cast<double>(x));
    }

    double __bery_math_atan2_bigint(int64_t y, int64_t x)
    {
        return std::atan2(static_cast<double>(y), static_cast<double>(x));
    }

    float __bery_math_atan2_float(float y, float x)
    {
        return std::atan2(y, x);
    }

    double __bery_math_atan2_double(double y, double x)
    {
        return std::atan2(y, x);
    }





    // HYPERBOLIC FUNCTIONS
    double __bery_math_sinh_int(int32_t value)
    {
        return std::sinh(static_cast<double>(value));
    }

    double __bery_math_sinh_bigint(int64_t value)
    {
        return std::sinh(static_cast<double>(value));
    }

    float __bery_math_sinh_float(float value)
    {
        return std::sinh(value);
    }

    double __bery_math_sinh_double(double value)
    {
        return std::sinh(value);
    }


    double __bery_math_cosh_int(int32_t value)
    {
        return std::cosh(static_cast<double>(value));
    }

    double __bery_math_cosh_bigint(int64_t value)
    {
        return std::cosh(static_cast<double>(value));
    }

    float __bery_math_cosh_float(float value)
    {
        return std::cosh(value);
    }

    double __bery_math_cosh_double(double value)
    {
        return std::cosh(value);
    }


    double __bery_math_tanh_int(int32_t value)
    {
        return std::tanh(static_cast<double>(value));
    }
    double __bery_math_tanh_bigint(int64_t value)
    {
        return std::tanh(static_cast<double>(value));
    }
    float __bery_math_tanh_float(float value)
    {
        return std::tanh(value);
    }
    double __bery_math_tanh_double(double value)
    {
        return std::tanh(value);
    }

    double __bery_math_asinh_int(int32_t value)
    {
        return std::asinh(static_cast<double>(value));
    }
    double __bery_math_asinh_bigint(int64_t value)
    {
        return std::asinh(static_cast<double>(value));
    }
    float __bery_math_asinh_float(float value)
    {
        return std::asinh(value);
    }
    double __bery_math_asinh_double(double value)
    {
        return std::asinh(value);
    }


    double __bery_math_acosh_int(int32_t value)
    {
        return std::acosh(static_cast<double>(value));
    }
    double __bery_math_acosh_bigint(int64_t value)
    {
        return std::acosh(static_cast<double>(value));
    }
    float __bery_math_acosh_float(float value)
    {
        return std::acosh(value);
    }
    double __bery_math_acosh_double(double value)
    {
        return std::acosh(value);
    }

    double __bery_math_atanh_int(int32_t value)
    {
        return std::atanh(static_cast<double>(value));
    }
    double __bery_math_atanh_bigint(int64_t value)
    {
        return std::atanh(static_cast<double>(value));
    }
    float __bery_math_atanh_float(float value)
    {
        return std::atanh(value);
    }
    double __bery_math_atanh_double(double value)
    {
        return std::atanh(value);
    }


    // SPECIAL FUNCTIONS
    double __bery_math_erf_int(int32_t value)
    {
        return std::erf(static_cast<double>(value));
    }
    double __bery_math_erf_bigint(int64_t value)
    {
        return std::erf(static_cast<double>(value));
    }
    float __bery_math_erf_float(float value)
    {
        return std::erf(value);
    }
    double __bery_math_erf_double(double value)
    {
        return std::erf(value);
    }


    double __bery_math_erfc_int(int32_t value)
    {
        return std::erfc(static_cast<double>(value));
    }
    double __bery_math_erfc_bigint(int64_t value)
    {
        return std::erfc(static_cast<double>(value));
    }
    float __bery_math_erfc_float(float value)
    {
        return std::erfc(value);
    }
    double __bery_math_erfc_double(double value)
    {
        return std::erfc(value);
    }


    double __bery_math_lgamma_int(int32_t value)
    {
        return std::lgamma(static_cast<double>(value));
    }
    double __bery_math_lgamma_bigint(int64_t value)
    {
        return std::lgamma(static_cast<double>(value));
    }
    float __bery_math_lgamma_float(float value)
    {
        return std::lgamma(value);
    }
    double __bery_math_lgamma_double(double value)
    {
        return std::lgamma(value);
    }

    double __bery_math_tgamma_int(int32_t value)
    {
        return std::tgamma(static_cast<double>(value));
    }
    double __bery_math_tgamma_bigint(int64_t value)
    {
        return std::tgamma(static_cast<double>(value));
    }
    float __bery_math_tgamma_float(float value)
    {
        return std::tgamma(value);
    }
    double __bery_math_tgamma_double(double value)
    {
        return std::tgamma(value);
    }


    // MATHEMATICAL CONSTANTS
    double __bery_math_pi()
    {
        return 3.14159265358979323846;
    }
    double __bery_math_e()
    {
        return 2.71828182845904523536;
    }
    double __bery_math_tau()
    {
        return 6.28318530717958647692;
    }
    double __bery_math_log2e()
    {
        return 1.44269504088896340736;
    }
    double __bery_math_log10e()
    {
        return 0.43429448190325182765;
    }
    double __bery_math_inv_pi()
    {
        return 0.31830988618379067154;
    }
    double __bery_math_inv_sqrt_pi()
    {
        return 0.56418958354775628695;
    }
    double __bery_math_ln2()
    {
        return 0.69314718055994530942;
    }
    double __bery_math_ln10()
    {
        return 2.30258509299404568402;
    }
    double __bery_math_sqrt2()
    {
        return 1.41421356237309504880;
    }
    double __bery_math_sqrt3()
    {
        return 1.73205080756887729352;
    }
    double __bery_math_inv_sqrt3()
    {
        return 0.57735026918962576451;
    }
    double __bery_math_euler_gamma()
    {
        return 0.57721566490153286060;
    }
    double __bery_math_phi()
    {
        return 1.61803398874989484820;
    }

}