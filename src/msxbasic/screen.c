/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file screen.c
 * @brief MSX BASIC screen control functions implementation
 *
 * Z88DK compatible version using BIOS calls
 */

#include <stdint.h>
#include <msx.h>
#include "../../include/msxbasic/screen.h"

/* MSX System Variables */
#define LINL40      0xF3AE
#define LINL32      0xF3AF
#define LINLEN      0xF3B0
#define CRTCNT      0xF3B1
#define CSRY        0xF3DC
#define CSRX        0xF3DD
#define FORCLR      0xF3E9
#define BAKCLR      0xF3EA
#define BDRCLR      0xF3EB
#define SCRMOD      0xFCAF
#define CSRSW       0xFCA9
#define GRPACX      0xFCB7  /* Graphics accumulator X (2 bytes) */
#define GRPACY      0xFCB9  /* Graphics accumulator Y (2 bytes) */
#define ATRBYT      0xF3F2  /* Attribute byte for graphics */

/* VRAM table address system variables */
#define TXTNAM      0xF3B3  /* SCREEN 0 name table (2 bytes) */
#define T32NAM      0xF3BD  /* SCREEN 1 name table (2 bytes) */
#define GRPNAM      0xF3C7  /* SCREEN 2 name table (2 bytes) */
#define GRPCGP      0xF3CB  /* SCREEN 2 pattern generator (2 bytes) */

/* Helper functions */
#define sys_write8(addr, val) (*(volatile uint8_t*)(addr) = (val))
#define sys_read8(addr)       (*(volatile uint8_t*)(addr))
#define sys_read16(addr)      (*(volatile uint16_t*)(addr))
#define sys_write16(addr, val) (*(volatile uint16_t*)(addr) = (val))

/* Initialize BIOS trampoline (defined in system.c) */
extern void basic_init(void);

/* VRAM fill (defined in system.c) */
extern void sys_filvrm(uint16_t addr, uint16_t count, uint8_t value);

/* VDP fill for MSX2+ bitmap modes (defined in vdp.c) */
extern void vdp_fill(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color);

/* BIOS call wrapper using Z88DK */
extern void msx_bios_cls(void);
extern void msx_bios_posit(uint8_t x, uint8_t y);
extern void msx_bios_chput(uint8_t c);
extern void msx_bios_chgmod(uint8_t mode);
extern void msx_bios_chgclr(void);
extern void msx_bios_grpprt(uint8_t c);

#asm

PUBLIC _msx_bios_cls
PUBLIC _msx_bios_posit
PUBLIC _msx_bios_chput
PUBLIC _msx_bios_chgmod
PUBLIC _msx_bios_chgclr
PUBLIC _msx_bios_grpprt

_msx_bios_cls:
    xor a               ; Set Z flag (BIOS CLS requires Z=1)
    ld hl, 0x00C3
    ld (0xC02C), hl
    call 0xC000
    ret

_msx_bios_posit:
    pop de          ; return address
    pop hl          ; L = x (first param)
    pop bc          ; C = y (second param)
    push bc
    push hl
    push de
    ld h, l         ; H = x
    ld l, c         ; L = y
    push hl
    ld hl, 0x00C6
    ld (0xC02C), hl
    pop hl
    call 0xC000     ; POSIT
    ret

_msx_bios_chput:
    pop hl
    pop bc
    push bc
    push hl
    ld a, c
    ld hl, 0x00A2
    ld (0xC02C), hl
    call 0xC000     ; CHPUT
    ret

_msx_bios_chgmod:
    pop hl
    pop bc
    push bc
    push hl
    ld a, c
    ld hl, 0x005F
    ld (0xC02C), hl
    call 0xC000     ; CHGMOD
    ret

_msx_bios_chgclr:
    ld hl, 0x0062
    ld (0xC02C), hl
    call 0xC000     ; CHGCLR
    ret

_msx_bios_grpprt:
    pop hl
    pop bc
    push bc
    push hl
    ld a, c
    ld hl, 0x008D
    ld (0xC02C), hl
    call 0xC000     ; GRPPRT
    ret

#endasm

void basic_cls(void) {
    uint8_t mode;
    uint16_t addr, size;

    basic_init();
    mode = sys_read8(SCRMOD);

    switch (mode) {
        case 0:
            /* SCREEN 0: Fill name table with spaces */
            addr = sys_read16(TXTNAM);
            size = (uint16_t)sys_read8(LINLEN) * (uint16_t)sys_read8(CRTCNT);
            sys_filvrm(addr, size, 0x20);
            break;
        case 1:
            /* SCREEN 1: Fill name table with spaces */
            addr = sys_read16(T32NAM);
            sys_filvrm(addr, 32 * 24, 0x20);
            break;
        case 2:
        case 4:
            /* SCREEN 2/4: Clear pattern generator (6144 bytes) */
            addr = sys_read16(GRPCGP);
            sys_filvrm(addr, 6144, 0x00);
            break;
        case 3:
            /* SCREEN 3 (multicolor): Clear pattern generator (1536 bytes) */
            sys_filvrm(0x0000, 1536, 0x00);
            break;
        default:
            /* SCREEN 5-12 (MSX2 bitmap): Use VDP command engine */
            if (mode >= 5 && mode <= 12) {
                uint16_t width = (mode == 6 || mode == 7) ? 512 : 256;
                vdp_fill(0, 0, width, 212, 0);
            }
            break;
    }

    /* Reset cursor to home position */
    sys_write8(CSRX, 1);
    sys_write8(CSRY, 1);
}

void basic_screen(uint8_t mode) {
    basic_init();
    msx_bios_chgmod(mode);
    sys_write8(CSRSW, 0x00);  /* Hide cursor */
}

void basic_screen_ex(uint8_t mode, uint8_t sprite_size, uint8_t key_click) {
    (void)sprite_size;
    (void)key_click;
    basic_screen(mode);
}

void basic_color(uint8_t fg, uint8_t bg, uint8_t border) {
    basic_init();
    sys_write8(FORCLR, fg);
    sys_write8(BAKCLR, bg);
    sys_write8(BDRCLR, border);
    msx_bios_chgclr();
}

void basic_color_fg(uint8_t fg) {
    sys_write8(FORCLR, fg);
}

void basic_locate(uint8_t x, uint8_t y) {
    /* Use system variables directly - more reliable than BIOS POSIT */
    sys_write8(CSRX, x + 1);  /* 1-based */
    sys_write8(CSRY, y + 1);  /* 1-based */
    if (sys_read8(SCRMOD) >= 2) {
        sys_write16(GRPACX, (uint16_t)x << 3);
        sys_write16(GRPACY, (uint16_t)y << 3);
    }
}

void basic_locate_ex(uint8_t x, uint8_t y, uint8_t cursor_visible) {
    basic_locate(x, y);
    basic_cursor(cursor_visible);
}

void basic_print(const char* s) {
    basic_init();
    if (sys_read8(SCRMOD) >= 2) {
        /* Graphics mode: use GRPPRT */
        sys_write16(GRPACX, (uint16_t)(sys_read8(CSRX) - 1) << 3);
        sys_write16(GRPACY, (uint16_t)(sys_read8(CSRY) - 1) << 3);
        sys_write8(ATRBYT, sys_read8(FORCLR));
        while (*s) {
            msx_bios_grpprt(*s++);
        }
        /* Update text cursor from graphics cursor */
        sys_write8(CSRX, (uint8_t)(sys_read16(GRPACX) >> 3) + 1);
        sys_write8(CSRY, (uint8_t)(sys_read16(GRPACY) >> 3) + 1);
    } else {
        /* Text mode: use CHPUT */
        while (*s) {
            msx_bios_chput(*s++);
        }
    }
}

void basic_println(const char* s) {
    basic_print(s);
    if (sys_read8(SCRMOD) >= 2) {
        msx_bios_grpprt(13);  /* CR */
        msx_bios_grpprt(10);  /* LF */
    } else {
        msx_bios_chput(13);  /* CR */
        msx_bios_chput(10);  /* LF */
    }
}

void basic_print_int(int16_t n) {
    char buffer[8];
    char* p = buffer + 7;
    uint16_t un;
    uint8_t neg = 0;

    *p = '\0';

    if (n < 0) {
        neg = 1;
        un = -n;
    } else {
        un = n;
    }

    if (un == 0) {
        *--p = '0';
    } else {
        while (un > 0) {
            *--p = '0' + (un % 10);
            un /= 10;
        }
    }

    if (neg) {
        *--p = '-';
    }

    basic_print(p);
}

void basic_print_char(uint8_t c) {
    basic_init();
    if (sys_read8(SCRMOD) >= 2) {
        sys_write16(GRPACX, (uint16_t)(sys_read8(CSRX) - 1) << 3);
        sys_write16(GRPACY, (uint16_t)(sys_read8(CSRY) - 1) << 3);
        sys_write8(ATRBYT, sys_read8(FORCLR));
        msx_bios_grpprt(c);
        sys_write8(CSRX, (uint8_t)(sys_read16(GRPACX) >> 3) + 1);
        sys_write8(CSRY, (uint8_t)(sys_read16(GRPACY) >> 3) + 1);
    } else {
        msx_bios_chput(c);
    }
}

void basic_print_num(uint16_t n) {
    char buf[6];
    int8_t i = 5;
    buf[5] = '\0';
    if (n == 0) {
        buf[4] = '0';
        i = 4;
    } else {
        while (n > 0 && i > 0) {
            i--;
            buf[i] = '0' + (n % 10);
            n /= 10;
        }
    }
    basic_print(&buf[i]);
}

static const char hex_chars[] = "0123456789ABCDEF";

void basic_print_hex8(uint8_t n) {
    basic_print_char(hex_chars[(n >> 4) & 0x0F]);
    basic_print_char(hex_chars[n & 0x0F]);
}

void basic_print_hex16(uint16_t n) {
    basic_print_hex8((uint8_t)(n >> 8));
    basic_print_hex8((uint8_t)(n & 0xFF));
}

void basic_width(uint8_t w) {
    uint8_t mode = sys_read8(SCRMOD);

    if (mode == 0) {
        if (w <= 32) {
            sys_write8(LINL40, 32);
        } else {
            sys_write8(LINL40, 40);
        }
        basic_screen(0);
    } else if (mode == 1) {
        sys_write8(LINL32, 32);
    }

    sys_write8(LINLEN, w);
}

uint8_t basic_csrlin(void) {
    return sys_read8(CSRY);
}

uint8_t basic_pos(uint8_t dummy) {
    (void)dummy;
    return sys_read8(CSRX);
}

void basic_cursor(uint8_t visible) {
    sys_write8(CSRSW, visible ? 0xFF : 0x00);
}

/* Write to VDP register using BIOS */
static void write_vdp_reg(uint8_t reg, uint8_t val) {
    #asm
        ld hl, 2
        add hl, sp
        ld e, (hl)      ; val
        inc hl
        inc hl
        ld a, (hl)      ; reg
        ld hl, 0x0047
        ld (0xC02C), hl
        call 0xC000     ; WRTVDP via trampoline
    #endasm
}

void basic_set_palette(uint8_t palette, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t rb_val = ((r & 0x07) << 4) | (b & 0x07);
    uint8_t g_val = g & 0x07;

    /* Set palette index register */
    write_vdp_reg(16, palette);

    /* Write RB and G to palette data port 0x9A */
    #asm
        di
    #endasm

    /* Output rb_val to port 0x9A */
    #asm
        ld hl, 2
        add hl, sp
        ld a, (hl)      ; g_val at SP+2
        ld c, a
        inc hl
        inc hl
        ld a, (hl)      ; rb_val at SP+4
        out (0x9A), a
        ld a, c
        out (0x9A), a
        ei
    #endasm
}

uint8_t basic_get_screen_mode(void) {
    return sys_read8(SCRMOD);
}

void basic_tab(uint8_t n) {
    uint8_t current_x = sys_read8(CSRX);
    if (n > current_x) {
        while (current_x < n) {
            basic_print_char(' ');
            current_x++;
        }
    }
}

void basic_spc(uint8_t n) {
    while (n > 0) {
        basic_print_char(' ');
        n--;
    }
}

/* VRAM address for pattern name table (depends on screen mode) */
static uint16_t get_name_table_addr(void) {
    uint8_t mode = sys_read8(SCRMOD);
    switch (mode) {
        case 0: return 0x0000;  /* SCREEN 0: 40x24 or 32x24 */
        case 1: return 0x1800;  /* SCREEN 1: 32x24 */
        default: return 0x0000;
    }
}

/* Read VRAM using BIOS */
static uint8_t read_vram(uint16_t addr) {
    uint8_t result;
    #asm
        ld hl, 2
        add hl, sp
        ld e, (hl)
        inc hl
        ld d, (hl)
        ex de, hl       ; HL = addr
        push hl
        ld hl, 0x004A
        ld (0xC02C), hl
        pop hl
        call 0xC000     ; RDVRM via trampoline
        ld l, a
    #endasm
}

uint8_t basic_screen_char(uint8_t x, uint8_t y) {
    uint16_t addr;
    uint8_t width;
    uint8_t mode = sys_read8(SCRMOD);

    /* Only text modes (0, 1) supported */
    if (mode > 1) return 0;

    width = sys_read8(LINLEN);
    addr = get_name_table_addr() + (uint16_t)y * width + x;

    return read_vram(addr);
}

uint8_t basic_screen_char_attr(uint8_t x, uint8_t y, uint8_t* attr) {
    uint8_t ch = basic_screen_char(x, y);
    /* In SCREEN 1, color table is at 0x2000, one byte per 8 chars */
    /* For simplicity, return foreground/background colors */
    if (attr) {
        *attr = sys_read8(FORCLR);
    }
    return ch;
}

/* PRINT USING for integers */
void basic_print_using_int(char* dest, const char* format, int32_t value) {
    uint8_t digit_count = 0;
    uint8_t has_decimal = 0;
    uint8_t decimal_places = 0;
    uint8_t show_plus = 0;
    uint8_t trailing_minus = 0;
    uint8_t has_comma = 0;
    uint8_t is_negative = 0;
    const char* f = format;
    char* p = dest;
    char num_buf[16];
    char* np;
    int8_t i;
    uint32_t uval;

    /* Parse format string */
    while (*f) {
        if (*f == '#') digit_count++;
        else if (*f == '.') { has_decimal = 1; }
        else if (*f == '+') show_plus = 1;
        else if (*f == '-') trailing_minus = 1;
        else if (*f == ',') has_comma = 1;
        f++;
    }

    /* Count decimal places */
    f = format;
    while (*f && *f != '.') f++;
    if (*f == '.') {
        f++;
        while (*f == '#') { decimal_places++; f++; }
    }

    /* Handle negative */
    if (value < 0) {
        is_negative = 1;
        uval = (uint32_t)(-value);
    } else {
        uval = (uint32_t)value;
    }

    /* Convert number to string (reverse) */
    np = num_buf + 15;
    *np = '\0';

    if (uval == 0) {
        *--np = '0';
    } else {
        i = 0;
        while (uval > 0) {
            if (has_comma && i > 0 && (i % 3) == 0) {
                *--np = ',';
            }
            *--np = '0' + (uval % 10);
            uval /= 10;
            i++;
        }
    }

    /* Add decimal places (zeros for integer) */
    if (decimal_places > 0) {
        char* dec_start = np;
        for (i = 0; i < decimal_places; i++) {
            *--np = '0';
        }
        /* Shift decimal point */
        np = dec_start - decimal_places;
        /* Insert decimal point (simplified: just add zeros) */
    }

    /* Add sign */
    if (show_plus || is_negative) {
        if (is_negative) {
            *--np = '-';
        } else if (show_plus) {
            *--np = '+';
        }
    }

    /* Pad with spaces to match format width */
    while (p - dest < digit_count - (int)(num_buf + 15 - np) && p - dest < 15) {
        *p++ = ' ';
    }

    /* Copy number */
    while (*np) {
        *p++ = *np++;
    }

    /* Add trailing minus if needed */
    if (trailing_minus && is_negative) {
        p[-1] = '-';  /* Replace sign position */
    }

    *p = '\0';
}

/* PRINT USING for floats */
void basic_print_using_float(char* dest, const char* format, float value) {
    int32_t int_part;
    float frac_part;
    uint8_t decimal_places = 0;
    uint8_t digit_count = 0;
    const char* f = format;
    char* p = dest;
    uint8_t i;
    int8_t is_negative = 0;

    /* Count format positions */
    while (*f) {
        if (*f == '#') digit_count++;
        f++;
    }

    /* Count decimal places */
    f = format;
    while (*f && *f != '.') f++;
    if (*f == '.') {
        f++;
        while (*f == '#') { decimal_places++; f++; }
    }

    /* Handle negative */
    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    /* Multiply by 10^decimal_places and round */
    for (i = 0; i < decimal_places; i++) {
        value *= 10.0f;
    }
    int_part = (int32_t)(value + 0.5f);

    /* Convert using integer function */
    basic_print_using_int(dest, format, is_negative ? -int_part : int_part);

    /* Insert decimal point */
    if (decimal_places > 0) {
        p = dest;
        while (*p) p++;
        /* Find position for decimal point */
        if (p - dest > decimal_places) {
            char* dp = p - decimal_places;
            /* Shift characters right */
            while (p >= dp) {
                *(p + 1) = *p;
                p--;
            }
            *dp = '.';
        }
    }
}

/* PRINT USING for strings */
void basic_print_using_str(char* dest, const char* format, const char* value) {
    char* p = dest;
    const char* v = value;
    uint8_t width = 0;

    if (*format == '!') {
        /* Single character */
        if (*value) {
            *p++ = *value;
        }
    } else if (*format == '&') {
        /* Full string */
        while (*v) {
            *p++ = *v++;
        }
    } else if (*format == '\\') {
        /* Fixed width - count spaces between backslashes */
        const char* f = format + 1;
        width = 2;  /* Minimum width */
        while (*f && *f != '\\') {
            width++;
            f++;
        }
        /* Copy up to width characters */
        while (*v && width > 0) {
            *p++ = *v++;
            width--;
        }
        /* Pad with spaces */
        while (width > 0) {
            *p++ = ' ';
            width--;
        }
    } else {
        /* Default: copy as-is */
        while (*v) {
            *p++ = *v++;
        }
    }

    *p = '\0';
}

/* WAIT for port value */
void basic_wait(uint16_t port, uint8_t and_mask, uint8_t xor_mask) {
    uint8_t val;
    do {
        #asm
            ld hl, 8
            add hl, sp
            ld c, (hl)      ; port low
            inc hl
            ld b, (hl)      ; port high
            in a, (c)
            ld hl, 4
            add hl, sp
            xor (hl)        ; XOR with xor_mask
            ld hl, 6
            add hl, sp
            and (hl)        ; AND with and_mask
            ld l, a
        #endasm
    } while (val == 0);
}
