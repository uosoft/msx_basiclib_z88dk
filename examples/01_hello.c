/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * 01_hello.c - Basic Screen and Text Demo
 *
 * Demonstrates:
 * - Screen mode setting
 * - Color control
 * - Text output (PRINT, LOCATE, TAB, SPC)
 * - Cursor control
 * - PRINT USING formatted output
 *
 * Compatible: MSX1/MSX2/MSX2+/turboR
 */

#include <msxbasic/msxbasic.h>

void main(void) {
    char buffer[32];
    uint8_t i;

    /* Initialize SCREEN 0 (text 40x24) */
    basic_screen(0);
    basic_width(40);
    basic_color(15, 4, 4);  /* White on dark blue */
    basic_cls();

    /* Title */
    basic_locate(5, 0);
    basic_print("=== MSX BASIC Library Demo ===");

    /* Basic text output */
    basic_locate(0, 2);
    basic_print("Hello, MSX World!");

    /* LOCATE demonstration */
    basic_locate(0, 4);
    basic_print("LOCATE demo:");
    for (i = 0; i < 5; i++) {
        basic_locate(i * 6, 5);
        basic_print_int(i);
    }

    /* TAB demonstration */
    basic_locate(0, 7);
    basic_print("TAB demo:");
    basic_locate(0, 8);
    basic_print("A");
    basic_tab(10);
    basic_print("B");
    basic_tab(20);
    basic_print("C");
    basic_tab(30);
    basic_print("D");

    /* SPC demonstration */
    basic_locate(0, 10);
    basic_print("SPC demo: [");
    basic_spc(5);
    basic_print("][");
    basic_spc(10);
    basic_print("]");

    /* PRINT USING - Integer formatting */
    basic_locate(0, 12);
    basic_print("PRINT USING (int):");
    basic_print_using_int(buffer, "####", 42);
    basic_locate(0, 13);
    basic_print("  Format '####': ");
    basic_print(buffer);

    basic_print_using_int(buffer, "+####", 123);
    basic_locate(0, 14);
    basic_print("  Format '+####': ");
    basic_print(buffer);

    basic_print_using_int(buffer, "##,###", 12345);
    basic_locate(0, 15);
    basic_print("  Format '##,###': ");
    basic_print(buffer);

    /* PRINT USING - Float formatting */
    basic_locate(0, 17);
    basic_print("PRINT USING (float):");
    basic_print_using_float(buffer, "###.##", 3.14159f);
    basic_locate(0, 18);
    basic_print("  Format '###.##': ");
    basic_print(buffer);

    /* PRINT USING - String formatting */
    basic_locate(0, 20);
    basic_print("PRINT USING (str):");
    basic_print_using_str(buffer, "!", "Hello");
    basic_locate(0, 21);
    basic_print("  Format '!': ");
    basic_print(buffer);

    basic_print_using_str(buffer, "&", "MSX");
    basic_locate(0, 22);
    basic_print("  Format '&': ");
    basic_print(buffer);

    /* Wait for key */
    basic_locate(0, 23);
    basic_print("Press any key to exit...");
    basic_wait_key();

    basic_cls();
    basic_print("Done!");

    while (1);
}
