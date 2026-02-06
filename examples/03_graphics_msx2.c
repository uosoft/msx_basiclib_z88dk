/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * 03_graphics_msx2.c - MSX2 Graphics Demo (SCREEN 5-8)
 *
 * Demonstrates:
 * - SCREEN 5 (256x212, 16 colors)
 * - SCREEN 7 (512x212, 16 colors)
 * - SCREEN 8 (256x212, 256 colors)
 * - VDP palette customization
 * - Page switching
 * - Block COPY operations
 *
 * Requires: MSX2 or later
 */

#include <msxbasic/msxbasic.h>

void demo_screen5(void) {
    int16_t i;

    basic_screen(5);
    basic_wait_vblank();
    basic_wait_vblank();

    /* Set custom palette */
    vdp_set_palette(0, 0, 0, 0);   /* Black */
    vdp_set_palette(1, 7, 0, 0);   /* Red */
    vdp_set_palette(2, 0, 7, 0);   /* Green */
    vdp_set_palette(3, 0, 0, 7);   /* Blue */
    vdp_set_palette(4, 7, 7, 0);   /* Yellow */
    vdp_set_palette(5, 7, 0, 7);   /* Magenta */
    vdp_set_palette(6, 0, 7, 7);   /* Cyan */
    vdp_set_palette(7, 7, 7, 7);   /* White */

    /* Clear screen with color 0 */
    basic_boxfill(0, 0, 255, 211, 0);

    /* Draw color bars */
    for (i = 0; i < 8; i++) {
        basic_boxfill(i * 32, 0, (i + 1) * 32 - 1, 30, i);
    }

    /* Draw various shapes */
    basic_circle(64, 100, 40, 7);
    basic_circle_fill(192, 100, 40, 1);

    basic_ellipse(128, 100, 50, 30, 2);
    basic_ellipse_fill(128, 160, 40, 20, 3);

    /* Draw pattern */
    for (i = 0; i < 16; i++) {
        basic_line(0, 40 + i * 2, 255, 40 + i * 2, i % 8);
    }

    basic_wait_key();
}

void demo_screen7(void) {
    int16_t x;

    basic_screen(7);
    basic_wait_vblank();
    basic_wait_vblank();

    /* 512-pixel wide mode */
    basic_boxfill(0, 0, 511, 211, 0);

    /* Draw across full width */
    basic_line(0, 100, 511, 100, 15);
    basic_line(0, 0, 511, 211, 7);
    basic_line(511, 0, 0, 211, 7);

    /* Color gradient */
    for (x = 0; x < 512; x += 32) {
        basic_boxfill(x, 10, x + 31, 50, (x / 32) % 16);
    }

    /* Circles across full width */
    basic_circle(100, 150, 30, 15);
    basic_circle(256, 150, 30, 15);
    basic_circle(400, 150, 30, 15);

    basic_wait_key();
}

void demo_screen8(void) {
    int16_t x, y;
    uint8_t color;

    basic_screen(8);
    basic_wait_vblank();
    basic_wait_vblank();

    /* 256-color mode: GRB 332 format */
    /* Draw color gradient */
    for (y = 0; y < 212; y++) {
        for (x = 0; x < 256; x++) {
            /* Create gradient color: R in X, G in Y, B fixed */
            color = ((x >> 5) & 0x07) |         /* R: 3 bits */
                    (((y >> 5) & 0x07) << 3) |  /* G: 3 bits */
                    0xC0;                        /* B: 2 bits (max) */
            basic_pset(x, y, color);
        }
    }

    /* Draw white shapes on top */
    basic_circle(128, 106, 50, 0xFF);
    basic_box(50, 50, 200, 160, 0xFF);

    basic_wait_key();
}

void demo_page_copy(void) {
    basic_screen(5);
    basic_wait_vblank();
    basic_wait_vblank();

    /* Draw on page 0 */
    basic_set_page(0, 0);
    basic_boxfill(0, 0, 255, 211, 1);
    basic_circle_fill(128, 106, 50, 7);

    /* Draw on page 1 */
    basic_set_page(0, 1);
    basic_boxfill(0, 0, 255, 211, 4);
    basic_box(50, 50, 200, 160, 15);

    /* Flip between pages */
    {
        uint8_t i;
        for (i = 0; i < 10; i++) {
            basic_set_page(0, 0);
            vdp_set_display_page(0);
            basic_wait_frames(30);

            basic_set_page(1, 1);
            vdp_set_display_page(1);
            basic_wait_frames(30);
        }
    }

    /* Copy from page 1 to page 0 */
    basic_set_page(0, 0);
    basic_copy_page(50, 50, 100, 60, 1, 100, 100, 0);

    basic_wait_key();
}

void main(void) {
    /* Check MSX2 */
    if (!basic_is_msx2()) {
        basic_screen(1);
        basic_print("MSX2 or later required!");
        while (1);
    }

    /* Run demos */
    demo_screen5();
    demo_screen7();
    demo_screen8();
    demo_page_copy();

    basic_screen(1);
    basic_cls();
    basic_print("MSX2 Graphics Demo Complete!");

    while (1);
}
