/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * 09_string.c - String Functions Demo
 *
 * Demonstrates:
 * - LEN, LEFT$, RIGHT$, MID$
 * - ASC, CHR$, STR$, VAL
 * - HEX$, BIN$, OCT$
 * - INSTR, STRING$, SPACE$
 * - MID$ replacement
 * - UCASE, LCASE
 * - JIS/SJIS conversion
 *
 * Compatible: MSX1/MSX2/MSX2+/turboR
 */

#include <msxbasic/msxbasic.h>

static char buf[64];

void demo_basic_string(void) {
    const char* test = "Hello, MSX!";

    basic_cls();
    basic_locate(0, 0);
    basic_print("=== Basic String Functions ===");

    /* LEN */
    basic_locate(0, 2);
    basic_print("LEN(\"Hello, MSX!\") = ");
    basic_print_int(basic_len(test));

    /* LEFT$ */
    basic_left(buf, test, 5);
    basic_locate(0, 4);
    basic_print("LEFT$(s$, 5) = \"");
    basic_print(buf);
    basic_print("\"");

    /* RIGHT$ */
    basic_right(buf, test, 4);
    basic_locate(0, 5);
    basic_print("RIGHT$(s$, 4) = \"");
    basic_print(buf);
    basic_print("\"");

    /* MID$ */
    basic_mid(buf, test, 8, 3);
    basic_locate(0, 6);
    basic_print("MID$(s$, 8, 3) = \"");
    basic_print(buf);
    basic_print("\"");

    /* ASC */
    basic_locate(0, 8);
    basic_print("ASC(\"A\") = ");
    basic_print_int(basic_asc("A"));

    /* CHR$ */
    basic_chr(buf, 65);
    basic_locate(0, 9);
    basic_print("CHR$(65) = \"");
    basic_print(buf);
    basic_print("\"");

    /* STRING$ */
    basic_string(buf, 10, '*');
    basic_locate(0, 11);
    basic_print("STRING$(10, \"*\") = \"");
    basic_print(buf);
    basic_print("\"");

    /* SPACE$ */
    basic_space(buf, 5);
    basic_locate(0, 12);
    basic_print("SPACE$(5) = \"[");
    basic_print(buf);
    basic_print("]\"");

    /* INSTR */
    basic_locate(0, 14);
    basic_print("INSTR(\"Hello, MSX!\", \"MSX\") = ");
    basic_print_int(basic_instr("Hello, MSX!", "MSX", 1));

    basic_locate(0, 16);
    basic_print("Press any key...");
    basic_wait_key();
}

void demo_conversion(void) {
    basic_cls();
    basic_locate(0, 0);
    basic_print("=== Conversion Functions ===");

    /* STR$ (integer) */
    basic_str(buf, -1234);
    basic_locate(0, 2);
    basic_print("STR$(-1234) = \"");
    basic_print(buf);
    basic_print("\"");

    /* STR$ (long) */
    basic_str_long(buf, 100000L);
    basic_locate(0, 3);
    basic_print("STR$(100000) = \"");
    basic_print(buf);
    basic_print("\"");

    /* STR$ (float) */
    basic_str_float(buf, 3.14159f);
    basic_locate(0, 4);
    basic_print("STR$(3.14159) = \"");
    basic_print(buf);
    basic_print("\"");

    /* VAL */
    basic_locate(0, 6);
    basic_print("VAL(\"42\") = ");
    basic_print_int(basic_val("42"));

    /* VAL (float with decimal) */
    basic_locate(0, 7);
    basic_print("VAL(\"3.14\") = ");
    {
        float v = basic_val_float("3.14");
        basic_str_float(buf, v);
        basic_print(buf);
    }

    /* VAL (scientific notation) */
    basic_locate(0, 8);
    basic_print("VAL(\"1.5E3\") = ");
    {
        float v = basic_val_float("1.5E3");
        basic_str_float(buf, v);
        basic_print(buf);
    }

    /* HEX$ */
    basic_hex(buf, 255);
    basic_locate(0, 10);
    basic_print("HEX$(255) = \"");
    basic_print(buf);
    basic_print("\"");

    /* BIN$ */
    basic_bin(buf, 170);
    basic_locate(0, 11);
    basic_print("BIN$(170) = \"");
    basic_print(buf);
    basic_print("\"");

    /* OCT$ */
    basic_oct(buf, 255);
    basic_locate(0, 12);
    basic_print("OCT$(255) = \"");
    basic_print(buf);
    basic_print("\"");

    basic_locate(0, 14);
    basic_print("Press any key...");
    basic_wait_key();
}

void demo_manipulation(void) {
    char work[32];

    basic_cls();
    basic_locate(0, 0);
    basic_print("=== String Manipulation ===");

    /* CONCAT */
    basic_concat(buf, "Hello", " World");
    basic_locate(0, 2);
    basic_print("\"Hello\" + \" World\" = \"");
    basic_print(buf);
    basic_print("\"");

    /* MID$ replacement */
    basic_locate(0, 4);
    basic_print("MID$ replacement:");
    {
        uint8_t i;
        for (i = 0; i < 12; i++) work[i] = "Hello World"[i];
        work[11] = '\0';
    }
    basic_locate(0, 5);
    basic_print("  Before: \"");
    basic_print(work);
    basic_print("\"");

    basic_mid_replace(work, 7, 5, "MSX!!");
    basic_locate(0, 6);
    basic_print("  After:  \"");
    basic_print(work);
    basic_print("\"");

    /* UCASE / LCASE */
    {
        uint8_t i;
        for (i = 0; i < 10; i++) work[i] = "Hello MSX!"[i];
        work[9] = '\0';
    }

    basic_locate(0, 8);
    basic_print("UCASE / LCASE:");

    basic_ucase(work);
    basic_locate(0, 9);
    basic_print("  UCASE: \"");
    basic_print(work);
    basic_print("\"");

    basic_lcase(work);
    basic_locate(0, 10);
    basic_print("  LCASE: \"");
    basic_print(work);
    basic_print("\"");

    /* STRCMP */
    basic_locate(0, 12);
    basic_print("String comparison:");

    basic_locate(0, 13);
    basic_print("  \"ABC\" vs \"ABC\" = ");
    basic_print_int(basic_strcmp("ABC", "ABC"));
    basic_print(" (0=equal)");

    basic_locate(0, 14);
    basic_print("  \"ABC\" vs \"DEF\" = ");
    basic_print_int(basic_strcmp("ABC", "DEF"));

    basic_locate(0, 15);
    basic_print("  \"DEF\" vs \"ABC\" = ");
    basic_print_int(basic_strcmp("DEF", "ABC"));

    basic_locate(0, 17);
    basic_print("Press any key...");
    basic_wait_key();
}

void main(void) {
    basic_screen(0);
    basic_color(15, 1, 1);

    demo_basic_string();
    demo_conversion();
    demo_manipulation();

    basic_cls();
    basic_print("String Demo Complete!");

    while (1);
}
