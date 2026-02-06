/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * 05_sound.c - Sound and Music Demo
 *
 * Demonstrates:
 * - BEEP
 * - PSG direct control (SOUND)
 * - MML music playback (PLAY)
 * - 3-channel simultaneous music
 * - Sound effects
 *
 * Compatible: MSX1/MSX2/MSX2+/turboR
 */

#include <msxbasic/msxbasic.h>

void demo_beep(void) {
    basic_locate(0, 2);
    basic_print("1. BEEP demo");
    basic_locate(0, 3);
    basic_print("   Playing 3 beeps...");

    basic_beep();
    basic_wait_frames(30);
    basic_beep();
    basic_wait_frames(30);
    basic_beep();

    basic_wait_frames(60);
}

void demo_psg_direct(void) {
    uint16_t freq;
    uint8_t vol;

    basic_locate(0, 5);
    basic_print("2. PSG Direct Control");
    basic_locate(0, 6);
    basic_print("   Frequency sweep...");

    /* Enable tone on channel A */
    basic_sound(PSG_MIXER, 0x3E);

    /* Frequency sweep */
    for (freq = 100; freq < 500; freq += 10) {
        basic_set_tone(0, freq);
        basic_set_volume(0, 12);
        basic_wait_vblank();
        basic_wait_vblank();
    }

    /* Volume fade */
    basic_locate(0, 7);
    basic_print("   Volume fade...");
    basic_set_tone(0, 300);
    for (vol = 15; vol > 0; vol--) {
        basic_set_volume(0, vol);
        basic_wait_frames(5);
    }

    basic_sound_off();
    basic_wait_frames(30);
}

void demo_mml_single(void) {
    basic_locate(0, 9);
    basic_print("3. MML Music (single channel)");
    basic_locate(0, 10);
    basic_print("   Playing scale...");

    /* Play a simple scale */
    basic_play("T120 L4 O4 CDEFGAB O5 C2");

    basic_wait_frames(60);

    basic_locate(0, 11);
    basic_print("   Playing melody...");

    /* Simple melody with note lengths */
    basic_play("T150 O4 L8 CCGGAAG4 FFEEDDC4");

    basic_wait_frames(60);
}

void demo_mml_3channel(void) {
    basic_locate(0, 13);
    basic_print("4. 3-Channel Music");
    basic_locate(0, 14);
    basic_print("   Playing harmony...");

    /* 3-channel harmony */
    basic_play_3ch(
        "T100 O4 L4 CEGC2",     /* Channel A: melody */
        "T100 O3 L4 EGCE2",     /* Channel B: harmony */
        "T100 O2 L2 CG"         /* Channel C: bass */
    );

    basic_wait_frames(60);
}

void demo_sfx(void) {
    basic_locate(0, 16);
    basic_print("5. Sound Effects");

    basic_locate(0, 17);
    basic_print("   Explosion...");
    basic_sfx_explosion();
    basic_wait_frames(30);

    basic_locate(0, 18);
    basic_print("   Laser...");
    basic_sfx_laser();
    basic_wait_frames(30);

    basic_locate(0, 19);
    basic_print("   Jump...");
    basic_sfx_jump();
    basic_wait_frames(30);

    basic_locate(0, 20);
    basic_print("   Coin...");
    basic_sfx_coin();
    basic_wait_frames(30);

    basic_locate(0, 21);
    basic_print("   Hit...");
    basic_sfx_hit();
    basic_wait_frames(30);
}

void main(void) {
    basic_screen(0);
    basic_color(15, 1, 1);
    basic_cls();

    basic_locate(8, 0);
    basic_print("=== Sound & Music Demo ===");

    demo_beep();
    demo_psg_direct();
    demo_mml_single();
    demo_mml_3channel();
    demo_sfx();

    basic_locate(0, 23);
    basic_print("Press any key to exit...");
    basic_wait_key();

    basic_cls();
    basic_print("Sound Demo Complete!");

    while (1);
}
