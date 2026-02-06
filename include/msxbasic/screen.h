/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file screen.h
 * @brief MSX BASIC screen control functions
 *
 * Provides functions equivalent to MSX BASIC's:
 * CLS, SCREEN, COLOR, LOCATE, PRINT, WIDTH, CSRLIN, POS
 */

#ifndef MSXBASIC_SCREEN_H
#define MSXBASIC_SCREEN_H

#include <stdint.h>

/* Screen modes */
#define SCREEN_0    0   /* Text 40x24 (WIDTH 40) or 32x24 (WIDTH 32) */
#define SCREEN_1    1   /* Text 32x24 with color */
#define SCREEN_2    2   /* Graphics 256x192, 16 colors */
#define SCREEN_3    3   /* Multi-color 64x48 */
#define SCREEN_4    4   /* Graphics 256x192 (MSX2) */
#define SCREEN_5    5   /* Graphics 256x212, 16 colors (MSX2) */
#define SCREEN_6    6   /* Graphics 512x212, 4 colors (MSX2) */
#define SCREEN_7    7   /* Graphics 512x212, 16 colors (MSX2) */
#define SCREEN_8    8   /* Graphics 256x212, 256 colors (MSX2) */
#define SCREEN_10   10  /* Graphics 256x212, YJK (MSX2+) */
#define SCREEN_11   11  /* Graphics 256x212, YJK (MSX2+) */
#define SCREEN_12   12  /* Graphics 256x212, YJK+YAE (MSX2+) */

/* Color codes (MSX standard colors) */
#define COLOR_TRANSPARENT   0
#define COLOR_BLACK         1
#define COLOR_GREEN         2
#define COLOR_LIGHT_GREEN   3
#define COLOR_DARK_BLUE     4
#define COLOR_LIGHT_BLUE    5
#define COLOR_DARK_RED      6
#define COLOR_CYAN          7
#define COLOR_RED           8
#define COLOR_LIGHT_RED     9
#define COLOR_DARK_YELLOW   10
#define COLOR_LIGHT_YELLOW  11
#define COLOR_DARK_GREEN    12
#define COLOR_MAGENTA       13
#define COLOR_GRAY          14
#define COLOR_WHITE         15

/**
 * @brief Clear the screen
 * Equivalent to: CLS
 */
void basic_cls(void);

/**
 * @brief Set screen mode
 * Equivalent to: SCREEN n
 * @param mode Screen mode (0-12)
 */
void basic_screen(uint8_t mode);

/**
 * @brief Set screen mode with extended parameters
 * Equivalent to: SCREEN mode, sprite_size, key_click, baud, printer, interlace
 * @param mode Screen mode (0-12)
 * @param sprite_size Sprite size (0=8x8, 1=8x8 magnified, 2=16x16, 3=16x16 magnified)
 * @param key_click Key click sound (0=off, 1=on)
 */
void basic_screen_ex(uint8_t mode, uint8_t sprite_size, uint8_t key_click);

/**
 * @brief Set foreground, background, and border colors
 * Equivalent to: COLOR fg, bg, border
 * @param fg Foreground color (0-15)
 * @param bg Background color (0-15)
 * @param border Border color (0-15)
 */
void basic_color(uint8_t fg, uint8_t bg, uint8_t border);

/**
 * @brief Set foreground color only
 * Equivalent to: COLOR fg
 * @param fg Foreground color (0-15)
 */
void basic_color_fg(uint8_t fg);

/**
 * @brief Move cursor to specified position
 * Equivalent to: LOCATE x, y
 * @param x Column (0-based)
 * @param y Row (0-based)
 */
void basic_locate(uint8_t x, uint8_t y);

/**
 * @brief Move cursor and optionally show/hide cursor
 * Equivalent to: LOCATE x, y, cursor_switch
 * @param x Column (0-based)
 * @param y Row (0-based)
 * @param cursor_visible 0=hide cursor, 1=show cursor
 */
void basic_locate_ex(uint8_t x, uint8_t y, uint8_t cursor_visible);

/**
 * @brief Print a string at current cursor position
 * Equivalent to: PRINT s$
 * @param s String to print (null-terminated)
 */
void basic_print(const char* s);

/**
 * @brief Print a string followed by newline
 * Equivalent to: PRINT s$
 * @param s String to print (null-terminated)
 */
void basic_println(const char* s);

/**
 * @brief Print an integer number
 * Equivalent to: PRINT n
 * @param n Integer to print
 */
void basic_print_int(int16_t n);

/**
 * @brief Print a character
 * Equivalent to: PRINT CHR$(c)
 * @param c Character code
 */
void basic_print_char(uint8_t c);

/**
 * @brief Print an unsigned integer
 * @param n Unsigned integer to print
 */
void basic_print_num(uint16_t n);

/**
 * @brief Print a byte in hexadecimal
 * @param n Byte to print
 */
void basic_print_hex8(uint8_t n);

/**
 * @brief Print a 16-bit value in hexadecimal
 * @param n Value to print
 */
void basic_print_hex16(uint16_t n);

/**
 * @brief Set screen width
 * Equivalent to: WIDTH n
 * @param w Width (32 or 40 for SCREEN 0)
 */
void basic_width(uint8_t w);

/**
 * @brief Get current cursor line (row)
 * Equivalent to: CSRLIN
 * @return Current cursor row (1-based, as in BASIC)
 */
uint8_t basic_csrlin(void);

/**
 * @brief Get current cursor column
 * Equivalent to: POS(0)
 * @param dummy Dummy parameter (ignored, for BASIC compatibility)
 * @return Current cursor column (1-based, as in BASIC)
 */
uint8_t basic_pos(uint8_t dummy);

/**
 * @brief Control cursor visibility
 * @param visible 0=hide, 1=show
 */
void basic_cursor(uint8_t visible);

/**
 * @brief Set palette color (MSX2+)
 * Equivalent to: COLOR=(palette, r, g, b)
 * @param palette Palette number (0-15)
 * @param r Red component (0-7)
 * @param g Green component (0-7)
 * @param b Blue component (0-7)
 */
void basic_set_palette(uint8_t palette, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Get current screen mode
 * @return Current screen mode
 */
uint8_t basic_get_screen_mode(void);

/**
 * @brief Print with TAB positioning
 * Equivalent to: PRINT TAB(n); ...
 * Moves cursor to column n (1-based)
 * @param n Column position (1-based)
 */
void basic_tab(uint8_t n);

/**
 * @brief Print spaces
 * Equivalent to: PRINT SPC(n); ...
 * @param n Number of spaces to print
 */
void basic_spc(uint8_t n);

/**
 * @brief Get character at screen position (text mode)
 * Equivalent to: SCREEN$(x, y)
 * @param x Column (0-based)
 * @param y Row (0-based)
 * @return Character code at position
 */
uint8_t basic_screen_char(uint8_t x, uint8_t y);

/**
 * @brief Get character and attribute at screen position
 * @param x Column (0-based)
 * @param y Row (0-based)
 * @param attr Pointer to store attribute (color)
 * @return Character code at position
 */
uint8_t basic_screen_char_attr(uint8_t x, uint8_t y, uint8_t* attr);

/**
 * @brief Print formatted string (PRINT USING)
 * Equivalent to: PRINT USING format$; value
 * Format characters:
 *   #  - Digit placeholder
 *   .  - Decimal point
 *   +  - Show sign
 *   -  - Trailing minus for negative
 *   ,  - Thousands separator
 * @param dest Destination buffer
 * @param format Format string
 * @param value Value to format
 */
void basic_print_using_int(char* dest, const char* format, int32_t value);

/**
 * @brief Print formatted float (PRINT USING)
 * @param dest Destination buffer
 * @param format Format string
 * @param value Value to format
 */
void basic_print_using_float(char* dest, const char* format, float value);

/**
 * @brief Print formatted string (PRINT USING)
 * Format characters:
 *   !  - First character only
 *   &  - Full string
 *   \\ spaces \\ - Fixed width
 * @param dest Destination buffer
 * @param format Format string
 * @param value String value to format
 */
void basic_print_using_str(char* dest, const char* format, const char* value);

/**
 * @brief Wait for specific port value
 * Equivalent to: WAIT port, and_mask, xor_mask
 * Waits until (INP(port) XOR xor_mask) AND and_mask is non-zero
 * @param port I/O port address
 * @param and_mask AND mask
 * @param xor_mask XOR mask (optional, 0 = no XOR)
 */
void basic_wait(uint16_t port, uint8_t and_mask, uint8_t xor_mask);

#endif /* MSXBASIC_SCREEN_H */
