/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * 11_system.c - System Functions and VRAM Demo
 *
 * Demonstrates:
 * - MSX type detection
 * - PEEK/POKE
 * - VPEEK/VPOKE
 * - VDP status/register access
 * - TIME (timer)
 * - BASE (VRAM addresses)
 * - VRAM block operations
 * - Slot functions
 * - SCREEN$ (read character)
 *
 * Compatible: MSX1/MSX2/MSX2+/turboR
 */

#include <msxbasic/msxbasic.h>

void demo_msx_detect(void) {
    uint8_t type;

    basic_cls();
    basic_locate(0, 0);
    basic_print("=== MSX Type Detection ===");

    type = basic_get_msx_type();
    basic_locate(0, 2);
    basic_print("MSX type ID: ");
    basic_print_int(type);

    basic_locate(0, 4);
    basic_print("Machine: ");
    if (basic_is_turbo_r()) {
        basic_print("MSX turboR");
    } else if (basic_is_msx2plus()) {
        basic_print("MSX2+");
    } else if (basic_is_msx2()) {
        basic_print("MSX2");
    } else {
        basic_print("MSX1");
    }

    /* Show key system variables */
    basic_locate(0, 6);
    basic_print("System Variables:");

    basic_locate(2, 7);
    basic_print("SCRMOD (mode): ");
    basic_print_int(basic_peek(0xFCAF));

    basic_locate(2, 8);
    basic_print("FORCLR (fg):   ");
    basic_print_int(basic_peek(0xF3E9));

    basic_locate(2, 9);
    basic_print("BAKCLR (bg):   ");
    basic_print_int(basic_peek(0xF3EA));

    basic_locate(2, 10);
    basic_print("BDRCLR (bdr):  ");
    basic_print_int(basic_peek(0xF3EB));

    basic_locate(2, 11);
    basic_print("LINLEN (width): ");
    basic_print_int(basic_peek(0xF3B0));

    basic_locate(2, 12);
    basic_print("CRTCNT (rows): ");
    basic_print_int(basic_peek(0xF3B1));

    /* Free memory estimate */
    basic_locate(0, 14);
    basic_print("FRE(0) = ");
    basic_print_num(basic_fre());
    basic_print(" bytes");

    basic_locate(0, 16);
    basic_print("Press any key...");
    basic_wait_key();
}

void demo_vdp_status(void) {
    uint8_t status;

    basic_cls();
    basic_locate(0, 0);
    basic_print("=== VDP Status Registers ===");

    /* VDP Status Register 0 */
    status = basic_vdp_status();
    basic_locate(0, 2);
    basic_print("S#0: 0x");
    basic_print_hex8(status);
    basic_locate(2, 3);
    basic_print("VBlank:    ");
    basic_print((status & 0x80) ? "Yes" : "No");
    basic_locate(2, 4);
    basic_print("Collision: ");
    basic_print((status & 0x20) ? "Yes" : "No");

    /* MSX2 extended status registers */
    if (basic_is_msx2()) {
        basic_locate(0, 6);
        basic_print("MSX2 Extended Status:");

        status = basic_vdp_status_n(1);
        basic_locate(2, 7);
        basic_print("S#1: 0x");
        basic_print_hex8(status);

        status = basic_vdp_status_n(2);
        basic_locate(2, 8);
        basic_print("S#2: 0x");
        basic_print_hex8(status);
        basic_locate(4, 9);
        basic_print("VDP Cmd: ");
        basic_print((status & 0x01) ? "Busy" : "Ready");
    }

    /* VDP register shadows */
    basic_locate(0, 11);
    basic_print("VDP Register Shadows (R#0-7):");
    {
        uint8_t i;
        for (i = 0; i < 8; i++) {
            basic_locate(2, 12 + i);
            basic_print("R#");
            basic_print_int(i);
            basic_print(": 0x");
            basic_print_hex8(basic_vdp(i));
        }
    }

    basic_locate(0, 21);
    basic_print("Press any key...");
    basic_wait_key();
}

void demo_timer(void) {
    uint32_t start_time, elapsed;
    uint16_t i;

    basic_cls();
    basic_locate(0, 0);
    basic_print("=== TIME / Timer ===");

    /* Show current time */
    basic_locate(0, 2);
    basic_print("Current TIME value: ");
    basic_print_num((uint16_t)(basic_time() & 0xFFFF));

    /* Reset timer */
    basic_time_set(0);
    basic_locate(0, 4);
    basic_print("Timer reset to 0.");

    /* Count 3 seconds (180 ticks at 60Hz, 150 at 50Hz) */
    basic_locate(0, 6);
    basic_print("Counting 180 frames...");
    start_time = basic_time();

    for (i = 0; i < 180; i++) {
        basic_wait_vblank();
        if ((i % 30) == 0) {
            basic_locate(0, 7);
            basic_print("Frame: ");
            basic_print_num(i);
            basic_print("  TIME: ");
            basic_print_num((uint16_t)(basic_time() & 0xFFFF));
            basic_print("   ");
        }
    }

    elapsed = basic_time() - start_time;
    basic_locate(0, 9);
    basic_print("Elapsed ticks: ");
    basic_print_num((uint16_t)(elapsed & 0xFFFF));

    basic_locate(0, 11);
    basic_print("Press any key...");
    basic_wait_key();
}

void demo_vram(void) {
    uint16_t base_addr;
    uint8_t i;

    basic_cls();
    basic_locate(0, 0);
    basic_print("=== VRAM Operations ===");

    /* BASE addresses */
    basic_locate(0, 2);
    basic_print("BASE addresses (SCREEN 1):");

    basic_locate(2, 3);
    basic_print("Name table:    0x");
    basic_print_hex16(basic_base(0));

    basic_locate(2, 4);
    basic_print("Color table:   0x");
    basic_print_hex16(basic_base(1));

    basic_locate(2, 5);
    basic_print("Pattern gen:   0x");
    basic_print_hex16(basic_base(2));

    basic_locate(2, 6);
    basic_print("Sprite attr:   0x");
    basic_print_hex16(basic_base(5));

    basic_locate(2, 7);
    basic_print("Sprite pat:    0x");
    basic_print_hex16(basic_base(6));

    /* VPEEK demo */
    basic_locate(0, 9);
    basic_print("VPEEK - First 16 bytes of VRAM:");
    basic_locate(2, 10);
    for (i = 0; i < 16; i++) {
        basic_print_hex8(basic_vpeek(i));
        basic_print(" ");
    }

    /* VPOKE demo - write pattern to unused area */
    basic_locate(0, 12);
    basic_print("VPOKE - Writing test pattern:");
    base_addr = 0x3F00;  /* Unused area */
    for (i = 0; i < 16; i++) {
        basic_vpoke(base_addr + i, i * 17);
    }

    /* Read back */
    basic_locate(2, 13);
    for (i = 0; i < 16; i++) {
        basic_print_hex8(basic_vpeek(base_addr + i));
        basic_print(" ");
    }

    /* SCREEN$ demo */
    basic_locate(0, 15);
    basic_print("SCREEN$ (read character):");
    basic_locate(0, 16);
    basic_print("ABCDEFGH");

    basic_locate(0, 17);
    basic_print("Reading back: ");
    for (i = 0; i < 8; i++) {
        uint8_t ch = basic_screen_char(i, 16);
        basic_print_char(ch);
    }

    basic_locate(0, 19);
    basic_print("Press any key...");
    basic_wait_key();
}

void demo_vram_block(void) {
    uint8_t src_data[32];
    uint8_t dst_data[32];
    uint8_t i;

    basic_cls();
    basic_locate(0, 0);
    basic_print("=== VRAM Block Operations ===");

    /* Prepare test data */
    for (i = 0; i < 32; i++) {
        src_data[i] = i + 0x40;  /* '@', 'A', 'B', ... */
    }

    /* Write block to VRAM */
    basic_locate(0, 2);
    basic_print("VRAM_WRITE (32 bytes to 0x3E00):");
    basic_vram_write(0x3E00, src_data, 32);

    /* Read back */
    basic_vram_read(dst_data, 0x3E00, 32);

    basic_locate(0, 3);
    basic_print("VRAM_READ back: ");
    for (i = 0; i < 16; i++) {
        basic_print_char(dst_data[i]);
    }

    /* VRAM FILL */
    basic_locate(0, 5);
    basic_print("VRAM_FILL (fill 0x3F00 with 0xAA):");
    basic_vram_fill(0x3F00, 0xAA, 32);

    basic_locate(0, 6);
    basic_print("Read back: ");
    basic_vram_read(dst_data, 0x3F00, 16);
    for (i = 0; i < 16; i++) {
        basic_print_hex8(dst_data[i]);
        basic_print(" ");
    }

    basic_locate(0, 8);
    basic_print("Press any key...");
    basic_wait_key();
}

void main(void) {
    basic_screen(1);
    basic_color(15, 1, 1);

    demo_msx_detect();
    demo_vdp_status();
    demo_timer();
    demo_vram();
    demo_vram_block();

    basic_cls();
    basic_print("System Demo Complete!");

    while (1);
}
