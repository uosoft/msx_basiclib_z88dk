/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file bmath.h
 * @brief MSX BASIC math functions
 *
 * Provides functions equivalent to MSX BASIC's:
 * ABS, SGN, INT, FIX, SQR, SIN, COS, TAN, ATN, LOG, EXP, RND
 *
 * Note: Named bmath.h to avoid conflict with standard math.h
 */

#ifndef MSXBASIC_MATH_H
#define MSXBASIC_MATH_H

#include <stdint.h>

/* Math constants */
#define BASIC_PI        3.14159265358979f
#define BASIC_PI_2      1.57079632679490f   /* PI/2 */
#define BASIC_2PI       6.28318530717959f   /* 2*PI */
#define BASIC_E         2.71828182845905f

/**
 * @brief Get absolute value of integer
 * Equivalent to: ABS(n)
 * @param n Input value
 * @return Absolute value
 */
int16_t basic_abs(int16_t n);

/**
 * @brief Get absolute value of long integer
 * @param n Input value
 * @return Absolute value
 */
int32_t basic_abs_long(int32_t n);

/**
 * @brief Get absolute value of float
 * @param n Input value
 * @return Absolute value
 */
float basic_abs_float(float n);

/**
 * @brief Get sign of number
 * Equivalent to: SGN(n)
 * @param n Input value
 * @return -1, 0, or 1
 */
int8_t basic_sgn(int16_t n);

/**
 * @brief Get sign of float
 * @param n Input value
 * @return -1, 0, or 1
 */
int8_t basic_sgn_float(float n);

/**
 * @brief Convert to integer (floor)
 * Equivalent to: INT(n)
 * Returns largest integer not greater than n
 * @param n Input value
 * @return Integer portion (floor)
 */
int16_t basic_int(float n);

/**
 * @brief Convert to integer (truncate toward zero)
 * Equivalent to: FIX(n)
 * @param n Input value
 * @return Integer portion (truncated)
 */
int16_t basic_fix(float n);

/**
 * @brief Convert to single precision
 * Equivalent to: CSNG(n)
 * @param n Input value (double)
 * @return Single precision value
 */
float basic_csng(double n);

/**
 * @brief Convert to integer
 * Equivalent to: CINT(n)
 * @param n Input value
 * @return Integer value (rounded)
 */
int16_t basic_cint(float n);

/**
 * @brief Convert to double precision
 * Equivalent to: CDBL(n)
 * @param n Input value (single)
 * @return Double precision value
 */
double basic_cdbl(float n);

/**
 * @brief Calculate square root
 * Equivalent to: SQR(n)
 * @param n Input value (must be >= 0)
 * @return Square root
 */
float basic_sqr(float n);

/**
 * @brief Calculate sine
 * Equivalent to: SIN(n)
 * @param n Angle in radians
 * @return Sine value
 */
float basic_sin(float n);

/**
 * @brief Calculate cosine
 * Equivalent to: COS(n)
 * @param n Angle in radians
 * @return Cosine value
 */
float basic_cos(float n);

/**
 * @brief Calculate tangent
 * Equivalent to: TAN(n)
 * @param n Angle in radians
 * @return Tangent value
 */
float basic_tan(float n);

/**
 * @brief Calculate arctangent
 * Equivalent to: ATN(n)
 * @param n Input value
 * @return Arctangent in radians
 */
float basic_atn(float n);

/**
 * @brief Calculate natural logarithm
 * Equivalent to: LOG(n)
 * @param n Input value (must be > 0)
 * @return Natural logarithm
 */
float basic_log(float n);

/**
 * @brief Calculate base-10 logarithm
 * @param n Input value (must be > 0)
 * @return Base-10 logarithm
 */
float basic_log10(float n);

/**
 * @brief Calculate exponential (e^n)
 * Equivalent to: EXP(n)
 * @param n Exponent
 * @return e raised to power n
 */
float basic_exp(float n);

/**
 * @brief Calculate power
 * @param base Base value
 * @param exponent Exponent
 * @return base raised to power exponent
 */
float basic_pow(float base, float exponent);

/**
 * @brief Generate random number
 * Equivalent to: RND(n)
 * @param n Seed control:
 *          n > 0: return next random number (0 to 1)
 *          n = 0: return last random number
 *          n < 0: seed with n, return first number
 * @return Random number between 0 and 1
 */
float basic_rnd(int16_t n);

/**
 * @brief Generate random integer in range
 * @param min Minimum value (inclusive)
 * @param max Maximum value (inclusive)
 * @return Random integer in range
 */
int16_t basic_rnd_range(int16_t min, int16_t max);

/**
 * @brief Set random seed
 * Equivalent to: RANDOMIZE seed
 * @param seed Random seed value
 */
void basic_randomize(uint16_t seed);

/**
 * @brief Set random seed from timer
 * Equivalent to: RANDOMIZE TIME
 */
void basic_randomize_time(void);

/**
 * @brief Get minimum of two integers
 * @param a First value
 * @param b Second value
 * @return Smaller value
 */
int16_t basic_min(int16_t a, int16_t b);

/**
 * @brief Get maximum of two integers
 * @param a First value
 * @param b Second value
 * @return Larger value
 */
int16_t basic_max(int16_t a, int16_t b);

/**
 * @brief Clamp value to range
 * @param value Value to clamp
 * @param min_val Minimum
 * @param max_val Maximum
 * @return Clamped value
 */
int16_t basic_clamp(int16_t value, int16_t min_val, int16_t max_val);

/**
 * @brief Convert degrees to radians
 * @param degrees Angle in degrees
 * @return Angle in radians
 */
float basic_deg_to_rad(float degrees);

/**
 * @brief Convert radians to degrees
 * @param radians Angle in radians
 * @return Angle in degrees
 */
float basic_rad_to_deg(float radians);

#endif /* MSXBASIC_MATH_H */
