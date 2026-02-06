/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * 10_math.c - Math Functions Demo
 *
 * Demonstrates:
 * - ABS, SGN, INT, FIX
 * - SQR, SIN, COS, TAN, ATN
 * - LOG, EXP, POW
 * - RND, RANDOMIZE
 * - CINT, CSNG, CDBL
 * - Utility: MIN, MAX, CLAMP
 *
 * Compatible: MSX1/MSX2/MSX2+/turboR
 */

#include <msxbasic/msxbasic.h>

static char buf[32];

/* Helper to print float */
static void print_float(float v) {
    basic_str_float(buf, v);
    basic_print(buf);
}

void demo_basic_math(void) {
    basic_cls();
    basic_locate(0, 0);
    basic_print("=== Basic Math Functions ===");

    /* ABS */
    basic_locate(0, 2);
    basic_print("ABS(-42) = ");
    basic_print_int(basic_abs(-42));

    basic_locate(0, 3);
    basic_print("ABS(-3.14) = ");
    print_float(basic_abs_float(-3.14f));

    /* SGN */
    basic_locate(0, 5);
    basic_print("SGN(-10) = ");
    basic_print_int(basic_sgn(-10));

    basic_locate(0, 6);
    basic_print("SGN(0) = ");
    basic_print_int(basic_sgn(0));

    basic_locate(0, 7);
    basic_print("SGN(10) = ");
    basic_print_int(basic_sgn(10));

    /* INT / FIX */
    basic_locate(0, 9);
    basic_print("INT(3.7) = ");
    basic_print_int(basic_int(3.7f));

    basic_locate(0, 10);
    basic_print("INT(-3.7) = ");
    basic_print_int(basic_int(-3.7f));

    basic_locate(0, 11);
    basic_print("FIX(3.7) = ");
    basic_print_int(basic_fix(3.7f));

    basic_locate(0, 12);
    basic_print("FIX(-3.7) = ");
    basic_print_int(basic_fix(-3.7f));

    /* CINT */
    basic_locate(0, 14);
    basic_print("CINT(3.5) = ");
    basic_print_int(basic_cint(3.5f));

    basic_locate(0, 15);
    basic_print("CINT(4.5) = ");
    basic_print_int(basic_cint(4.5f));

    /* MIN / MAX / CLAMP */
    basic_locate(0, 17);
    basic_print("MIN(10, 20) = ");
    basic_print_int(basic_min(10, 20));

    basic_locate(0, 18);
    basic_print("MAX(10, 20) = ");
    basic_print_int(basic_max(10, 20));

    basic_locate(0, 19);
    basic_print("CLAMP(50, 0, 100) = ");
    basic_print_int(basic_clamp(50, 0, 100));

    basic_locate(0, 20);
    basic_print("CLAMP(-5, 0, 100) = ");
    basic_print_int(basic_clamp(-5, 0, 100));

    basic_locate(0, 22);
    basic_print("Press any key...");
    basic_wait_key();
}

void demo_trig(void) {
    basic_cls();
    basic_locate(0, 0);
    basic_print("=== Trigonometric Functions ===");

    /* SIN */
    basic_locate(0, 2);
    basic_print("SIN(0) = ");
    print_float(basic_sin(0.0f));

    basic_locate(0, 3);
    basic_print("SIN(PI/2) = ");
    print_float(basic_sin(BASIC_PI_2));

    basic_locate(0, 4);
    basic_print("SIN(PI) = ");
    print_float(basic_sin(BASIC_PI));

    /* COS */
    basic_locate(0, 6);
    basic_print("COS(0) = ");
    print_float(basic_cos(0.0f));

    basic_locate(0, 7);
    basic_print("COS(PI/2) = ");
    print_float(basic_cos(BASIC_PI_2));

    basic_locate(0, 8);
    basic_print("COS(PI) = ");
    print_float(basic_cos(BASIC_PI));

    /* TAN */
    basic_locate(0, 10);
    basic_print("TAN(0) = ");
    print_float(basic_tan(0.0f));

    basic_locate(0, 11);
    basic_print("TAN(PI/4) = ");
    print_float(basic_tan(BASIC_PI / 4.0f));

    /* ATN */
    basic_locate(0, 13);
    basic_print("ATN(1) = ");
    print_float(basic_atn(1.0f));

    basic_locate(0, 14);
    basic_print("ATN(1)*4 = ");
    print_float(basic_atn(1.0f) * 4.0f);
    basic_print(" (=PI)");

    /* Degree conversion */
    basic_locate(0, 16);
    basic_print("DEG_TO_RAD(90) = ");
    print_float(basic_deg_to_rad(90.0f));

    basic_locate(0, 17);
    basic_print("RAD_TO_DEG(PI) = ");
    print_float(basic_rad_to_deg(BASIC_PI));

    basic_locate(0, 19);
    basic_print("Press any key...");
    basic_wait_key();
}

void demo_math_funcs(void) {
    basic_cls();
    basic_locate(0, 0);
    basic_print("=== Math Functions ===");

    /* SQR */
    basic_locate(0, 2);
    basic_print("SQR(2) = ");
    print_float(basic_sqr(2.0f));

    basic_locate(0, 3);
    basic_print("SQR(144) = ");
    print_float(basic_sqr(144.0f));

    /* LOG */
    basic_locate(0, 5);
    basic_print("LOG(1) = ");
    print_float(basic_log(1.0f));

    basic_locate(0, 6);
    basic_print("LOG(E) = ");
    print_float(basic_log(BASIC_E));

    basic_locate(0, 7);
    basic_print("LOG10(100) = ");
    print_float(basic_log10(100.0f));

    /* EXP */
    basic_locate(0, 9);
    basic_print("EXP(0) = ");
    print_float(basic_exp(0.0f));

    basic_locate(0, 10);
    basic_print("EXP(1) = ");
    print_float(basic_exp(1.0f));

    /* POW */
    basic_locate(0, 12);
    basic_print("POW(2, 10) = ");
    print_float(basic_pow(2.0f, 10.0f));

    basic_locate(0, 13);
    basic_print("POW(3, 3) = ");
    print_float(basic_pow(3.0f, 3.0f));

    basic_locate(0, 15);
    basic_print("Press any key...");
    basic_wait_key();
}

void demo_random(void) {
    uint8_t i;

    basic_cls();
    basic_locate(0, 0);
    basic_print("=== Random Numbers ===");

    /* Seed from timer */
    basic_randomize_time();

    /* RND (0 to 1) */
    basic_locate(0, 2);
    basic_print("RND(1) - 10 random floats:");
    for (i = 0; i < 10; i++) {
        basic_locate(0, 3 + i);
        basic_print("  ");
        basic_print_int(i);
        basic_print(": ");
        print_float(basic_rnd(1));
    }

    /* Random integers in range */
    basic_locate(0, 14);
    basic_print("Random 1-6 (dice): ");
    for (i = 0; i < 10; i++) {
        basic_print_int(basic_rnd_range(1, 6));
        basic_print(" ");
    }

    /* Random with fixed seed */
    basic_locate(0, 16);
    basic_print("RANDOMIZE 42 (reproducible):");
    basic_randomize(42);
    basic_locate(0, 17);
    basic_print("  ");
    for (i = 0; i < 8; i++) {
        print_float(basic_rnd(1));
        basic_print(" ");
    }

    /* Same seed again */
    basic_randomize(42);
    basic_locate(0, 18);
    basic_print("  ");
    for (i = 0; i < 8; i++) {
        print_float(basic_rnd(1));
        basic_print(" ");
    }

    basic_locate(0, 20);
    basic_print("(Both lines should be identical)");

    basic_locate(0, 22);
    basic_print("Press any key...");
    basic_wait_key();
}

void main(void) {
    basic_screen(0);
    basic_color(15, 1, 1);

    demo_basic_math();
    demo_trig();
    demo_math_funcs();
    demo_random();

    basic_cls();
    basic_print("Math Demo Complete!");

    while (1);
}
