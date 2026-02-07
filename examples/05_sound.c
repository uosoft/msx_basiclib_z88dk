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
    basic_sound(PSG_MIXER, 0xBE);

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

void demo_scale(void) {
    /* PSG frequency dividers for C major scale (C4-C5) */
    static const uint16_t scale[] = {851, 758, 675, 637, 568, 506, 451, 426};
    uint8_t i;

    basic_locate(0, 9);
    basic_print("3. Musical Scale");
    basic_locate(0, 10);
    basic_print("   Playing C major scale...");

    /* Play scale using direct PSG control */
    basic_sound(PSG_MIXER, 0xBE);  /* Tone A only */
    for (i = 0; i < 8; i++) {
        basic_set_tone(0, scale[i]);
        basic_set_volume(0, 12);
        basic_wait_frames(15);
        basic_set_volume(0, 0);
        basic_wait_frames(2);
    }

    basic_sound_off();
    basic_wait_frames(30);

    basic_locate(0, 11);
    basic_print("   Playing melody...");

    /* Twinkle Twinkle melody using PSG */
    basic_sound(PSG_MIXER, 0xBE);
    basic_set_volume(0, 12);

    basic_set_tone(0, 851); basic_wait_frames(10);  /* C */
    basic_set_volume(0, 0); basic_wait_frames(2);
    basic_set_volume(0, 12);
    basic_set_tone(0, 851); basic_wait_frames(10);  /* C */
    basic_set_volume(0, 0); basic_wait_frames(2);
    basic_set_volume(0, 12);
    basic_set_tone(0, 568); basic_wait_frames(10);  /* G */
    basic_set_volume(0, 0); basic_wait_frames(2);
    basic_set_volume(0, 12);
    basic_set_tone(0, 568); basic_wait_frames(10);  /* G */
    basic_set_volume(0, 0); basic_wait_frames(2);
    basic_set_volume(0, 12);
    basic_set_tone(0, 506); basic_wait_frames(10);  /* A */
    basic_set_volume(0, 0); basic_wait_frames(2);
    basic_set_volume(0, 12);
    basic_set_tone(0, 506); basic_wait_frames(10);  /* A */
    basic_set_volume(0, 0); basic_wait_frames(2);
    basic_set_volume(0, 12);
    basic_set_tone(0, 568); basic_wait_frames(20);  /* G (half) */
    basic_set_volume(0, 0); basic_wait_frames(4);

    basic_sound_off();
    basic_wait_frames(30);
}

void demo_harmony(void) {
    basic_locate(0, 13);
    basic_print("4. 3-Channel Harmony");
    basic_locate(0, 14);
    basic_print("   Playing chords...");

    /* Enable tone on all 3 channels */
    basic_sound(PSG_MIXER, 0xB8);

    /* C major chord: C4 + E4 + G4 */
    basic_set_tone(0, 851);   /* C4 */
    basic_set_tone(1, 675);   /* E4 */
    basic_set_tone(2, 568);   /* G4 */
    basic_set_volume(0, 10);
    basic_set_volume(1, 10);
    basic_set_volume(2, 10);
    basic_wait_frames(60);

    /* G major chord: G3 + B3 + D4 */
    basic_set_tone(0, 1136);  /* G3 */
    basic_set_tone(1, 902);   /* B3 */
    basic_set_tone(2, 758);   /* D4 */
    basic_wait_frames(60);

    /* C major chord again */
    basic_set_tone(0, 851);
    basic_set_tone(1, 675);
    basic_set_tone(2, 568);
    basic_wait_frames(60);

    basic_sound_off();
    basic_wait_frames(30);
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
    demo_scale();
    demo_harmony();
    demo_sfx();

    basic_locate(0, 23);
    basic_print("Press any key to exit...");
    basic_wait_key();

    basic_cls();
    basic_print("Sound Demo Complete!");

    while (1);
}
