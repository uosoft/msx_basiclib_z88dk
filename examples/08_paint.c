/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * 08_paint.c - Filled Shapes, PAINT, and GET/PUT Demo
 *
 * Demonstrates:
 * - Filled shapes (circle_fill, boxfill, ellipse_fill)
 * - PAINT command (flood fill)
 * - GET/PUT block operations
 * - Circle arcs and ellipses
 *
 * Note: In SCREEN 2, each 8x1 pixel strip can only have 2 colors
 *       (foreground/background). PAINT with a different fill color
 *       than the border color may not work correctly. Use circle_fill,
 *       boxfill, etc. for reliable filled shapes on MSX1.
 *
 * Compatible: MSX1/MSX2/MSX2+/turboR
 */

#include <msxbasic/msxbasic.h>

/* Buffer for GET/PUT (max 32x32 pixels) */
static uint8_t image_buffer[1100];

void demo_filled_shapes(void) {
    basic_cls();
    basic_init_grp();

    basic_locate(0, 0);
    basic_print("1. Filled Shapes");

    /* Filled circle */
    basic_circle_fill(55, 80, 30, 6);
    basic_circle(55, 80, 30, 6);

    /* Filled rectangle */
    basic_boxfill(105, 55, 165, 105, 4);

    /* Filled ellipse */
    basic_ellipse_fill(215, 80, 25, 35, 3);

    /* Labels */
    basic_locate(3, 15);
    basic_print("Circle");
    basic_locate(13, 15);
    basic_print("Box");
    basic_locate(24, 15);
    basic_print("Ellipse");

    /* Small filled shapes */
    basic_circle_fill(40, 150, 15, 9);
    basic_circle_fill(80, 150, 15, 10);
    basic_circle_fill(120, 150, 15, 11);
    basic_boxfill(145, 138, 175, 162, 7);
    basic_boxfill(185, 138, 215, 162, 8);
    basic_ellipse_fill(245, 150, 10, 20, 5);

    basic_wait_key();
}

void demo_paint_fill(void) {
    basic_cls();
    basic_init_grp();

    basic_locate(0, 0);
    basic_print("2. PAINT Flood Fill");

    /* PAINT works reliably when fill color matches border color.
     * Draw border and fill with same color, then the flood fill
     * correctly detects the boundary. */

    /* Three filled boxes using PAINT */
    basic_box(20, 40, 75, 95, 6);
    basic_paint_c(48, 68, 6);

    basic_box(95, 40, 160, 95, 4);
    basic_paint_c(128, 68, 4);

    basic_box(180, 40, 245, 95, 3);
    basic_paint_c(213, 68, 3);

    /* Nested boxes: outer ring filled, inner empty */
    basic_box(40, 115, 215, 180, 7);
    basic_paint_c(45, 148, 7);
    /* Clear inner area to show ring effect */
    basic_boxfill(65, 130, 190, 165, 0);

    basic_locate(0, 23);
    basic_print("PAINT fills enclosed regions");

    basic_wait_key();
}

void demo_get_put(void) {
    uint16_t size;
    int16_t x;

    basic_cls();
    basic_init_grp();

    basic_locate(0, 0);
    basic_print("3. GET/PUT Block Operations");

    /* Draw a small sprite-like image */
    basic_circle_fill(35, 65, 10, 6);   /* Face */
    basic_circle(35, 65, 12, 15);       /* Head outline */
    basic_pset(30, 62, 15);             /* Left eye */
    basic_pset(40, 62, 15);             /* Right eye */
    basic_line(32, 70, 38, 70, 15);     /* Mouth */

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
    basic_print("4. Circle Arcs and Ellipses");

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

    /* Filled shapes for pie chart effect */
    basic_circle_fill(100, 150, 35, 6);
    basic_circle_fill(100, 150, 20, 4);
    basic_circle_fill(100, 150, 10, 2);
    basic_circle(100, 150, 35, 15);

    /* Concentric filled ellipses */
    basic_ellipse_fill(200, 150, 40, 25, 5);
    basic_ellipse_fill(200, 150, 28, 17, 8);
    basic_ellipse_fill(200, 150, 16, 10, 11);

    basic_wait_key();
}

void main(void) {
    basic_screen(2);
    basic_color(15, 1, 1);

    demo_filled_shapes();
    demo_paint_fill();
    demo_get_put();
    demo_circle_arc();

    basic_screen(1);
    basic_cls();
    basic_print("Graphics Demo Complete!");

    while (1);
}
