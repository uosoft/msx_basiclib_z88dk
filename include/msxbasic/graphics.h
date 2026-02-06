/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file graphics.h
 * @brief MSX BASIC graphics functions
 *
 * Provides functions equivalent to MSX BASIC's:
 * PSET, PRESET, LINE, CIRCLE, PAINT, DRAW, POINT, COPY, SPRITE
 */

#ifndef MSXBASIC_GRAPHICS_H
#define MSXBASIC_GRAPHICS_H

#include <stdint.h>

/* Line style constants */
#define LINE_STYLE_NORMAL   0   /* Solid line */
#define LINE_STYLE_BOX      1   /* Box outline (B) */
#define LINE_STYLE_BOXFILL  2   /* Filled box (BF) */

/* Circle drawing constants */
#define CIRCLE_FULL         0   /* Full circle */

/**
 * @brief Initialize SCREEN 2 graphics color table
 * Call after basic_color() to set background color for entire screen
 */
void basic_init_grp(void);

/**
 * @brief Set a pixel at the specified coordinates
 * Equivalent to: PSET (x, y), color
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Color (0-15 or 0-255 depending on screen mode)
 */
void basic_pset(int16_t x, int16_t y, uint8_t color);

/**
 * @brief Set a pixel using current foreground color
 * Equivalent to: PSET (x, y)
 * @param x X coordinate
 * @param y Y coordinate
 */
void basic_pset_c(int16_t x, int16_t y);

/**
 * @brief Set a pixel using step (relative) coordinates
 * Equivalent to: PSET STEP(dx, dy), color
 * @param dx Relative X offset
 * @param dy Relative Y offset
 * @param color Color
 */
void basic_pset_step(int16_t dx, int16_t dy, uint8_t color);

/**
 * @brief Clear a pixel (set to background color)
 * Equivalent to: PRESET (x, y)
 * @param x X coordinate
 * @param y Y coordinate
 */
void basic_preset(int16_t x, int16_t y);

/**
 * @brief Draw a line between two points
 * Equivalent to: LINE (x1, y1)-(x2, y2), color
 * @param x1 Start X coordinate
 * @param y1 Start Y coordinate
 * @param x2 End X coordinate
 * @param y2 End Y coordinate
 * @param color Line color
 */
void basic_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);

/**
 * @brief Draw a line with style
 * Equivalent to: LINE (x1, y1)-(x2, y2), color, style
 * @param x1 Start X coordinate
 * @param y1 Start Y coordinate
 * @param x2 End X coordinate
 * @param y2 End Y coordinate
 * @param color Line color
 * @param style LINE_STYLE_NORMAL, LINE_STYLE_BOX, or LINE_STYLE_BOXFILL
 */
void basic_line_ex(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color, uint8_t style);

/**
 * @brief Draw a box outline
 * Equivalent to: LINE (x1, y1)-(x2, y2), color, B
 * @param x1 Top-left X coordinate
 * @param y1 Top-left Y coordinate
 * @param x2 Bottom-right X coordinate
 * @param y2 Bottom-right Y coordinate
 * @param color Box color
 */
void basic_box(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);

/**
 * @brief Draw a filled box
 * Equivalent to: LINE (x1, y1)-(x2, y2), color, BF
 * @param x1 Top-left X coordinate
 * @param y1 Top-left Y coordinate
 * @param x2 Bottom-right X coordinate
 * @param y2 Bottom-right Y coordinate
 * @param color Fill color
 */
void basic_boxfill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color);

/**
 * @brief Draw a circle
 * Equivalent to: CIRCLE (x, y), radius, color
 * @param x Center X coordinate
 * @param y Center Y coordinate
 * @param radius Radius
 * @param color Circle color
 */
void basic_circle(int16_t x, int16_t y, int16_t radius, uint8_t color);

/**
 * @brief Draw a circle/ellipse/arc with extended parameters
 * Equivalent to: CIRCLE (x, y), radius, color, start, end, aspect
 * @param x Center X coordinate
 * @param y Center Y coordinate
 * @param radius Radius
 * @param color Circle color
 * @param start_deg Start angle in degrees (0-360, ignored in current impl)
 * @param end_deg End angle in degrees (0-360, ignored in current impl)
 * @param aspect_100 Aspect ratio * 100 (100 = circle, ignored in current impl)
 */
void basic_circle_ex(int16_t x, int16_t y, int16_t radius, uint8_t color,
                     int16_t start_deg, int16_t end_deg, int16_t aspect_100);

/**
 * @brief Draw an ellipse
 * @param x Center X coordinate
 * @param y Center Y coordinate
 * @param rx Horizontal radius
 * @param ry Vertical radius
 * @param color Ellipse color
 */
void basic_ellipse(int16_t x, int16_t y, int16_t rx, int16_t ry, uint8_t color);

/**
 * @brief Paint (flood fill) an area
 * Equivalent to: PAINT (x, y), color, border
 * @param x Start X coordinate
 * @param y Start Y coordinate
 * @param color Fill color
 * @param border Border color (fill stops at this color)
 */
void basic_paint(int16_t x, int16_t y, uint8_t color, uint8_t border);

/**
 * @brief Paint with fill color only
 * Equivalent to: PAINT (x, y), color
 * @param x Start X coordinate
 * @param y Start Y coordinate
 * @param color Fill color (also used as border color)
 */
void basic_paint_c(int16_t x, int16_t y, uint8_t color);

/**
 * @brief Execute DRAW command string
 * Equivalent to: DRAW command$
 * Supported commands: U, D, L, R, E, F, G, H (movement)
 *                    M (move to), B (pen up), N (no update), C (color)
 *                    A (angle), S (scale)
 * @param cmd DRAW command string (null-terminated)
 */
void basic_draw(const char* cmd);

/**
 * @brief Get the color of a pixel
 * Equivalent to: POINT(x, y)
 * @param x X coordinate
 * @param y Y coordinate
 * @return Color of the pixel
 */
uint8_t basic_point(int16_t x, int16_t y);

/**
 * @brief Get current graphics X position
 * @return Current X position
 */
int16_t basic_grp_x(void);

/**
 * @brief Get current graphics Y position
 * @return Current Y position
 */
int16_t basic_grp_y(void);

/**
 * @brief Set current graphics position
 * @param x X coordinate
 * @param y Y coordinate
 */
void basic_grp_move(int16_t x, int16_t y);

/* === Sprite functions === */

/**
 * @brief Set sprite size
 * Equivalent to: SCREEN mode,size
 * @param size 0 = 8x8 pixels, 1 = 16x16 pixels (magnified)
 *             2 = 8x8 magnified, 3 = 16x16 magnified
 */
void basic_sprite_size(uint8_t size);

/**
 * @brief Define a sprite pattern
 * Equivalent to: SPRITE$(n) = pattern$
 * @param pattern_num Sprite pattern number (0-255 for 8x8, 0-63 for 16x16)
 * @param pattern Pointer to pattern data (8 bytes for 8x8, 32 bytes for 16x16)
 */
void basic_sprite_pattern(uint8_t pattern_num, const uint8_t* pattern);

/**
 * @brief Set sprite attributes
 * Equivalent to: PUT SPRITE n, (x, y), color, pattern
 * @param sprite_num Sprite plane number (0-31)
 * @param x X position
 * @param y Y position
 * @param color Sprite color (0-15)
 * @param pattern Pattern number
 */
void basic_put_sprite(uint8_t sprite_num, int16_t x, int16_t y, uint8_t color, uint8_t pattern);

/**
 * @brief Disable/hide a sprite
 * @param sprite_num Sprite plane number (0-31)
 */
void basic_sprite_off(uint8_t sprite_num);

/**
 * @brief Disable all sprites
 */
void basic_sprites_off(void);

/**
 * @brief Check sprite collision
 * Equivalent to: SPRITE ON / collision flag
 * @return Sprite collision flag
 */
uint8_t basic_sprite_collision(void);

/* === COPY commands (MSX2) === */

/**
 * @brief Copy screen area (MSX2)
 * Equivalent to: COPY (x1,y1)-(x2,y2) TO (dx,dy)
 * @param sx Source X
 * @param sy Source Y
 * @param width Width of area
 * @param height Height of area
 * @param dx Destination X
 * @param dy Destination Y
 */
void basic_copy(int16_t sx, int16_t sy, uint16_t width, uint16_t height,
                int16_t dx, int16_t dy);

/**
 * @brief Copy between pages (MSX2)
 * Equivalent to: COPY (x1,y1)-(x2,y2),src_page TO (dx,dy),dst_page
 * @param sx Source X
 * @param sy Source Y
 * @param width Width of area
 * @param height Height of area
 * @param src_page Source page
 * @param dx Destination X
 * @param dy Destination Y
 * @param dst_page Destination page
 */
void basic_copy_page(int16_t sx, int16_t sy, uint16_t width, uint16_t height,
                     uint8_t src_page, int16_t dx, int16_t dy, uint8_t dst_page);

/**
 * @brief Set display page (MSX2)
 * Equivalent to: SET PAGE display, active
 * @param display_page Page to display (0-3)
 * @param active_page Page for drawing (0-3)
 */
void basic_set_page(uint8_t display_page, uint8_t active_page);

/* === GET/PUT Graphics Block operations === */

/**
 * @brief Calculate buffer size needed for GET operation
 * @param width Width of area
 * @param height Height of area
 * @return Required buffer size in bytes
 */
uint16_t basic_get_size(uint16_t width, uint16_t height);

/**
 * @brief Save screen area to buffer
 * Equivalent to: GET (x1,y1)-(x2,y2), array
 * @param x1 Left X coordinate
 * @param y1 Top Y coordinate
 * @param x2 Right X coordinate
 * @param y2 Bottom Y coordinate
 * @param buffer Buffer to store image data
 * @return Bytes written to buffer
 */
uint16_t basic_get(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t* buffer);

/**
 * @brief Restore screen area from buffer
 * Equivalent to: PUT (x,y), array, operation
 * @param x Left X coordinate
 * @param y Top Y coordinate
 * @param buffer Buffer containing image data
 * @param op Operation mode: 0=PSET, 1=AND, 2=OR, 3=XOR, 4=PRESET
 */
void basic_put(int16_t x, int16_t y, const uint8_t* buffer, uint8_t op);

/* PUT operation modes */
#define PUT_PSET    0   /* Overwrite */
#define PUT_AND     1   /* AND operation */
#define PUT_OR      2   /* OR operation */
#define PUT_XOR     3   /* XOR operation */
#define PUT_PRESET  4   /* Inverted overwrite */

/**
 * @brief Draw filled circle
 * @param x Center X coordinate
 * @param y Center Y coordinate
 * @param radius Radius
 * @param color Fill color
 */
void basic_circle_fill(int16_t x, int16_t y, int16_t radius, uint8_t color);

/**
 * @brief Draw filled ellipse
 * @param x Center X coordinate
 * @param y Center Y coordinate
 * @param rx Horizontal radius
 * @param ry Vertical radius
 * @param color Fill color
 */
void basic_ellipse_fill(int16_t x, int16_t y, int16_t rx, int16_t ry, uint8_t color);

#endif /* MSXBASIC_GRAPHICS_H */
