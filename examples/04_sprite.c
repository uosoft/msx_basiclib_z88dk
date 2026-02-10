/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * 04_sprite.c - Sprite Demo
 *
 * Demonstrates:
 * - Sprite pattern definition
 * - Sprite display (PUT SPRITE)
 * - Sprite animation
 * - Sprite collision detection
 * - Multiple sprites
 *
 * Compatible: MSX1/MSX2/MSX2+/turboR
 */

#include <msxbasic/msxbasic.h>

/* 16x16 sprite patterns */
static const uint8_t sprite_ship[32] = {
    0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F,
    0x7F, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03,
    0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC,
    0xFE, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0
};

static const uint8_t sprite_ball[32] = {
    0x07, 0x1F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0x1F, 0x07,
    0xE0, 0xF8, 0xFC, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFC, 0xF8, 0xE0
};

static const uint8_t sprite_star[32] = {
    0x01, 0x01, 0x03, 0x03, 0x1F, 0x3F, 0x0F, 0x0F,
    0x1F, 0x3B, 0x71, 0x61, 0x41, 0x00, 0x00, 0x00,
    0x80, 0x80, 0xC0, 0xC0, 0xF8, 0xFC, 0xF0, 0xF0,
    0xF8, 0xDC, 0x8E, 0x86, 0x82, 0x00, 0x00, 0x00
};

void main(void) {
    int16_t ship_x = 100, ship_y = 80;
    int16_t ball_x[4], ball_y[4];
    int16_t ball_dx[4], ball_dy[4];
    uint8_t i;
    uint8_t stick_dir;
    uint8_t collision;
    uint8_t beep_cooldown = 0;

    /* Initialize SCREEN 2 for MSX1 compatibility */
    basic_screen(2);
    basic_color(1, 1, 1);
    basic_init_grp();
    basic_cls();

    /* Set 16x16 sprite mode */
    basic_sprite_size(1);

    /* Define sprite patterns */
    basic_sprite_pattern(0, sprite_ship);
    basic_sprite_pattern(1, sprite_ball);
    basic_sprite_pattern(2, sprite_star);

    /* Initialize balls */
    for (i = 0; i < 4; i++) {
        ball_x[i] = 30 + i * 50;
        ball_y[i] = 30 + i * 30;
        ball_dx[i] = (i % 2) ? 2 : -2;
        ball_dy[i] = (i < 2) ? 1 : -1;
    }

    /* Draw background */
    basic_color_fg(15);  /* White text on black background */
    basic_print("Move with cursor keys. Press SPACE to exit.");
    basic_color_fg(1);   /* Restore foreground */
    basic_line(0, 20, 255, 20, 15);
    basic_box(0, 20, 255, 191, 7);

    /* Main game loop */
    while (1) {
        /* Read joystick/cursor */
        stick_dir = basic_stick(0);

        /* Move ship based on direction */
        switch (stick_dir) {
            case STICK_UP:         ship_y -= 3; break;
            case STICK_UP_RIGHT:   ship_x += 3; ship_y -= 3; break;
            case STICK_RIGHT:      ship_x += 3; break;
            case STICK_DOWN_RIGHT: ship_x += 3; ship_y += 3; break;
            case STICK_DOWN:       ship_y += 3; break;
            case STICK_DOWN_LEFT:  ship_x -= 3; ship_y += 3; break;
            case STICK_LEFT:       ship_x -= 3; break;
            case STICK_UP_LEFT:    ship_x -= 3; ship_y -= 3; break;
        }

        /* Keep ship on screen */
        if (ship_x < 0) ship_x = 0;
        if (ship_x > 239) ship_x = 239;
        if (ship_y < 20) ship_y = 20;
        if (ship_y > 175) ship_y = 175;

        /* Update balls */
        for (i = 0; i < 4; i++) {
            ball_x[i] += ball_dx[i];
            ball_y[i] += ball_dy[i];

            /* Bounce off walls */
            if (ball_x[i] < 0 || ball_x[i] > 239) {
                ball_dx[i] = -ball_dx[i];
                ball_x[i] += ball_dx[i];
            }
            if (ball_y[i] < 20 || ball_y[i] > 175) {
                ball_dy[i] = -ball_dy[i];
                ball_y[i] += ball_dy[i];
            }
        }

        /* Display sprites */
        basic_put_sprite(0, ship_x, ship_y, 15, 0);  /* Ship - white */
        for (i = 0; i < 4; i++) {
            basic_put_sprite(1 + i, ball_x[i], ball_y[i],
                           9 + (i % 4), (i < 2) ? 1 : 2);  /* Balls + Stars */
        }

        /* Check collision (with cooldown to avoid blocking beep loop) */
        collision = basic_sprite_collision();
        if (collision && beep_cooldown == 0) {
            basic_beep();
            beep_cooldown = 120;  /* ~2 sec cooldown at 60Hz */
        }
        if (beep_cooldown > 0) beep_cooldown--;

        /* Check for exit */
        if (basic_strig(STRIG_SPACE)) {
            break;
        }

        /* Wait for vsync */
        basic_wait_vblank();
    }

    /* Cleanup */
    basic_sprites_off();
    basic_screen(1);
    basic_color(15, 1, 1);  /* White on black */
    basic_cls();
    basic_print("Sprite Demo Complete!");

    while (1);
}
