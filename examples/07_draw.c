/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * 07_draw.c - DRAW Command Demo
 *
 * Demonstrates:
 * - DRAW command string parsing
 * - Movement commands (U/D/L/R/E/F/G/H)
 * - Absolute/Relative move (M)
 * - Pen control (B/N)
 * - Color change (C)
 * - Angle rotation (A)
 * - Scale factor (S)
 *
 * Compatible: MSX1/MSX2/MSX2+/turboR
 */

#include <msxbasic/msxbasic.h>

void demo_basic_movement(void) {
    basic_cls();
    basic_init_grp();

    basic_locate(0, 0);
    basic_print("1. Basic Movement Commands");

    /* Start position */
    basic_grp_move(50, 50);

    /* Draw a square using U/D/L/R */
    basic_draw("C15 R30 D30 L30 U30");

    /* Draw diagonal lines using E/F/G/H */
    basic_grp_move(100, 50);
    basic_draw("C7 E15 F15 G15 H15");

    /* Show direction labels */
    basic_grp_move(150, 30);
    basic_draw("C10 U20");  /* Up */
    basic_grp_move(150, 80);
    basic_draw("C10 D20");  /* Down */
    basic_grp_move(120, 50);
    basic_draw("C10 L20");  /* Left */
    basic_grp_move(180, 50);
    basic_draw("C10 R20");  /* Right */

    basic_wait_key();
}

void demo_pen_control(void) {
    basic_cls();
    basic_init_grp();

    basic_locate(0, 0);
    basic_print("2. Pen Control (B=up, N=no update)");

    basic_grp_move(30, 60);

    /* B = Pen up (move without drawing) */
    basic_draw("C15 R20 B R20 R20 B R20 R20");

    /* N = No position update after draw */
    basic_grp_move(30, 100);
    basic_draw("C6 R40 N U30 N D30 R40");

    /* Dashed line effect using B */
    basic_grp_move(30, 140);
    basic_draw("C9 R5 B R5 R5 B R5 R5 B R5 R5 B R5 R5 B R5 R5");

    basic_wait_key();
}

void demo_absolute_move(void) {
    basic_cls();
    basic_init_grp();

    basic_locate(0, 0);
    basic_print("3. Absolute Move (M)");

    /* Draw lines between absolute coordinates */
    basic_grp_move(10, 50);
    basic_draw("C15 M100,50");          /* Line to (100,50) */
    basic_draw("C7 M100,150");          /* Line to (100,150) */
    basic_draw("C9 M10,150");           /* Line to (10,150) */
    basic_draw("C11 M10,50");           /* Line to (10,50) */

    /* Relative move with M+/M- */
    basic_grp_move(150, 100);
    basic_draw("C15 M+50,0");           /* Relative: right 50 */
    basic_draw("C7 M+0,+50");           /* Relative: down 50 */
    basic_draw("C9 M-50,+0");           /* Relative: left 50 */
    basic_draw("C11 M+0,-50");          /* Relative: up 50 */

    basic_wait_key();
}

void demo_angle_rotation(void) {
    int16_t i;

    basic_cls();
    basic_init_grp();

    basic_locate(0, 0);
    basic_print("4. Angle Rotation (A0-3)");

    /* Draw the same shape at different angles */
    /* A0 = 0 degrees (normal) */
    basic_grp_move(50, 80);
    basic_draw("C15 A0 R20 D20 L20 U20");

    /* A1 = 90 degrees */
    basic_grp_move(100, 80);
    basic_draw("C7 A1 R20 D20 L20 U20");

    /* A2 = 180 degrees */
    basic_grp_move(150, 80);
    basic_draw("C9 A2 R20 D20 L20 U20");

    /* A3 = 270 degrees */
    basic_grp_move(200, 80);
    basic_draw("C11 A3 R20 D20 L20 U20");

    /* Labels */
    basic_locate(5, 14);
    basic_print("A0    A1    A2    A3");

    /* Rotating pattern */
    for (i = 0; i < 4; i++) {
        basic_grp_move(128, 160);
        switch (i) {
            case 0: basic_draw("C15 A0 R30 E10 U10"); break;
            case 1: basic_draw("C7 A1 R30 E10 U10"); break;
            case 2: basic_draw("C9 A2 R30 E10 U10"); break;
            case 3: basic_draw("C11 A3 R30 E10 U10"); break;
        }
    }

    basic_wait_key();
}

void demo_scale(void) {
    basic_cls();
    basic_init_grp();

    basic_locate(0, 0);
    basic_print("5. Scale Factor (S1-255, S4=1:1)");

    /* S2 = 0.5x scale */
    basic_grp_move(30, 80);
    basic_draw("C15 S2 R20 D20 L20 U20");

    /* S4 = 1x scale (default) */
    basic_grp_move(70, 80);
    basic_draw("C7 S4 R20 D20 L20 U20");

    /* S8 = 2x scale */
    basic_grp_move(130, 80);
    basic_draw("C9 S8 R20 D20 L20 U20");

    /* S12 = 3x scale */
    basic_grp_move(60, 140);
    basic_draw("C11 S12 R20 D20 L20 U20");

    basic_locate(2, 23);
    basic_print("S2    S4    S8         S12");

    basic_wait_key();
}

void demo_complex_shape(void) {
    basic_cls();
    basic_init_grp();

    basic_locate(0, 0);
    basic_print("6. Complex Shape - House");

    /* Draw a house */
    basic_grp_move(100, 150);

    /* Walls */
    basic_draw("C15 U60 E30 F30 D60 L60");

    /* Door */
    basic_grp_move(115, 150);
    basic_draw("C6 U30 R20 D30");

    /* Window */
    basic_grp_move(145, 110);
    basic_draw("C5 R15 D15 L15 U15");
    basic_grp_move(152, 110);
    basic_draw("C5 D15");
    basic_grp_move(145, 117);
    basic_draw("C5 R15");

    /* Chimney */
    basic_grp_move(145, 90);
    basic_draw("C8 U20 R10 D10");

    basic_wait_key();
}

void main(void) {
    basic_screen(2);
    basic_color(15, 1, 1);

    demo_basic_movement();
    demo_pen_control();
    demo_absolute_move();
    demo_angle_rotation();
    demo_scale();
    demo_complex_shape();

    basic_screen(1);
    basic_cls();
    basic_print("DRAW Demo Complete!");

    while (1);
}
