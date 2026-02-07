/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file graphics.c
 * @brief MSX BASIC graphics functions implementation (Z88DK compatible)
 *
 * Supports MSX1 (SCREEN 2-3) and MSX2/MSX2+ (SCREEN 4-12)
 */

#include <stdint.h>
#include <msx.h>
#include "../../include/msxbasic/graphics.h"
#include "../../include/msxbasic/vdp.h"

/* MSX System Variables */
#define GRPACX      0xFCB7
#define GRPACY      0xFCB9
#define ATRBYT      0xF3F2
#define SCRMOD      0xFCAF
#define FORCLR      0xF3E9
#define BAKCLR      0xF3EA
#define MSXVER      0x002D
#define ACPAGE      0xFAF6  /* Active page (MSX2) */

/* Helper macros */
#define sys_write8(addr, val)  (*(volatile uint8_t*)(addr) = (val))
#define sys_read8(addr)        (*(volatile uint8_t*)(addr))
#define sys_write16(addr, val) (*(volatile uint16_t*)(addr) = (val))
#define sys_read16(addr)       (*(volatile uint16_t*)(addr))

/* Use cached MSX version from system.c */
extern uint8_t basic_is_msx2(void);

static uint8_t is_msx2(void) {
    return basic_is_msx2();
}

/* Pack color for SCREEN 6 (4 colors, 2 bits per pixel)
 * VRAM format: 4 pixels per byte, bits 7-6=pixel0, 5-4=pixel1, 3-2=pixel2, 1-0=pixel3 */
static uint8_t pack_color_screen6(uint8_t color) {
    color &= 0x03;  /* Mask to 2 bits */
    /* Pack 4 copies of 2-bit color into byte for VRAM storage */
    return (color << 6) | (color << 4) | (color << 2) | color;
}

/* Check if current screen mode uses MSX2 VDP commands */
static uint8_t is_msx2_gfx_mode(void) {
    uint8_t mode = sys_read8(SCRMOD);
    return (mode >= 5 && mode <= 12);
}

/* BIOS wrappers - sccz80 pushes parameters left-to-right */
#asm

PUBLIC _gfx_clrspr
PUBLIC _gfx_rdvrm
PUBLIC _gfx_wrtvrm

_gfx_clrspr:
    ld hl, 0x0069
    ld (0xC02C), hl
    call 0xC000     ; CLRSPR via trampoline
    ret

; uint8_t gfx_rdvrm(uint16_t addr)
; Stack: [ret_addr][addr]
_gfx_rdvrm:
    ld hl, 2
    add hl, sp
    ld e, (hl)      ; addr low
    inc hl
    ld d, (hl)      ; addr high
    ex de, hl       ; HL = addr
    push hl
    ld hl, 0x004A
    ld (0xC02C), hl
    pop hl
    call 0xC000     ; RDVRM via trampoline
    ld l, a         ; return value in L
    ret

; void gfx_wrtvrm(uint16_t addr, uint8_t val)
; Stack: [ret_addr][val][addr]
_gfx_wrtvrm:
    ld hl, 2
    add hl, sp
    ld a, (hl)      ; val at SP+2
    inc hl
    inc hl          ; skip high byte of val
    ld e, (hl)      ; addr low at SP+4
    inc hl
    ld d, (hl)      ; addr high at SP+5
    ex de, hl       ; HL = addr
    push hl
    push af
    ld hl, 0x004D
    ld (0xC02C), hl
    pop af
    pop hl
    call 0xC000     ; WRTVRM via trampoline
    ret

#endasm

extern void gfx_clrspr(void);
extern uint8_t gfx_rdvrm(uint16_t addr);
extern void gfx_wrtvrm(uint16_t addr, uint8_t val);

/* Static variables for extended VRAM access */
static uint16_t vram_addr;
static uint8_t vram_val;

/* MSX2 extended VRAM write (for addresses >= 0x4000) */
static void gfx_wrtvrm_ext(uint16_t addr, uint8_t val) {
    uint8_t mode = sys_read8(SCRMOD);

    /* For MSX1 modes or addresses < 0x4000, use standard BIOS */
    if (mode < 5 || addr < 0x4000) {
        gfx_wrtvrm(addr, val);
        return;
    }

    /* Store to static variables for assembly access */
    vram_addr = addr;
    vram_val = val;

    /* MSX2 extended VRAM access via direct VDP port */
    #asm
        di
        ; Set R#14 for extended address bits (bits 16-14 of address)
        ld hl, (_vram_addr + 1)  ; High byte of addr
        ld a, l
        rlca
        rlca
        and 0x07
        out (0x99), a
        ld a, 0x80 + 14
        out (0x99), a

        ; Set VRAM address for write
        ld hl, (_vram_addr)
        ld a, l             ; Low byte
        out (0x99), a
        ld a, h             ; High byte
        and 0x3F
        or 0x40             ; Set write bit
        out (0x99), a

        ; Write data
        ld a, (_vram_val)
        out (0x98), a

        ; Reset R#14 to 0
        xor a
        out (0x99), a
        ld a, 0x80 + 14
        out (0x99), a
        ei
    #endasm
}

/* SCREEN 2 VRAM layout */
#define SCR2_PATTERN_BASE   0x0000
#define SCR2_COLOR_BASE     0x2000

/* Forward declaration for basic_boxfill */
void basic_boxfill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);

/* Assembly implementation of basic_pset for MSX2 modes */
#asm

; void basic_pset_msx2(int16_t x, int16_t y, uint8_t color)
; sccz80 stack: [ret][color][y][x]
; SP+2 = color, SP+4 = y, SP+6 = x
PUBLIC _basic_pset_msx2
_basic_pset_msx2:
    ; Wait for VDP command complete
_pset_wait2:
    ld a, 2
    di
    out (0x99), a
    ld a, 0x8F
    out (0x99), a
    in a, (0x99)
    and 0x01
    jr nz, _pset_wait2
    xor a
    out (0x99), a
    ld a, 0x8F
    out (0x99), a

    ; Set R#17 to 36 with auto-increment
    ld a, 36
    out (0x99), a
    ld a, 0x80 + 17
    out (0x99), a

    ; Get parameters from stack and write to VDP
    ld hl, 6
    add hl, sp
    ld a, (hl)              ; R#36 DX low (x low)
    out (0x9B), a
    inc hl
    ld a, (hl)              ; R#37 DX high (x high)
    and 0x01
    out (0x9B), a

    ld hl, 4
    add hl, sp
    ld a, (hl)              ; R#38 DY low (y low)
    out (0x9B), a
    inc hl
    ld a, (hl)              ; R#39 DY high (y high)
    and 0x03
    out (0x9B), a

    ld a, 1                 ; R#40 NX low = 1
    out (0x9B), a
    xor a                   ; R#41 NX high = 0
    out (0x9B), a
    ld a, 1                 ; R#42 NY low = 1
    out (0x9B), a
    xor a                   ; R#43 NY high = 0
    out (0x9B), a

    ld hl, 2
    add hl, sp
    ld a, (hl)              ; R#44 color
    out (0x9B), a

    xor a                   ; R#45 ARG = 0
    out (0x9B), a
    ld a, 0x80              ; R#46 CMD = LMMV (0x80)
    out (0x9B), a

    ei
    ret

#endasm

extern void basic_pset_msx2(int16_t x, int16_t y, uint8_t color);

void basic_pset(int16_t x, int16_t y, uint8_t color) {
    uint8_t mode = sys_read8(SCRMOD);

    /* MSX2/MSX2+ modes (SCREEN 5-12) use VDP commands */
    if (mode >= 5 && mode <= 12) {
        int16_t max_x;
        uint8_t packed_color;

        /* Set max_x based on screen mode */
        if (mode == 6 || mode == 7) {
            max_x = 511;
        } else {
            max_x = 255;
        }

        /* Bounds check with signed comparison */
        if (x < 0 || x > max_x || y < 0 || y > 211) return;

        /* SCREEN 6: pack color (4 copies of 2-bit color) */
        if (mode == 6) {
            packed_color = pack_color_screen6(color);
        } else {
            packed_color = color;
        }

        /* Call assembly directly with stack parameters */
        basic_pset_msx2(x, y, packed_color);

        sys_write16(GRPACX, x);
        sys_write16(GRPACY, y);
        return;
    }

    /* SCREEN 4: Same as SCREEN 2 but with sprite mode 2 */
    if (mode == 4) {
        /* SCREEN 4 uses same pattern/color layout as SCREEN 2 */
        /* Fall through to software rendering */
    }

    /* MSX1 modes (SCREEN 2-4) use software rendering */
    {
        uint16_t pattern_num;
        uint16_t bank_offset;
        uint16_t pattern_addr;
        uint16_t color_addr;
        uint8_t bit_pos;
        uint8_t pattern_byte;
        uint8_t color_byte;

        /* Bounds check */
        if (x < 0 || x > 255 || y < 0 || y > 191) return;

        /* Calculate addresses for SCREEN 2
         * Screen is divided into 3 banks (y 0-63, 64-127, 128-191)
         * Each bank has 256 patterns (8 char rows x 32 char cols)
         */
        pattern_num = (((y >> 3) & 7) << 5) + (x >> 3);  /* ((y/8)%8)*32 + (x/8), 0-255 */
        bank_offset = (y >> 6) << 11;                     /* (y/64)*0x800 */
        pattern_addr = SCR2_PATTERN_BASE + bank_offset + (pattern_num << 3) + (y & 7);
        color_addr = SCR2_COLOR_BASE + bank_offset + (pattern_num << 3) + (y & 7);

        /* Calculate bit position (leftmost = bit 7) */
        bit_pos = 7 - (x & 7);

        /* Read current pattern, set bit */
        pattern_byte = gfx_rdvrm(pattern_addr);
        pattern_byte |= (1 << bit_pos);
        gfx_wrtvrm(pattern_addr, pattern_byte);

        /* Set color (foreground in high nibble, background in low nibble) */
        color_byte = (color << 4) | (sys_read8(BAKCLR) & 0x0F);
        gfx_wrtvrm(color_addr, color_byte);
    }

    /* Update graphic cursor position */
    sys_write16(GRPACX, x);
    sys_write16(GRPACY, y);
}

void basic_pset_c(int16_t x, int16_t y) {
    basic_pset(x, y, sys_read8(FORCLR));
}

/* Initialize graphics screen with background color */
void basic_init_grp(void) {
    uint8_t mode = sys_read8(SCRMOD);
    uint8_t bg_color = sys_read8(BAKCLR);
    uint8_t packed_color;

    /* MSX2/MSX2+ modes (SCREEN 5-12) - use VDP fill to clear screen */
    if (mode >= 5 && mode <= 12) {
        uint16_t width = (mode == 6 || mode == 7) ? 512 : 256;
        uint16_t height = 212;

        /* SCREEN 6: pack color (4 copies of 2-bit color) */
        if (mode == 6) {
            packed_color = pack_color_screen6(bg_color);
        } else {
            packed_color = bg_color;
        }

        /* For SCREEN 8, bg_color is already 0-255 (GRB 332 format)
         * For SCREEN 10-12 (YJK), fill with the specified color value */
        vdp_fill(0, 0, width, height, packed_color);
        return;
    }

    /* SCREEN 4: Same layout as SCREEN 2 */
    if (mode == 4) {
        /* Initialize color table same as SCREEN 2 */
    }

    /* MSX1 modes (SCREEN 2-4) - initialize color table */
    {
        uint16_t addr;
        uint8_t color_byte;

        color_byte = (sys_read8(FORCLR) << 4) | (bg_color & 0x0F);

        /* Initialize all 6144 bytes of color table (0x2000-0x37FF) */
        for (addr = SCR2_COLOR_BASE; addr < SCR2_COLOR_BASE + 0x1800; addr++) {
            gfx_wrtvrm(addr, color_byte);
        }
    }
}

void basic_pset_step(int16_t dx, int16_t dy, uint8_t color) {
    int16_t x = sys_read16(GRPACX) + dx;
    int16_t y = sys_read16(GRPACY) + dy;
    basic_pset(x, y, color);
}

void basic_preset(int16_t x, int16_t y) {
    basic_pset(x, y, sys_read8(BAKCLR));
}

void basic_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color) {
    uint8_t mode = sys_read8(SCRMOD);

    /* MSX2+ modes use hardware LINE command */
    if (mode >= 5 && mode <= 12) {
        uint8_t packed_color;

        /* SCREEN 6: pack color (4 copies of 2-bit color) */
        if (mode == 6) {
            packed_color = pack_color_screen6(color);
        } else {
            packed_color = color;
        }

        vdp_line((uint16_t)x1, (uint16_t)y1, (uint16_t)x2, (uint16_t)y2, packed_color, VDP_LOG_IMP);
        sys_write16(GRPACX, x2);
        sys_write16(GRPACY, y2);
        return;
    }

    /* MSX1 modes use software Bresenham */
    {
        int16_t dx, dy, sx, sy, err, e2;

        dx = x2 - x1;
        if (dx < 0) dx = -dx;
        dy = y2 - y1;
        if (dy < 0) dy = -dy;

        sx = (x1 < x2) ? 1 : -1;
        sy = (y1 < y2) ? 1 : -1;
        err = dx - dy;

        while (1) {
            basic_pset(x1, y1, color);

            if (x1 == x2 && y1 == y2) break;

            e2 = err * 2;
            if (e2 > -dy) {
                err -= dy;
                x1 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y1 += sy;
            }
        }
    }

    sys_write16(GRPACX, x2);
    sys_write16(GRPACY, y2);
}

void basic_line_ex(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color, uint8_t style) {
    if (style == LINE_STYLE_BOX) {
        basic_box(x1, y1, x2, y2, color);
    } else if (style == LINE_STYLE_BOXFILL) {
        basic_boxfill(x1, y1, x2, y2, color);
    } else {
        basic_line(x1, y1, x2, y2, color);
    }
}

void basic_box(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color) {
    basic_line(x1, y1, x2, y1, color);
    basic_line(x2, y1, x2, y2, color);
    basic_line(x2, y2, x1, y2, color);
    basic_line(x1, y2, x1, y1, color);
}

void basic_boxfill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color) {
    int16_t tmp;
    uint8_t mode = sys_read8(SCRMOD);
    uint8_t packed_color;

    if (x1 > x2) { tmp = x1; x1 = x2; x2 = tmp; }
    if (y1 > y2) { tmp = y1; y1 = y2; y2 = tmp; }

    /* MSX2/MSX2+ modes (SCREEN 5-12) use hardware fill */
    if (mode >= 5 && mode <= 12) {
        /* SCREEN 6: 4 colors, need to pack color byte */
        if (mode == 6) {
            packed_color = pack_color_screen6(color);
        } else {
            packed_color = color;
        }
        vdp_fill((uint16_t)x1, (uint16_t)y1,
                 (uint16_t)(x2 - x1 + 1), (uint16_t)(y2 - y1 + 1), packed_color);
        return;
    }

    /* MSX1 modes use software fill */
    {
        int16_t y;
        for (y = y1; y <= y2; y++) {
            basic_line(x1, y, x2, y, color);
        }
    }
}

void basic_circle(int16_t x, int16_t y, int16_t radius, uint8_t color) {
    /* Bresenham circle algorithm - accurate circle for all modes */
    int16_t cx = 0;
    int16_t cy = radius;
    int16_t d = 1 - radius;

    while (cx <= cy) {
        basic_pset(x + cx, y + cy, color);
        basic_pset(x - cx, y + cy, color);
        basic_pset(x + cx, y - cy, color);
        basic_pset(x - cx, y - cy, color);
        basic_pset(x + cy, y + cx, color);
        basic_pset(x - cy, y + cx, color);
        basic_pset(x + cy, y - cx, color);
        basic_pset(x - cy, y - cx, color);

        if (d < 0) {
            d += 2 * cx + 3;
        } else {
            d += 2 * (cx - cy) + 5;
            cy--;
        }
        cx++;
    }

    sys_write16(GRPACX, x);
    sys_write16(GRPACY, y);
}

/* Sine table for 0-90 degrees, scaled by 256 (sin * 256) */
static const uint8_t sin_table[91] = {
    0, 4, 9, 13, 18, 22, 27, 31, 36, 40, 44, 49, 53, 58, 62, 66,
    71, 75, 79, 83, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 131,
    135, 139, 142, 146, 149, 152, 156, 159, 162, 165, 168, 171, 174, 177, 180, 183,
    185, 188, 190, 193, 195, 198, 200, 202, 204, 206, 208, 210, 212, 214, 215, 217,
    219, 220, 222, 223, 224, 226, 227, 228, 229, 230, 231, 232, 233, 234, 234, 235,
    236, 236, 237, 237, 238, 238, 239, 239, 239, 240, 240
};

/* Get sin value * 256 for angle in degrees */
static int16_t get_sin256(int16_t deg) {
    int8_t neg = 0;

    /* Normalize to 0-359 */
    while (deg < 0) deg += 360;
    while (deg >= 360) deg -= 360;

    /* Handle quadrants */
    if (deg > 270) {
        deg = 360 - deg;
        neg = 1;
    } else if (deg > 180) {
        deg = deg - 180;
        neg = 1;
    } else if (deg > 90) {
        deg = 180 - deg;
    }

    return neg ? -(int16_t)sin_table[deg] : (int16_t)sin_table[deg];
}

/* Get cos value * 256 for angle in degrees */
static int16_t get_cos256(int16_t deg) {
    return get_sin256(deg + 90);
}

void basic_circle_ex(int16_t x, int16_t y, int16_t radius, uint8_t color,
                     int16_t start_deg, int16_t end_deg, int16_t aspect_100) {
    int16_t deg;
    int16_t rx, ry;
    int16_t px, py;

    /* Default aspect ratio is 100 (1:1) */
    if (aspect_100 <= 0) aspect_100 = 100;

    /* Normalize angles */
    while (start_deg < 0) start_deg += 360;
    while (end_deg < 0) end_deg += 360;
    while (start_deg >= 360) start_deg -= 360;
    while (end_deg >= 360) end_deg -= 360;

    /* Calculate radii based on aspect ratio */
    if (aspect_100 >= 100) {
        rx = radius;
        ry = (int16_t)((int32_t)radius * 100 / aspect_100);
    } else {
        rx = (int16_t)((int32_t)radius * aspect_100 / 100);
        ry = radius;
    }

    /* Draw arc by stepping through angles */
    for (deg = start_deg; ; deg++) {
        if (deg >= 360) deg -= 360;

        /* Calculate point on ellipse */
        px = x + (int16_t)((int32_t)rx * get_cos256(deg) / 256);
        py = y - (int16_t)((int32_t)ry * get_sin256(deg) / 256);

        basic_pset(px, py, color);

        if (deg == end_deg) break;
    }

    sys_write16(GRPACX, x);
    sys_write16(GRPACY, y);
}

void basic_ellipse(int16_t x, int16_t y, int16_t rx, int16_t ry, uint8_t color) {
    int32_t rx2 = (int32_t)rx * rx;
    int32_t ry2 = (int32_t)ry * ry;
    int16_t cx = 0;
    int16_t cy = ry;
    int32_t px = 0;
    int32_t py = 2 * rx2 * cy;
    int32_t p;

    p = ry2 - rx2 * ry + rx2 / 4;
    while (px < py) {
        basic_pset(x + cx, y + cy, color);
        basic_pset(x - cx, y + cy, color);
        basic_pset(x + cx, y - cy, color);
        basic_pset(x - cx, y - cy, color);

        cx++;
        px += 2 * ry2;
        if (p < 0) {
            p += ry2 + px;
        } else {
            cy--;
            py -= 2 * rx2;
            p += ry2 + px - py;
        }
    }

    p = ry2 * (cx + 1) * (cx + 1) / 4 + rx2 * (cy - 1) * (cy - 1) - rx2 * ry2;
    while (cy >= 0) {
        basic_pset(x + cx, y + cy, color);
        basic_pset(x - cx, y + cy, color);
        basic_pset(x + cx, y - cy, color);
        basic_pset(x - cx, y - cy, color);

        cy--;
        py -= 2 * rx2;
        if (p > 0) {
            p += rx2 - py;
        } else {
            cx++;
            px += 2 * ry2;
            p += rx2 - py + px;
        }
    }
}

/* Stack for scanline flood fill - limited size for MSX memory constraints */
#define PAINT_STACK_SIZE 128

typedef struct {
    int16_t x1, x2, y, dy;
} PaintSpan;

static PaintSpan paint_stack[PAINT_STACK_SIZE];
static int16_t paint_sp;

static void paint_push(int16_t x1, int16_t x2, int16_t y, int16_t dy) {
    if (paint_sp < PAINT_STACK_SIZE) {
        paint_stack[paint_sp].x1 = x1;
        paint_stack[paint_sp].x2 = x2;
        paint_stack[paint_sp].y = y;
        paint_stack[paint_sp].dy = dy;
        paint_sp++;
    }
}

static int paint_pop(int16_t *x1, int16_t *x2, int16_t *y, int16_t *dy) {
    if (paint_sp > 0) {
        paint_sp--;
        *x1 = paint_stack[paint_sp].x1;
        *x2 = paint_stack[paint_sp].x2;
        *y = paint_stack[paint_sp].y;
        *dy = paint_stack[paint_sp].dy;
        return 1;
    }
    return 0;
}

void basic_paint(int16_t x, int16_t y, uint8_t color, uint8_t border) {
    uint8_t mode = sys_read8(SCRMOD);
    int16_t max_x, max_y;
    int16_t x1, x2, ny, dy;
    int16_t lx, rx;
    uint8_t target_color;

    /* Get screen dimensions */
    if (mode == 6 || mode == 7) {
        max_x = 511;
    } else {
        max_x = 255;
    }
    max_y = (mode >= 5 && mode <= 12) ? 211 : 191;

    /* Bounds check */
    if (x < 0 || x > max_x || y < 0 || y > max_y) return;

    /* Get the color at starting point */
    target_color = basic_point(x, y);

    /* Don't fill if starting point is already the fill color or border */
    if (target_color == color || target_color == border) return;

    /* Initialize stack */
    paint_sp = 0;

    /* Push initial span */
    paint_push(x, x, y, 1);
    paint_push(x, x, y - 1, -1);

    while (paint_pop(&x1, &x2, &y, &dy)) {
        ny = y + dy;

        /* Bounds check for new y */
        if (ny < 0 || ny > max_y) continue;

        /* Scan left from x1 */
        lx = x1;
        while (lx > 0) {
            uint8_t c = basic_point(lx - 1, ny);
            if (c == border || c == color) break;
            lx--;
        }

        /* Scan right from x2 */
        rx = x2;
        while (rx < max_x) {
            uint8_t c = basic_point(rx + 1, ny);
            if (c == border || c == color) break;
            rx++;
        }

        /* Check if current point is valid */
        {
            uint8_t c = basic_point(x1, ny);
            if (c == border || c == color) {
                /* Find next valid segment */
                int16_t sx = x1;
                while (sx <= rx) {
                    c = basic_point(sx, ny);
                    if (c != border && c != color) break;
                    sx++;
                }
                if (sx > rx) continue;
                lx = sx;
                while (lx > 0) {
                    c = basic_point(lx - 1, ny);
                    if (c == border || c == color) break;
                    lx--;
                }
            }
        }

        /* Fill the scanline from lx to rx */
        {
            int16_t fx;
            for (fx = lx; fx <= rx; fx++) {
                uint8_t c = basic_point(fx, ny);
                if (c != border && c != color) {
                    basic_pset(fx, ny, color);
                }
            }
        }

        /* Push spans for next scanline */
        if (lx < x1) {
            paint_push(lx, x1 - 1, ny, -dy);
        }
        if (rx > x2) {
            paint_push(x2 + 1, rx, ny, -dy);
        }
        paint_push(lx, rx, ny, dy);
    }
}

void basic_paint_c(int16_t x, int16_t y, uint8_t color) {
    basic_paint(x, y, color, color);
}

void basic_draw(const char* cmd) {
    int16_t x = sys_read16(GRPACX);
    int16_t y = sys_read16(GRPACY);
    uint8_t color = sys_read8(FORCLR);
    uint8_t pen_down = 1;
    uint8_t no_update = 0;  /* N prefix: don't update position */
    uint8_t angle = 0;      /* A0-3: rotation (0=0, 1=90, 2=180, 3=270) */
    int16_t scale = 4;      /* S1-255: scale factor (4 = default, means 1:1) */
    int16_t dist;
    int16_t dx, dy, tmp;
    int16_t num1, num2;

    while (*cmd) {
        char c = *cmd++;

        if (c == ' ' || c == '\t' || c == ';') continue;

        /* Parse number after command */
        dist = 0;
        while (*cmd >= '0' && *cmd <= '9') {
            dist = dist * 10 + (*cmd - '0');
            cmd++;
        }

        /* Check for second number after comma (for M command) */
        num1 = dist;
        num2 = 0;
        if (*cmd == ',') {
            cmd++;
            while (*cmd >= '0' && *cmd <= '9') {
                num2 = num2 * 10 + (*cmd - '0');
                cmd++;
            }
        }

        dx = 0;
        dy = 0;

        switch (c) {
            case 'U': case 'u':
                if (dist == 0) dist = 1;
                dy = -(dist * scale) / 4;
                break;
            case 'D': case 'd':
                if (dist == 0) dist = 1;
                dy = (dist * scale) / 4;
                break;
            case 'L': case 'l':
                if (dist == 0) dist = 1;
                dx = -(dist * scale) / 4;
                break;
            case 'R': case 'r':
                if (dist == 0) dist = 1;
                dx = (dist * scale) / 4;
                break;
            case 'E': case 'e':
                if (dist == 0) dist = 1;
                dx = (dist * scale) / 4;
                dy = -(dist * scale) / 4;
                break;
            case 'F': case 'f':
                if (dist == 0) dist = 1;
                dx = (dist * scale) / 4;
                dy = (dist * scale) / 4;
                break;
            case 'G': case 'g':
                if (dist == 0) dist = 1;
                dx = -(dist * scale) / 4;
                dy = (dist * scale) / 4;
                break;
            case 'H': case 'h':
                if (dist == 0) dist = 1;
                dx = -(dist * scale) / 4;
                dy = -(dist * scale) / 4;
                break;
            case 'M': case 'm':
                /* Absolute or relative move */
                if (*cmd == '+' || *cmd == '-') {
                    /* Relative move: M+dx,dy or M-dx,dy */
                    int8_t sign = (*cmd == '-') ? -1 : 1;
                    cmd++;
                    num1 = 0;
                    while (*cmd >= '0' && *cmd <= '9') {
                        num1 = num1 * 10 + (*cmd - '0');
                        cmd++;
                    }
                    num1 *= sign;
                    if (*cmd == ',') {
                        cmd++;
                        sign = 1;
                        if (*cmd == '-') { sign = -1; cmd++; }
                        else if (*cmd == '+') cmd++;
                        num2 = 0;
                        while (*cmd >= '0' && *cmd <= '9') {
                            num2 = num2 * 10 + (*cmd - '0');
                            cmd++;
                        }
                        num2 *= sign;
                    }
                    dx = num1;
                    dy = num2;
                } else {
                    /* Absolute move: Mx,y */
                    if (pen_down) {
                        basic_line(x, y, num1, num2, color);
                    }
                    x = num1;
                    y = num2;
                    pen_down = 1;
                    no_update = 0;
                    continue;
                }
                break;
            case 'B': case 'b':
                pen_down = 0;
                continue;
            case 'N': case 'n':
                no_update = 1;
                continue;
            case 'C': case 'c':
                color = (uint8_t)dist;
                continue;
            case 'A': case 'a':
                /* Angle: 0=0, 1=90, 2=180, 3=270 degrees */
                angle = (uint8_t)(dist & 3);
                continue;
            case 'S': case 's':
                /* Scale: 1-255 (4 = default 1:1) */
                if (dist > 0 && dist <= 255) scale = dist;
                continue;
            default:
                continue;
        }

        /* Apply angle rotation */
        switch (angle) {
            case 1: /* 90 degrees */
                tmp = dx; dx = -dy; dy = tmp;
                break;
            case 2: /* 180 degrees */
                dx = -dx; dy = -dy;
                break;
            case 3: /* 270 degrees */
                tmp = dx; dx = dy; dy = -tmp;
                break;
        }

        if (dx != 0 || dy != 0) {
            int16_t new_x = x + dx;
            int16_t new_y = y + dy;

            if (pen_down) {
                basic_line(x, y, new_x, new_y, color);
            }

            if (!no_update) {
                x = new_x;
                y = new_y;
            }
        }

        pen_down = 1;
        no_update = 0;
    }

    sys_write16(GRPACX, x);
    sys_write16(GRPACY, y);
}

uint8_t basic_point(int16_t x, int16_t y) {
    uint16_t pattern_num;
    uint16_t bank_offset;
    uint16_t pattern_addr;
    uint16_t color_addr;
    uint8_t bit_pos;
    uint8_t pattern_byte;
    uint8_t color_byte;

    /* Bounds check */
    if (x < 0 || x > 255 || y < 0 || y > 191) return 0;

    /* Calculate addresses (same as basic_pset) */
    pattern_num = (((y >> 3) & 7) << 5) + (x >> 3);
    bank_offset = (y >> 6) << 11;
    pattern_addr = SCR2_PATTERN_BASE + bank_offset + (pattern_num << 3) + (y & 7);
    color_addr = SCR2_COLOR_BASE + bank_offset + (pattern_num << 3) + (y & 7);

    bit_pos = 7 - (x & 7);

    pattern_byte = gfx_rdvrm(pattern_addr);
    color_byte = gfx_rdvrm(color_addr);

    sys_write16(GRPACX, x);
    sys_write16(GRPACY, y);

    /* Return foreground or background color based on pattern bit */
    if (pattern_byte & (1 << bit_pos)) {
        return (color_byte >> 4) & 0x0F;  /* foreground */
    } else {
        return color_byte & 0x0F;          /* background */
    }
}

int16_t basic_grp_x(void) {
    return sys_read16(GRPACX);
}

int16_t basic_grp_y(void) {
    return sys_read16(GRPACY);
}

void basic_grp_move(int16_t x, int16_t y) {
    sys_write16(GRPACX, x);
    sys_write16(GRPACY, y);
}

/* SCREEN 2/4 sprite tables (MSX1) */
#define SCR2_SAT_BASE       0x1B00  /* Sprite Attribute Table */
#define SCR2_SPG_BASE       0x3800  /* Sprite Pattern Generator */

/* SCREEN 5-8 sprite tables (MSX2) */
#define SCR5_SAT_BASE       0x7600  /* Sprite Attribute Table */
#define SCR5_SPG_BASE       0x7800  /* Sprite Pattern Generator */

#define SPRITE_OFF_Y        208     /* Y value to hide sprite (216 for MSX2) */

/* System variable for sprite size */
#define RG1SAV      0xF3E0  /* VDP register 1 shadow */

/* Get sprite size in bytes: 8 = 8x8, 32 = 16x16 */
static uint8_t get_sprite_size(void) {
    return (sys_read8(RG1SAV) & 0x02) ? 32 : 8;
}

/* Get sprite attribute table base address based on screen mode */
static uint16_t get_sat_base(void) {
    uint8_t mode = sys_read8(SCRMOD);
    if (mode >= 5 && mode <= 8) {
        return SCR5_SAT_BASE;
    }
    return SCR2_SAT_BASE;
}

/* Get sprite pattern generator base address based on screen mode */
static uint16_t get_spg_base(void) {
    uint8_t mode = sys_read8(SCRMOD);
    if (mode >= 5 && mode <= 8) {
        return SCR5_SPG_BASE;
    }
    return SCR2_SPG_BASE;
}

void basic_sprite_size(uint8_t size) {
    uint8_t rg1 = sys_read8(RG1SAV);

    /* Clear size bits */
    rg1 &= ~0x03;

    /* Set new size:
     * 0 = 8x8 normal
     * 1 = 16x16 normal
     * 2 = 8x8 magnified (x2)
     * 3 = 16x16 magnified (x2)
     */
    if (size & 1) rg1 |= 0x02;  /* 16x16 */
    if (size & 2) rg1 |= 0x01;  /* Magnified */

    sys_write8(RG1SAV, rg1);

    /* Write to VDP register 1 */
    #asm
        ld hl, 0xF3E0   ; RG1SAV address
        ld a, (hl)
        ld b, a
        ld c, 1         ; Register 1
        ld hl, 0x0047
        ld (0xC02C), hl
        call 0xC000     ; WRTVDP via trampoline
    #endasm
}

void basic_sprite_pattern(uint8_t pattern_num, const uint8_t* pattern) {
    uint16_t addr;
    uint8_t size;
    uint8_t i;

    size = get_sprite_size();
    addr = get_spg_base() + (uint16_t)pattern_num * size;

    for (i = 0; i < size; i++) {
        gfx_wrtvrm_ext(addr + i, pattern[i]);
    }
}

void basic_put_sprite(uint8_t sprite_num, int16_t x, int16_t y, uint8_t color, uint8_t pattern) {
    uint16_t sat_addr;
    uint8_t ec_bit = 0;

    if (sprite_num > 31) return;

    sat_addr = get_sat_base() + (uint16_t)sprite_num * 4;

    /* Handle negative X with Early Clock bit */
    if (x < 0) {
        ec_bit = 0x80;
        x += 32;
    }

    /* Write sprite attribute (Y, X, pattern, color+EC) */
    gfx_wrtvrm_ext(sat_addr + 0, (uint8_t)(y - 1));  /* Y is offset by 1 in hardware */
    gfx_wrtvrm_ext(sat_addr + 1, (uint8_t)x);
    gfx_wrtvrm_ext(sat_addr + 2, pattern);
    gfx_wrtvrm_ext(sat_addr + 3, (color & 0x0F) | ec_bit);
}

void basic_sprite_off(uint8_t sprite_num) {
    uint16_t sat_addr;

    if (sprite_num > 31) return;

    sat_addr = get_sat_base() + (uint16_t)sprite_num * 4;
    gfx_wrtvrm_ext(sat_addr + 0, SPRITE_OFF_Y);  /* Y = 208 hides sprite */
}

void basic_sprites_off(void) {
    uint8_t i;

    /* Hide all 32 sprites */
    for (i = 0; i < 32; i++) {
        basic_sprite_off(i);
    }
}

uint8_t basic_sprite_collision(void) {
    /* Read STATFL (0xF3E7) which is updated by the interrupt handler
     * each VBLANK. Reading RDVDP directly races with the interrupt
     * handler that already cleared S#0. */
    return (*(volatile uint8_t*)0xF3E7 & 0x20) ? 1 : 0;
}

void basic_copy(int16_t sx, int16_t sy, uint16_t width, uint16_t height,
                int16_t dx, int16_t dy) {
    if (!is_msx2()) return;
    vdp_copy((uint16_t)sx, (uint16_t)sy, (uint16_t)dx, (uint16_t)dy, width, height);
}

void basic_copy_page(int16_t sx, int16_t sy, uint16_t width, uint16_t height,
                     uint8_t src_page, int16_t dx, int16_t dy, uint8_t dst_page) {
    uint16_t page_height;
    uint16_t sy_abs, dy_abs;
    uint8_t mode;

    if (!is_msx2()) return;

    mode = sys_read8(SCRMOD);

    /* Calculate page height based on screen mode */
    if (mode == 5 || mode == 6) {
        page_height = 256;  /* 4 pages in 128KB */
    } else if (mode == 7 || mode == 8) {
        page_height = 256;  /* 2 pages in 128KB */
    } else {
        page_height = 212;  /* Default */
    }

    /* Calculate absolute Y coordinates including page offset */
    sy_abs = (uint16_t)sy + (src_page * page_height);
    dy_abs = (uint16_t)dy + (dst_page * page_height);

    vdp_copy((uint16_t)sx, sy_abs, (uint16_t)dx, dy_abs, width, height);
}

void basic_set_page(uint8_t display_page, uint8_t active_page) {
    if (!is_msx2()) return;
    vdp_set_display_page(display_page);
    vdp_set_active_page(active_page);
}

/* === GET/PUT Graphics Block Operations === */

uint16_t basic_get_size(uint16_t width, uint16_t height) {
    uint8_t mode = sys_read8(SCRMOD);
    uint16_t bytes_per_line;

    /* Calculate bytes per line based on screen mode */
    switch (mode) {
        case 5:  /* 256x212, 16 colors (4 bits/pixel) */
            bytes_per_line = (width + 1) / 2;
            break;
        case 6:  /* 512x212, 4 colors (2 bits/pixel) */
            bytes_per_line = (width + 3) / 4;
            break;
        case 7:  /* 512x212, 16 colors (4 bits/pixel) */
            bytes_per_line = (width + 1) / 2;
            break;
        case 8:  /* 256x212, 256 colors (8 bits/pixel) */
            bytes_per_line = width;
            break;
        default: /* SCREEN 2/4: 1 bit/pixel */
            bytes_per_line = (width + 7) / 8;
            break;
    }

    /* Header (4 bytes: width, height) + image data */
    return 4 + bytes_per_line * height;
}

uint16_t basic_get(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t* buffer) {
    uint8_t mode = sys_read8(SCRMOD);
    int16_t width, height;
    int16_t x, y;
    uint16_t offset;
    int16_t tmp;

    /* Ensure x1 <= x2 and y1 <= y2 */
    if (x1 > x2) { tmp = x1; x1 = x2; x2 = tmp; }
    if (y1 > y2) { tmp = y1; y1 = y2; y2 = tmp; }

    width = x2 - x1 + 1;
    height = y2 - y1 + 1;

    /* Store dimensions in header */
    buffer[0] = width & 0xFF;
    buffer[1] = (width >> 8) & 0xFF;
    buffer[2] = height & 0xFF;
    buffer[3] = (height >> 8) & 0xFF;
    offset = 4;

    /* For MSX2 modes, use VDP HMMC command would be optimal,
     * but for simplicity, read pixel by pixel */
    if (mode >= 5 && mode <= 8) {
        for (y = y1; y <= y2; y++) {
            for (x = x1; x <= x2; x++) {
                buffer[offset++] = basic_point(x, y);
            }
        }
    } else {
        /* MSX1 modes: read pixel by pixel */
        for (y = y1; y <= y2; y++) {
            for (x = x1; x <= x2; x++) {
                buffer[offset++] = basic_point(x, y);
            }
        }
    }

    return offset;
}

void basic_put(int16_t x, int16_t y, const uint8_t* buffer, uint8_t op) {
    uint16_t width, height;
    int16_t dx, dy;
    uint16_t offset;
    uint8_t src_color, dst_color, result_color;

    /* Read dimensions from header */
    width = buffer[0] | ((uint16_t)buffer[1] << 8);
    height = buffer[2] | ((uint16_t)buffer[3] << 8);
    offset = 4;

    for (dy = 0; dy < (int16_t)height; dy++) {
        for (dx = 0; dx < (int16_t)width; dx++) {
            src_color = buffer[offset++];

            switch (op) {
                case 0: /* PUT_PSET - Overwrite */
                    basic_pset(x + dx, y + dy, src_color);
                    break;

                case 1: /* PUT_AND */
                    dst_color = basic_point(x + dx, y + dy);
                    result_color = src_color & dst_color;
                    basic_pset(x + dx, y + dy, result_color);
                    break;

                case 2: /* PUT_OR */
                    dst_color = basic_point(x + dx, y + dy);
                    result_color = src_color | dst_color;
                    basic_pset(x + dx, y + dy, result_color);
                    break;

                case 3: /* PUT_XOR */
                    dst_color = basic_point(x + dx, y + dy);
                    result_color = src_color ^ dst_color;
                    basic_pset(x + dx, y + dy, result_color);
                    break;

                case 4: /* PUT_PRESET - Inverted */
                    basic_pset(x + dx, y + dy, ~src_color & 0x0F);
                    break;
            }
        }
    }
}

/* === Filled Circle and Ellipse === */

void basic_circle_fill(int16_t x, int16_t y, int16_t radius, uint8_t color) {
    int16_t cx = 0;
    int16_t cy = radius;
    int16_t d = 1 - radius;

    while (cx <= cy) {
        /* Draw horizontal lines for each y level */
        basic_line(x - cx, y + cy, x + cx, y + cy, color);
        basic_line(x - cx, y - cy, x + cx, y - cy, color);
        basic_line(x - cy, y + cx, x + cy, y + cx, color);
        basic_line(x - cy, y - cx, x + cy, y - cx, color);

        if (d < 0) {
            d += 2 * cx + 3;
        } else {
            d += 2 * (cx - cy) + 5;
            cy--;
        }
        cx++;
    }

    sys_write16(GRPACX, x);
    sys_write16(GRPACY, y);
}

void basic_ellipse_fill(int16_t x, int16_t y, int16_t rx, int16_t ry, uint8_t color) {
    int32_t rx2 = (int32_t)rx * rx;
    int32_t ry2 = (int32_t)ry * ry;
    int16_t cx = 0;
    int16_t cy = ry;
    int32_t px = 0;
    int32_t py = 2 * rx2 * cy;
    int32_t p;

    /* Draw initial horizontal lines */
    basic_line(x - rx, y, x + rx, y, color);

    /* Region 1 */
    p = ry2 - rx2 * ry + rx2 / 4;
    while (px < py) {
        cx++;
        px += 2 * ry2;
        if (p < 0) {
            p += ry2 + px;
        } else {
            cy--;
            py -= 2 * rx2;
            p += ry2 + px - py;
            /* Draw horizontal lines */
            basic_line(x - cx, y + cy, x + cx, y + cy, color);
            basic_line(x - cx, y - cy, x + cx, y - cy, color);
        }
    }

    /* Region 2 */
    p = ry2 * (cx + 1) * (cx + 1) / 4 + rx2 * (cy - 1) * (cy - 1) - rx2 * ry2;
    while (cy >= 0) {
        basic_line(x - cx, y + cy, x + cx, y + cy, color);
        basic_line(x - cx, y - cy, x + cx, y - cy, color);

        cy--;
        py -= 2 * rx2;
        if (p > 0) {
            p += rx2 - py;
        } else {
            cx++;
            px += 2 * ry2;
            p += rx2 - py + px;
        }
    }

    sys_write16(GRPACX, x);
    sys_write16(GRPACY, y);
}
