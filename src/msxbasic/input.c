/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file input.c
 * @brief MSX BASIC input functions (Z88DK compatible)
 */

#include <stdint.h>
#include "../../include/msxbasic/input.h"

#define CLIKSW 0xF3DB
#define FNKSTR 0xF87F   /* Function key string area (10 keys * 16 bytes) */

/* External assembly function from system.c */
extern uint8_t sys_snsmat(uint8_t row);

#asm

PUBLIC _inp_inkey
PUBLIC _inp_waitkey
PUBLIC _inp_stick
PUBLIC _inp_strig
PUBLIC _inp_kilbuf
PUBLIC _inp_dspfnk
PUBLIC _inp_erafnk

_inp_inkey:
    call 0x009C     ; CHSNS
    jr z, inkey_none
    call 0x009F     ; CHGET
    ld l, a
    ret
inkey_none:
    ld l, 0
    ret

_inp_waitkey:
    call 0x009F     ; CHGET
    ld l, a
    ret

_inp_stick:
    pop hl
    pop bc
    push bc
    push hl
    ld a, c
    call 0x00D5     ; GTSTCK
    ld l, a
    ret

_inp_strig:
    pop hl
    pop bc
    push bc
    push hl
    ld a, c
    call 0x00D8     ; GTTRIG
    ld l, a
    ret

_inp_kilbuf:
    call 0x0156     ; KILBUF
    ret

_inp_dspfnk:
    call 0x00CF     ; DSPFNK
    ret

_inp_erafnk:
    call 0x00CC     ; ERAFNK
    ret

; uint8_t inp_gtpad(uint8_t pad)
; Read paddle position via GTPAD BIOS
; sccz80 stack: [ret][pad]
; SP+2 = pad number
PUBLIC _inp_gtpad
_inp_gtpad:
    ld hl, 2
    add hl, sp
    ld a, (hl)      ; pad number
    call 0x00DB     ; GTPAD
    ld l, a
    ret

; uint8_t inp_gtpdl(uint8_t paddle)
; Read paddle value via GTPDL BIOS
; sccz80 stack: [ret][paddle]
; SP+2 = paddle number
PUBLIC _inp_gtpdl
_inp_gtpdl:
    ld hl, 2
    add hl, sp
    ld a, (hl)      ; paddle number
    call 0x00DE     ; GTPDL
    ld l, a
    ret

#endasm

extern char inp_inkey(void);
extern char inp_waitkey(void);
extern uint8_t inp_stick(uint8_t dev);
extern int8_t inp_strig(uint8_t btn);
extern void inp_kilbuf(void);
extern void inp_dspfnk(void);
extern void inp_erafnk(void);
extern uint8_t inp_gtpad(uint8_t pad);
extern uint8_t inp_gtpdl(uint8_t paddle);

char basic_inkey(void) {
    return inp_inkey();
}

void basic_input_str(char* buffer, uint8_t n) {
    uint8_t i;
    for (i = 0; i < n; i++) {
        buffer[i] = inp_waitkey();
    }
    buffer[n] = '\0';
}

void basic_input_line(char* buffer, uint8_t max_len) {
    uint8_t count = 0;
    char c;
    while (count < max_len - 1) {
        c = inp_waitkey();
        if (c == 13) break;
        if (c == 8 && count > 0) {
            count--;
            buffer--;
        } else if (c >= 32) {
            *buffer++ = c;
            count++;
        }
    }
    *buffer = '\0';
}

uint8_t basic_stick(uint8_t device) {
    return inp_stick(device);
}

int8_t basic_strig(uint8_t button) {
    return inp_strig(button);
}

uint8_t basic_strig_bool(uint8_t button) {
    return basic_strig(button) ? 1 : 0;
}

uint8_t basic_paddle(uint8_t paddle) {
    /* GTPAD: paddle 1-12
     * Returns paddle position (0-255) */
    if (paddle < 1 || paddle > 12) return 0;
    return inp_gtpad(paddle);
}

uint8_t basic_pdl(uint8_t paddle) {
    /* GTPDL: paddle 1-2
     * Returns analog value (0-255) */
    if (paddle < 1 || paddle > 2) return 0;
    return inp_gtpdl(paddle);
}

uint8_t basic_peek(uint16_t address) {
    return *(volatile uint8_t*)address;
}

void basic_poke(uint16_t address, uint8_t value) {
    *(volatile uint8_t*)address = value;
}

uint16_t basic_peek16(uint16_t address) {
    return *(volatile uint16_t*)address;
}

void basic_poke16(uint16_t address, uint16_t value) {
    *(volatile uint16_t*)address = value;
}

uint8_t basic_inp(uint16_t port) {
    uint8_t val;
    port;  /* In C register from parameter */
    #asm
        pop hl
        pop bc
        push bc
        push hl
        in l, (c)
    #endasm
    return val;
}

void basic_out(uint16_t port, uint8_t value) {
    port; value;
    #asm
        pop hl
        pop bc
        pop de
        push de
        push bc
        push hl
        out (c), e
    #endasm
}

char basic_wait_key(void) {
    return inp_waitkey();
}

uint8_t basic_key_pressed(uint8_t row, uint8_t bit) {
    uint8_t matrix;
    if (row > 10 || bit > 7) return 0;
    matrix = sys_snsmat(row);
    /* SNSMAT returns 0 for pressed, so invert */
    return (matrix & (1 << bit)) ? 0 : 1;
}

void basic_clear_key_buffer(void) {
    inp_kilbuf();
}

void basic_key_click(uint8_t enable) {
    *(volatile uint8_t*)CLIKSW = enable ? 1 : 0;
}

void basic_key_def(uint8_t key_num, const char* str) {
    uint8_t i;
    volatile uint8_t* dest;
    if (key_num < 1 || key_num > 10) return;
    /* Function key strings are 16 bytes each at FNKSTR */
    dest = (volatile uint8_t*)(FNKSTR + (key_num - 1) * 16);
    for (i = 0; i < 15 && str[i]; i++) {
        dest[i] = str[i];
    }
    dest[i] = 0;  /* Null terminate */
}

void basic_key_display(uint8_t show) {
    if (show) inp_dspfnk();
    else inp_erafnk();
}

/* Mouse functions using GTPAD BIOS */
/* GTPAD(n) returns:
 * n=8,12: X offset for port 1,2
 * n=9,13: Y offset for port 1,2
 * n=10,14: Button status for port 1,2
 * n=11,15: Returns 255 if mouse connected
 */
int8_t basic_mouse_x(uint8_t port) {
    uint8_t base;
    if (port > 1) return 0;
    base = (port == 0) ? 8 : 12;
    return (int8_t)inp_gtpad(base);
}

int8_t basic_mouse_y(uint8_t port) {
    uint8_t base;
    if (port > 1) return 0;
    base = (port == 0) ? 9 : 13;
    return (int8_t)inp_gtpad(base);
}

uint8_t basic_mouse_button(uint8_t port) {
    uint8_t base, status;
    if (port > 1) return 0;
    base = (port == 0) ? 10 : 14;
    status = inp_gtpad(base);
    /* Return: bit 0 = left button, bit 1 = right button */
    return status;
}

uint8_t basic_mouse_check(uint8_t port) {
    uint8_t base;
    if (port > 1) return 0;
    base = (port == 0) ? 11 : 15;
    return (inp_gtpad(base) == 255) ? 1 : 0;
}

/* Touchpad functions - GTPAD n=0-3 */
uint8_t basic_touch_x(void) {
    return inp_gtpad(0);
}

uint8_t basic_touch_y(void) {
    return inp_gtpad(1);
}

uint8_t basic_touch_status(void) {
    return inp_gtpad(2);
}

/* Light pen functions - GTPAD n=4-7 */
uint16_t basic_pen_x(void) {
    return inp_gtpad(4);
}

uint16_t basic_pen_y(void) {
    return inp_gtpad(5);
}

uint8_t basic_pen_status(void) {
    return inp_gtpad(6);
}
