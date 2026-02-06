/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file bmath.c
 * @brief MSX BASIC math functions (Z88DK compatible)
 */

#include <stdint.h>
#include <math.h>
#include "../../include/msxbasic/bmath.h"

static uint32_t rnd_seed = 12345;
static float rnd_last = 0.0f;

int16_t basic_abs(int16_t n) { return (n < 0) ? -n : n; }
int32_t basic_abs_long(int32_t n) { return (n < 0) ? -n : n; }
float basic_abs_float(float n) { return (n < 0.0f) ? -n : n; }

int8_t basic_sgn(int16_t n) {
    if (n > 0) return 1;
    if (n < 0) return -1;
    return 0;
}

int8_t basic_sgn_float(float n) {
    if (n > 0.0f) return 1;
    if (n < 0.0f) return -1;
    return 0;
}

int16_t basic_int(float n) {
    int16_t i = (int16_t)n;
    if (n < 0.0f && n != (float)i) i--;
    return i;
}

int16_t basic_fix(float n) { return (int16_t)n; }

float basic_csng(double n) { return (float)n; }

int16_t basic_cint(float n) {
    if (n >= 0.0f) return (int16_t)(n + 0.5f);
    return (int16_t)(n - 0.5f);
}

double basic_cdbl(float n) { return (double)n; }

float basic_sqr(float n) {
    if (n <= 0.0f) return 0.0f;
    return (float)sqrt((double)n);
}

float basic_sin(float n) { return (float)sin((double)n); }
float basic_cos(float n) { return (float)cos((double)n); }
float basic_tan(float n) { return (float)tan((double)n); }
float basic_atn(float n) { return (float)atan((double)n); }
float basic_log(float n) { return (float)log((double)n); }
float basic_log10(float n) { return (float)log10((double)n); }
float basic_exp(float n) { return (float)exp((double)n); }
float basic_pow(float base, float exponent) { return (float)pow((double)base, (double)exponent); }

float basic_rnd(int16_t n) {
    if (n < 0) rnd_seed = (uint32_t)(-n);
    else if (n == 0) return rnd_last;
    rnd_seed = rnd_seed * 1103515245 + 12345;
    rnd_last = (float)(rnd_seed >> 8) / 16777216.0f;
    return rnd_last;
}

int16_t basic_rnd_range(int16_t min, int16_t max) {
    float r = basic_rnd(1);
    return min + (int16_t)(r * (max - min + 1));
}

void basic_randomize(uint16_t seed) { rnd_seed = seed; }
void basic_randomize_time(void) { rnd_seed = *(volatile uint16_t*)0xFC9E; }

int16_t basic_min(int16_t a, int16_t b) { return (a < b) ? a : b; }
int16_t basic_max(int16_t a, int16_t b) { return (a > b) ? a : b; }

int16_t basic_clamp(int16_t value, int16_t min_val, int16_t max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

float basic_deg_to_rad(float degrees) { return degrees * BASIC_PI / 180.0f; }
float basic_rad_to_deg(float radians) { return radians * 180.0f / BASIC_PI; }
