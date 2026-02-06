/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * 08_paint.c - PAINT (Flood Fill) Demo
 *
 * Demonstrates:
 * - PAINT command (flood fill)
 * - Fill with border color
 * - Complex shape filling
 * - GET/PUT block operations
 *
 * Compatible: MSX1/MSX2/MSX2+/turboR
 */

#include <msxbasic/msxbasic.h>

/* Buffer for GET/PUT (max 32x32 pixels = ~1KB) */
static uint8_t image_buffer[1100];

void demo_basic_paint(void) {
    basic_cls();
    basic_init_grp();

    basic_locate(0, 0);
    basic_print("1. Basic PAINT (Flood Fill)");

    /* Draw closed shapes */
    basic_circle(50, 80, 30, 15);
    basic_box(100, 50, 160, 110, 7);
    basic_circle(200, 80, 25, 9);

    /* Fill them with different colors */
    basic_paint(50, 80, 6, 15);    /* Circle: dark red, border white */
    basic_paint(130, 80, 4, 7);    /* Box: dark blue, border cyan */
    basic_paint(200, 80, 3, 9);    /* Circle: light green, border light red */

    basic_wait_key();
}

void demo_complex_shapes(void) {
    basic_cls();
    basic_init_grp();

    basic_locate(0, 0);
    basic_print("2. Complex Shape Filling");

    /* Draw a star outline */
    basic_grp_move(128, 50);
    basic_draw("C15 F20 L35 E20 D35 H20 R35 G20 U35 E20");

    /* Draw nested rectangles */
    basic_box(30, 100, 90, 160, 15);
    basic_box(40, 110, 80, 150, 15);

    /* Fill areas */
    basic_paint(128, 90, 11, 15);   /* Star: yellow */
    basic_paint(35, 105, 5, 15);    /* Outer rect: light blue */
    basic_paint(50, 130, 8, 15);    /* Inner rect: red */

    /* Draw a donut shape */
    basic_circle(200, 130, 35, 7);
    basic_circle(200, 130, 15, 7);

    /* Fill the ring area */
    basic_paint(200, 100, 13, 7);   /* Ring: magenta */

    basic_wait_key();
}

void demo_pattern_fill(void) {
    int16_t i;

    basic_cls();
    basic_init_grp();

    basic_locate(0, 0);
    basic_print("3. Multiple Regions");

    /* Draw a grid of cells */
    for (i = 0; i < 4; i++) {
        basic_line(50 + i * 40, 50, 50 + i * 40, 170, 15);
        basic_line(50, 50 + i * 30, 210, 50 + i * 30, 15);
    }
    basic_line(210, 50, 210, 170, 15);
    basic_line(50, 170, 210, 170, 15);

    /* Fill cells with different colors */
    basic_paint(70, 65, 2, 15);
    basic_paint(110, 65, 4, 15);
    basic_paint(150, 65, 6, 15);
    basic_paint(190, 65, 8, 15);

    basic_paint(70, 95, 9, 15);
    basic_paint(110, 95, 10, 15);
    basic_paint(150, 95, 11, 15);
    basic_paint(190, 95, 12, 15);

    basic_paint(70, 125, 3, 15);
    basic_paint(110, 125, 5, 15);
    basic_paint(150, 125, 7, 15);
    basic_paint(190, 125, 13, 15);

    basic_paint(70, 155, 14, 15);
    basic_paint(110, 155, 1, 15);
    basic_paint(150, 155, 6, 15);
    basic_paint(190, 155, 9, 15);

    basic_wait_key();
}

void demo_get_put(void) {
    uint16_t size;
    int16_t x;

    basic_cls();
    basic_init_grp();

    basic_locate(0, 0);
    basic_print("4. GET/PUT Block Operations");

    /* Draw a small sprite-like image */
    basic_boxfill(20, 50, 50, 80, 0);  /* Clear area */
    basic_circle(35, 65, 12, 15);      /* Head */
    basic_circle_fill(35, 65, 10, 6);  /* Face fill */
    basic_pset(30, 62, 15);            /* Left eye */
    basic_pset(40, 62, 15);            /* Right eye */
    basic_line(32, 70, 38, 70, 15);    /* Mouth */

    basic_locate(0, 12);
    basic_print("Original image at (20,50)");

    /* Calculate buffer size needed */
    size = basic_get_size(31, 31);
    basic_locate(0, 13);
    basic_print("Buffer size: ");
    basic_print_int(size);
    basic_print(" bytes");

    /* GET the image */
    basic_get(20, 50, 50, 80, image_buffer);

    /* PUT copies at different positions */
    basic_put(70, 50, image_buffer, PUT_PSET);   /* Normal copy */
    basic_put(120, 50, image_buffer, PUT_XOR);   /* XOR mode */

    /* Draw background for XOR demo */
    basic_boxfill(165, 45, 200, 85, 7);
    basic_put(170, 50, image_buffer, PUT_XOR);   /* XOR on background */

    basic_locate(0, 14);
    basic_print("PSET   XOR   XOR+BG");

    /* Animated PUT demo */
    basic_locate(0, 16);
    basic_print("Moving sprite (PUT XOR):");

    for (x = 20; x < 180; x += 4) {
        basic_put(x, 120, image_buffer, PUT_XOR);  /* Draw */
        basic_wait_vblank();
        basic_wait_vblank();
        basic_put(x, 120, image_buffer, PUT_XOR);  /* Erase (XOR again) */
    }

    basic_put(180, 120, image_buffer, PUT_PSET);  /* Final position */

    basic_wait_key();
}

void demo_circle_arc(void) {
    basic_cls();
    basic_init_grp();

    basic_locate(0, 0);
    basic_print("5. Circle Arcs and Ellipses");

    /* Draw arcs (partial circles) */
    basic_circle_ex(60, 80, 30, 15, 0, 90, 100);    /* 0-90 degrees */
    basic_circle_ex(60, 80, 25, 7, 90, 180, 100);   /* 90-180 degrees */
    basic_circle_ex(60, 80, 20, 9, 180, 270, 100);  /* 180-270 degrees */
    basic_circle_ex(60, 80, 15, 11, 270, 360, 100); /* 270-360 degrees */

    /* Draw ellipses with different aspect ratios */
    basic_circle_ex(150, 80, 30, 15, 0, 360, 50);   /* Wide ellipse */
    basic_circle_ex(150, 80, 30, 7, 0, 360, 200);   /* Tall ellipse */

    /* Pac-man style */
    basic_circle_ex(220, 80, 25, 11, 30, 330, 100);
    basic_pset(230, 75, 1);  /* Eye */

    /* Pie chart style */
    basic_grp_move(100, 150);
    basic_circle_ex(100, 150, 35, 15, 0, 360, 100);
    basic_line(100, 150, 135, 150, 15);
    basic_line(100, 150, 100, 115, 15);
    basic_line(100, 150, 75, 180, 15);

    basic_paint(115, 135, 6, 15);
    basic_paint(85, 135, 4, 15);
    basic_paint(100, 165, 2, 15);

    basic_wait_key();
}

void main(void) {
    basic_screen(2);
    basic_color(15, 1, 1);

    demo_basic_paint();
    demo_complex_shapes();
    demo_pattern_fill();
    demo_get_put();
    demo_circle_arc();

    basic_screen(1);
    basic_cls();
    basic_print("PAINT/GET/PUT Demo Complete!");

    while (1);
}
