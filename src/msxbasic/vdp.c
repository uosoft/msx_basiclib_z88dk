/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file vdp.c
 * @brief MSX VDP functions implementation
 */

#include <stdint.h>
#include "../../include/msxbasic/vdp.h"

/* System variables */
#define SCRMOD      0xFCAF  /* Current screen mode */
#define RG0SAV      0xF3DF  /* VDP register 0 shadow */
#define RG8SAV      0xFFE7  /* VDP register 8 shadow (MSX2) */
#define DPPAGE      0xFAF5  /* Display page (MSX2) */
#define ACPAGE      0xFAF6  /* Active page (MSX2) */

#define sys_read8(addr)  (*(volatile uint8_t*)(addr))
#define sys_write8(addr, val) (*(volatile uint8_t*)(addr) = (val))

/* Assembly functions for VDP access */
#asm

PUBLIC _vdp_write_reg
PUBLIC _vdp_read_status
PUBLIC _vdp_wait_cmd
PUBLIC _vdp_cmd_reg

; void vdp_write_reg(uint8_t reg, uint8_t value)
; Stack: [ret][value][reg]
_vdp_write_reg:
    ld hl, 2
    add hl, sp
    ld a, (hl)      ; value
    inc hl
    inc hl
    ld c, (hl)      ; reg
    di
    out (0x99), a   ; Write value first
    ld a, c
    or 0x80         ; Set bit 7 for register write
    out (0x99), a   ; Write register number
    ei
    ret

; uint8_t vdp_read_status(uint8_t reg)
; Stack: [ret][reg]
_vdp_read_status:
    ld hl, 2
    add hl, sp
    ld a, (hl)      ; reg number
    di
    out (0x99), a   ; Select status register
    ld a, 0x8F      ; R#15 = status register select
    out (0x99), a
    in a, (0x99)    ; Read status
    push af
    xor a           ; Reset to status register 0
    out (0x99), a
    ld a, 0x8F
    out (0x99), a
    pop af
    ei
    ld l, a
    ret

; void vdp_wait_cmd(void)
; Wait for VDP command to complete (CE bit in status register 2)
_vdp_wait_cmd:
    di
_wait_loop:
    ld a, 2         ; Status register 2
    out (0x99), a
    ld a, 0x8F
    out (0x99), a
    in a, (0x99)
    and 0x01        ; CE bit (Command Executing)
    jr nz, _wait_loop
    xor a           ; Reset to status register 0
    out (0x99), a
    ld a, 0x8F
    out (0x99), a
    ei
    ret

; void vdp_cmd_reg(uint8_t reg, uint8_t value)
; Write to VDP command register (R#32-R#46)
; Stack: [ret][value][reg]
_vdp_cmd_reg:
    ld hl, 2
    add hl, sp
    ld e, (hl)      ; value
    inc hl
    inc hl
    ld d, (hl)      ; reg
    di
    ld a, e
    out (0x99), a   ; Write value
    ld a, d
    or 0x80         ; Set write flag
    out (0x99), a   ; Write register number
    ei
    ret

#endasm

extern void vdp_cmd_reg(uint8_t reg, uint8_t value);

void vdp_set_write_addr(uint32_t addr) {
    uint8_t low = addr & 0xFF;
    uint8_t mid = (addr >> 8) & 0x3F;
    uint8_t high = (addr >> 14) & 0x07;

    vdp_write_reg(14, high);  /* R#14 VRAM address high bits */

    #asm
        di
        ld hl, 4
        add hl, sp
        ld a, (hl)      ; low byte of addr
        out (0x99), a
        inc hl
        ld a, (hl)      ; mid byte
        and 0x3F
        or 0x40         ; Set write flag
        out (0x99), a
        ei
    #endasm
}

void vdp_set_read_addr(uint32_t addr) {
    uint8_t low = addr & 0xFF;
    uint8_t mid = (addr >> 8) & 0x3F;
    uint8_t high = (addr >> 14) & 0x07;

    vdp_write_reg(14, high);

    #asm
        di
        ld hl, 4
        add hl, sp
        ld a, (hl)      ; low byte
        out (0x99), a
        inc hl
        ld a, (hl)      ; mid byte
        and 0x3F
        out (0x99), a
        ei
    #endasm
}

void vdp_write_vram(uint8_t value) {
    #asm
        ld hl, 2
        add hl, sp
        ld a, (hl)
        out (0x98), a
    #endasm
}

uint8_t vdp_read_vram(void) {
    #asm
        in a, (0x98)
        ld l, a
    #endasm
}

/* PSET using LMMV command inline (same code as vdp_fill but for 1x1 pixel) */
void vdp_pset(uint16_t x, uint16_t y, uint8_t color, uint8_t op) {
    uint8_t dx_low, dx_high, dy_low, dy_high;

    (void)op;

    /* Pre-compute coordinate bytes */
    dx_low = (uint8_t)(x & 0xFF);
    dx_high = (uint8_t)((x >> 8) & 0x01);
    dy_low = (uint8_t)(y & 0xFF);
    dy_high = (uint8_t)((y >> 8) & 0x03);

    vdp_wait_cmd();

    /* DX (R#36, R#37) - pixel coordinate */
    vdp_cmd_reg(36, dx_low);
    vdp_cmd_reg(37, dx_high);

    /* DY (R#38, R#39) */
    vdp_cmd_reg(38, dy_low);
    vdp_cmd_reg(39, dy_high);

    /* NX (R#40, R#41) = 1 pixel */
    vdp_cmd_reg(40, 1);
    vdp_cmd_reg(41, 0);

    /* NY (R#42, R#43) = 1 pixel */
    vdp_cmd_reg(42, 1);
    vdp_cmd_reg(43, 0);

    /* Color (R#44) */
    vdp_cmd_reg(44, color);

    /* ARG (R#45) - direction right-down */
    vdp_cmd_reg(45, 0);

    /* Command (R#46) - LMMV */
    vdp_cmd_reg(46, VDP_CMD_LMMV);
}

void vdp_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color, uint8_t op) {
    int16_t dx, dy;
    uint8_t arg = 0;
    uint8_t dx_low, dx_high, dy_low, dy_high;
    uint8_t nx_low, nx_high, ny_low, ny_high;
    uint16_t nx_val;

    /* Calculate deltas */
    dx = x2 - x1;
    dy = y2 - y1;

    if (dx < 0) {
        dx = -dx;
        arg |= 0x04;  /* DIX = left */
    }
    if (dy < 0) {
        dy = -dy;
        arg |= 0x08;  /* DIY = up */
    }

    /* Determine major axis */
    if (dx < dy) {
        arg |= 0x01;  /* MAJ = Y axis */
        /* Swap dx and dy */
        int16_t tmp = dx;
        dx = dy;
        dy = tmp;
    }

    /* Pre-compute all coordinate bytes */
    dx_low = (uint8_t)(x1 & 0xFF);
    dx_high = (uint8_t)((x1 >> 8) & 0x01);
    dy_low = (uint8_t)(y1 & 0xFF);
    dy_high = (uint8_t)((y1 >> 8) & 0x03);

    nx_val = (uint16_t)(dx + 1);
    nx_low = (uint8_t)(nx_val & 0xFF);
    nx_high = (uint8_t)((nx_val >> 8) & 0x03);
    ny_low = (uint8_t)(dy & 0xFF);
    ny_high = (uint8_t)((dy >> 8) & 0x03);

    vdp_wait_cmd();

    /* DX (R#36, R#37) - start X */
    vdp_cmd_reg(36, dx_low);
    vdp_cmd_reg(37, dx_high);

    /* DY (R#38, R#39) - start Y */
    vdp_cmd_reg(38, dy_low);
    vdp_cmd_reg(39, dy_high);

    /* NX (R#40, R#41) - long side (number of dots) */
    vdp_cmd_reg(40, nx_low);
    vdp_cmd_reg(41, nx_high);

    /* NY (R#42, R#43) - short side */
    vdp_cmd_reg(42, ny_low);
    vdp_cmd_reg(43, ny_high);

    /* Color (R#44) */
    vdp_cmd_reg(44, color);

    /* ARG (R#45) */
    vdp_cmd_reg(45, arg);

    /* Command (R#46) - LINE */
    vdp_cmd_reg(46, VDP_CMD_LINE | op);
}

void vdp_fill(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color) {
    uint8_t dx_low, dx_high, dy_low, dy_high;
    uint8_t nx_low, nx_high, ny_low, ny_high;

    /* Pre-compute all coordinate bytes */
    dx_low = (uint8_t)(x & 0xFF);
    dx_high = (uint8_t)((x >> 8) & 0x01);
    dy_low = (uint8_t)(y & 0xFF);
    dy_high = (uint8_t)((y >> 8) & 0x03);
    nx_low = (uint8_t)(width & 0xFF);
    nx_high = (uint8_t)((width >> 8) & 0x03);
    ny_low = (uint8_t)(height & 0xFF);
    ny_high = (uint8_t)((height >> 8) & 0x03);

    vdp_wait_cmd();

    /* DX (R#36, R#37) - pixel coordinate */
    vdp_cmd_reg(36, dx_low);
    vdp_cmd_reg(37, dx_high);

    /* DY (R#38, R#39) */
    vdp_cmd_reg(38, dy_low);
    vdp_cmd_reg(39, dy_high);

    /* NX (R#40, R#41) - pixel count */
    vdp_cmd_reg(40, nx_low);
    vdp_cmd_reg(41, nx_high);

    /* NY (R#42, R#43) */
    vdp_cmd_reg(42, ny_low);
    vdp_cmd_reg(43, ny_high);

    /* Color (R#44) */
    vdp_cmd_reg(44, color);

    /* ARG (R#45) - direction right-down */
    vdp_cmd_reg(45, 0);

    /* Command (R#46) - LMMV (logical fill with pixel coordinates) */
    vdp_cmd_reg(46, VDP_CMD_LMMV);
}

void vdp_copy(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint16_t width, uint16_t height) {
    uint8_t arg = 0;

    vdp_wait_cmd();

    /* Determine copy direction */
    if (dx > sx) arg |= 0x04;
    if (dy > sy) arg |= 0x08;

    /* SX (R#32, R#33) */
    vdp_cmd_reg(32, sx & 0xFF);
    vdp_cmd_reg(33, (sx >> 8) & 0x01);

    /* SY (R#34, R#35) */
    vdp_cmd_reg(34, sy & 0xFF);
    vdp_cmd_reg(35, (sy >> 8) & 0x03);

    /* DX (R#36, R#37) */
    vdp_cmd_reg(36, dx & 0xFF);
    vdp_cmd_reg(37, (dx >> 8) & 0x01);

    /* DY (R#38, R#39) */
    vdp_cmd_reg(38, dy & 0xFF);
    vdp_cmd_reg(39, (dy >> 8) & 0x03);

    /* NX (R#40, R#41) */
    vdp_cmd_reg(40, width & 0xFF);
    vdp_cmd_reg(41, (width >> 8) & 0x03);

    /* NY (R#42, R#43) */
    vdp_cmd_reg(42, height & 0xFF);
    vdp_cmd_reg(43, (height >> 8) & 0x03);

    /* ARG (R#45) */
    vdp_cmd_reg(45, arg);

    /* Command (R#46) - HMMM */
    vdp_cmd_reg(46, VDP_CMD_HMMM);
}

/* Static variables for palette */
static uint8_t s_pal_idx;
static uint8_t s_pal_rb;
static uint8_t s_pal_g;

void vdp_set_palette(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    /* Calculate and store to static variables */
    s_pal_idx = index;
    s_pal_rb = ((r & 0x07) << 4) | (b & 0x07);
    s_pal_g = g & 0x07;

    /* Use vdp_write_reg to set R#16 - this is known to work */
    vdp_write_reg(16, s_pal_idx);

    /* Write palette data to port 0x9A with interrupts disabled */
    #asm
        di
        ; Small delay after R#16 write
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop

        ; Write RB byte
        ld a, (_s_pal_rb)
        out (0x9A), a

        ; Write G byte
        ld a, (_s_pal_g)
        out (0x9A), a

        ei
    #endasm
}

void vdp_set_display_page(uint8_t page) {
    uint8_t mode = sys_read8(SCRMOD);
    uint8_t r2;

    sys_write8(DPPAGE, page);

    if (mode == 5 || mode == 6) {
        r2 = 0x1F | ((page & 3) << 5);
    } else if (mode == 7 || mode == 8) {
        r2 = 0x1F | ((page & 1) << 5);
    } else {
        r2 = 0x1F;
    }

    vdp_write_reg(2, r2);
}

void vdp_set_active_page(uint8_t page) {
    sys_write8(ACPAGE, page);
}

uint8_t vdp_get_mode(void) {
    return sys_read8(SCRMOD);
}
