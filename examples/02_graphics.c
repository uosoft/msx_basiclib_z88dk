/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * 02_graphics.c - MSX1 Graphics Demo (SCREEN 2)
 *
 * Demonstrates:
 * - SCREEN 2 (256x192, 16 colors)
 * - PSET, LINE, BOX, BOXFILL
 * - CIRCLE, ELLIPSE
 * - Filled shapes
 *
 * Compatible: MSX1/MSX2/MSX2+/turboR
 */

#include <msxbasic/msxbasic.h>

void main(void) {
    int16_t i;

    /* Initialize SCREEN 2 (256x192 graphics) */
    basic_screen(2);
    basic_color(15, 1, 1);  /* White on black */
    basic_init_grp();       /* Initialize graphics color table */
    basic_cls();

    /* PSET - Draw individual pixels */
    for (i = 0; i < 50; i++) {
        basic_pset(10 + i, 10, 15);      /* White line */
        basic_pset(10 + i, 12, 2);       /* Green line */
        basic_pset(10 + i, 14, 8);       /* Red line */
    }

    /* LINE - Draw lines */
    basic_line(10, 20, 100, 40, 6);      /* Dark red diagonal */
    basic_line(100, 20, 10, 40, 7);      /* Cyan diagonal */
    basic_line(10, 50, 100, 50, 10);     /* Yellow horizontal */

    /* BOX - Draw rectangle outlines */
    basic_box(120, 10, 180, 50, 5);      /* Light blue box */
    basic_box(125, 15, 175, 45, 9);      /* Light red inner box */

    /* BOXFILL - Draw filled rectangles */
    basic_boxfill(190, 10, 250, 30, 3);  /* Light green filled */
    basic_boxfill(190, 35, 250, 55, 13); /* Magenta filled */

    /* CIRCLE - Draw circles */
    basic_circle(50, 100, 30, 15);       /* White circle */
    basic_circle(50, 100, 20, 11);       /* Yellow circle */
    basic_circle(50, 100, 10, 8);        /* Red circle */

    /* CIRCLE_FILL - Draw filled circles */
    basic_circle_fill(130, 100, 25, 4);  /* Dark blue filled */
    basic_circle_fill(130, 100, 15, 5);  /* Light blue filled */

    /* ELLIPSE - Draw ellipses */
    basic_ellipse(210, 100, 35, 20, 14); /* Gray wide ellipse */
    basic_ellipse(210, 100, 20, 35, 12); /* Dark green tall ellipse */

    /* ELLIPSE_FILL - Draw filled ellipse */
    basic_ellipse_fill(50, 160, 40, 25, 6);  /* Dark red filled */
    basic_ellipse_fill(50, 160, 25, 15, 9);  /* Light red inner */

    /* Decorative pattern */
    for (i = 0; i < 8; i++) {
        basic_circle(180, 160, 10 + i * 4, (i % 15) + 1);
    }

    /* Wait for key */
    basic_wait_key();

    basic_screen(1);
    basic_cls();
    basic_print("Graphics Demo Complete!");

    while (1);
}
