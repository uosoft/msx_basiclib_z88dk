/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * 06_input.c - Input Device Demo
 *
 * Demonstrates:
 * - INKEY$ (keyboard input)
 * - STICK (joystick/cursor)
 * - STRIG (trigger buttons)
 * - PEEK/POKE (memory access)
 * - INP/OUT (I/O port access)
 * - Keyboard matrix reading
 *
 * Compatible: MSX1/MSX2/MSX2+/turboR
 */

#include <msxbasic/msxbasic.h>

void demo_inkey(void) {
    char key;
    uint8_t count = 0;

    basic_cls();
    basic_locate(0, 0);
    basic_print("=== INKEY$ Demo ===");
    basic_locate(0, 2);
    basic_print("Press keys (ESC to continue):");
    basic_locate(0, 4);
    basic_print("Key pressed: ");

    while (1) {
        key = basic_inkey();
        if (key) {
            basic_locate(14, 4);
            basic_print("   ");  /* Clear previous */
            basic_locate(14, 4);
            if (key >= 32 && key < 127) {
                basic_print_char(key);
            } else {
                basic_print("0x");
                basic_print_hex8((uint8_t)key);
            }

            basic_locate(0, 5);
            basic_print("ASCII code: ");
            basic_print_int((int16_t)(uint8_t)key);
            basic_print("   ");

            count++;
            basic_locate(0, 6);
            basic_print("Key count: ");
            basic_print_int(count);

            if (key == 27) break;  /* ESC */
        }
        basic_wait_vblank();
    }
}

void demo_stick_strig(void) {
    uint8_t stick, strig;
    const char* dir_names[] = {
        "Center   ", "Up       ", "Up-Right ",
        "Right    ", "Dn-Right ", "Down     ",
        "Dn-Left  ", "Left     ", "Up-Left  "
    };

    basic_cls();
    basic_locate(0, 0);
    basic_print("=== STICK/STRIG Demo ===");
    basic_locate(0, 2);
    basic_print("Move cursor/joystick, press buttons");
    basic_locate(0, 3);
    basic_print("Press SPACE to continue");

    basic_locate(0, 5);
    basic_print("Cursor keys: ");
    basic_locate(0, 6);
    basic_print("Joystick 1:  ");
    basic_locate(0, 7);
    basic_print("Joystick 2:  ");

    basic_locate(0, 9);
    basic_print("Space bar:   ");
    basic_locate(0, 10);
    basic_print("Joy1 Btn A:  ");
    basic_locate(0, 11);
    basic_print("Joy1 Btn B:  ");
    basic_locate(0, 12);
    basic_print("Joy2 Btn A:  ");
    basic_locate(0, 13);
    basic_print("Joy2 Btn B:  ");

    while (1) {
        /* Display STICK values */
        stick = basic_stick(STICK_KEYBOARD);
        basic_locate(14, 5);
        basic_print(dir_names[stick]);

        stick = basic_stick(STICK_JOY1);
        basic_locate(14, 6);
        basic_print(dir_names[stick]);

        stick = basic_stick(STICK_JOY2);
        basic_locate(14, 7);
        basic_print(dir_names[stick]);

        /* Display STRIG values */
        strig = basic_strig_bool(STRIG_SPACE);
        basic_locate(14, 9);
        basic_print(strig ? "PRESSED " : "        ");

        strig = basic_strig_bool(STRIG_JOY1_A);
        basic_locate(14, 10);
        basic_print(strig ? "PRESSED " : "        ");

        strig = basic_strig_bool(STRIG_JOY1_B);
        basic_locate(14, 11);
        basic_print(strig ? "PRESSED " : "        ");

        strig = basic_strig_bool(STRIG_JOY2_A);
        basic_locate(14, 12);
        basic_print(strig ? "PRESSED " : "        ");

        strig = basic_strig_bool(STRIG_JOY2_B);
        basic_locate(14, 13);
        basic_print(strig ? "PRESSED " : "        ");

        /* Exit on space */
        if (basic_strig(STRIG_SPACE)) {
            basic_wait_frames(15);  /* Debounce */
            break;
        }

        basic_wait_vblank();
    }
}

void demo_peek_poke(void) {
    uint8_t original, modified;

    basic_cls();
    basic_locate(0, 0);
    basic_print("=== PEEK/POKE Demo ===");

    /* Read system variables */
    basic_locate(0, 2);
    basic_print("System Variables:");

    basic_locate(0, 4);
    basic_print("Screen mode (SCRMOD): ");
    basic_print_int(basic_peek(0xFCAF));

    basic_locate(0, 5);
    basic_print("Foreground (FORCLR): ");
    basic_print_int(basic_peek(0xF3E9));

    basic_locate(0, 6);
    basic_print("Background (BAKCLR): ");
    basic_print_int(basic_peek(0xF3EA));

    basic_locate(0, 7);
    basic_print("Line length (LINLEN): ");
    basic_print_int(basic_peek(0xF3B0));

    /* Demonstrate POKE (careful!) */
    basic_locate(0, 9);
    basic_print("POKE Demo:");

    original = basic_peek(0xF3E9);  /* Save foreground */
    basic_locate(0, 10);
    basic_print("Original foreground: ");
    basic_print_int(original);

    basic_poke(0xF3E9, 6);  /* Change to dark red */
    modified = basic_peek(0xF3E9);
    basic_locate(0, 11);
    basic_print("Modified foreground: ");
    basic_print_int(modified);

    basic_poke(0xF3E9, original);  /* Restore */
    basic_locate(0, 12);
    basic_print("Restored foreground: ");
    basic_print_int(basic_peek(0xF3E9));

    basic_locate(0, 14);
    basic_print("Press any key...");
    basic_wait_key();
}

void demo_inp_out(void) {
    uint8_t vdp_status;

    basic_cls();
    basic_locate(0, 0);
    basic_print("=== INP/OUT Demo ===");

    basic_locate(0, 2);
    basic_print("Reading VDP Status Register:");

    /* Read VDP status (port 0x99) */
    /* Note: Direct port access requires care */
    vdp_status = basic_vdp_status();

    basic_locate(0, 4);
    basic_print("VDP Status: 0x");
    basic_print_hex8(vdp_status);

    basic_locate(0, 6);
    basic_print("Flags:");
    basic_locate(2, 7);
    basic_print("Sprite collision: ");
    basic_print((vdp_status & 0x20) ? "Yes" : "No");

    basic_locate(2, 8);
    basic_print("5th sprite: ");
    basic_print((vdp_status & 0x40) ? "Yes" : "No");

    basic_locate(2, 9);
    basic_print("VBlank: ");
    basic_print((vdp_status & 0x80) ? "Yes" : "No");

    basic_locate(0, 11);
    basic_print("Press any key...");
    basic_wait_key();
}

void main(void) {
    basic_screen(0);
    basic_color(15, 1, 1);

    demo_inkey();
    demo_stick_strig();
    demo_peek_poke();
    demo_inp_out();

    basic_cls();
    basic_print("Input Demo Complete!");

    while (1);
}
