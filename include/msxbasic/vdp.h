/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file vdp.h
 * @brief MSX VDP (Video Display Processor) functions
 *
 * Supports TMS9918 (MSX1), V9938 (MSX2), V9958 (MSX2+/turboR)
 */

#ifndef MSXBASIC_VDP_H
#define MSXBASIC_VDP_H

#include <stdint.h>

/* MSX version detection */
#define MSX_VERSION     (*(volatile uint8_t*)0x002D)
#define IS_MSX1()       (MSX_VERSION == 0)
#define IS_MSX2()       (MSX_VERSION >= 1)
#define IS_MSX2PLUS()   (MSX_VERSION >= 2)
#define IS_TURBOR()     (MSX_VERSION >= 3)

/* Screen mode constants */
#define SCREEN_MODE_0   0   /* TEXT1: 40x24 text */
#define SCREEN_MODE_1   1   /* GRAPHIC1: 32x24 text with sprites */
#define SCREEN_MODE_2   2   /* GRAPHIC2: 256x192, 16 colors (MSX1) */
#define SCREEN_MODE_3   3   /* MULTI: 64x48 multicolor */
#define SCREEN_MODE_4   4   /* GRAPHIC3: 256x192, sprite mode 2 (MSX2) */
#define SCREEN_MODE_5   5   /* GRAPHIC4: 256x212, 16 colors (MSX2) */
#define SCREEN_MODE_6   6   /* GRAPHIC5: 512x212, 4 colors (MSX2) */
#define SCREEN_MODE_7   7   /* GRAPHIC6: 512x212, 16 colors (MSX2) */
#define SCREEN_MODE_8   8   /* GRAPHIC7: 256x212, 256 colors (MSX2) */
#define SCREEN_MODE_10  10  /* YJK+YAE: 256x212, 12499 colors (MSX2+) */
#define SCREEN_MODE_11  11  /* YJK: 256x212, 12499 colors (MSX2+) */
#define SCREEN_MODE_12  12  /* YJK: 256x212, 19268 colors (MSX2+) */

/* Screen resolution helpers */
#define SCREEN_WIDTH(mode)  (((mode) == 6 || (mode) == 7) ? 512 : 256)
#define SCREEN_HEIGHT(mode) (((mode) >= 5 && (mode) <= 12) ? 212 : 192)

/* VDP ports */
#define VDP_DATA        0x98
#define VDP_COMMAND     0x99
#define VDP_PALETTE     0x9A
#define VDP_REGISTER    0x9B

/* VDP status register bits */
#define VDP_STATUS_F    0x80    /* Vertical retrace flag */
#define VDP_STATUS_5S   0x40    /* 5th sprite flag */
#define VDP_STATUS_C    0x20    /* Sprite collision */
#define VDP_STATUS_CE   0x01    /* Command executing (MSX2) */

/* MSX2 VDP commands */
#define VDP_CMD_HMMC    0xF0    /* CPU -> VRAM (high speed) */
#define VDP_CMD_YMMM    0xE0    /* VRAM -> VRAM (Y direction) */
#define VDP_CMD_HMMM    0xD0    /* VRAM -> VRAM (high speed) */
#define VDP_CMD_HMMV    0xC0    /* VDP -> VRAM (fill, high speed) */
#define VDP_CMD_LMMC    0xB0    /* CPU -> VRAM (logical) */
#define VDP_CMD_LMCM    0xA0    /* VRAM -> CPU (logical) */
#define VDP_CMD_LMMM    0x90    /* VRAM -> VRAM (logical) */
#define VDP_CMD_LMMV    0x80    /* VDP -> VRAM (fill, logical) */
#define VDP_CMD_LINE    0x70    /* Draw line */
#define VDP_CMD_SRCH    0x60    /* Search color */
#define VDP_CMD_PSET    0x50    /* Set pixel */
#define VDP_CMD_POINT   0x40    /* Get pixel */
#define VDP_CMD_STOP    0x00    /* Stop command */

/* Logical operations for VDP commands */
#define VDP_LOG_IMP     0x00    /* DC = SC */
#define VDP_LOG_AND     0x01    /* DC = SC AND DC */
#define VDP_LOG_OR      0x02    /* DC = SC OR DC */
#define VDP_LOG_XOR     0x03    /* DC = SC XOR DC */
#define VDP_LOG_NOT     0x04    /* DC = NOT SC */
#define VDP_LOG_TIMP    0x08    /* Transparent IMP */
#define VDP_LOG_TAND    0x09    /* Transparent AND */
#define VDP_LOG_TOR     0x0A    /* Transparent OR */
#define VDP_LOG_TXOR    0x0B    /* Transparent XOR */
#define VDP_LOG_TNOT    0x0C    /* Transparent NOT */

/**
 * @brief Write to VDP register
 * @param reg Register number
 * @param value Value to write
 */
void vdp_write_reg(uint8_t reg, uint8_t value);

/**
 * @brief Read VDP status register
 * @param reg Status register number (0-9 for MSX2)
 * @return Status value
 */
uint8_t vdp_read_status(uint8_t reg);

/**
 * @brief Wait for VDP command to complete
 */
void vdp_wait_cmd(void);

/**
 * @brief Set VRAM write address (MSX2, 128KB VRAM)
 * @param addr 17-bit VRAM address
 */
void vdp_set_write_addr(uint32_t addr);

/**
 * @brief Set VRAM read address (MSX2, 128KB VRAM)
 * @param addr 17-bit VRAM address
 */
void vdp_set_read_addr(uint32_t addr);

/**
 * @brief Write byte to VRAM (after setting address)
 * @param value Byte to write
 */
void vdp_write_vram(uint8_t value);

/**
 * @brief Read byte from VRAM (after setting address)
 * @return Byte read
 */
uint8_t vdp_read_vram(void);

/**
 * @brief MSX2 VDP PSET command
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Color
 * @param op Logical operation
 */
void vdp_pset(uint16_t x, uint16_t y, uint8_t color, uint8_t op);

/**
 * @brief MSX2 VDP LINE command
 * @param x1 Start X
 * @param y1 Start Y
 * @param x2 End X
 * @param y2 End Y
 * @param color Color
 * @param op Logical operation
 */
void vdp_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color, uint8_t op);

/**
 * @brief MSX2 VDP HMMV command (fill rectangle)
 * @param x X coordinate
 * @param y Y coordinate
 * @param width Width
 * @param height Height
 * @param color Color
 */
void vdp_fill(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color);

/**
 * @brief MSX2 VDP HMMM command (copy rectangle)
 * @param sx Source X
 * @param sy Source Y
 * @param dx Destination X
 * @param dy Destination Y
 * @param width Width
 * @param height Height
 */
void vdp_copy(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint16_t width, uint16_t height);

/**
 * @brief Set palette color (MSX2)
 * @param index Palette index (0-15)
 * @param r Red (0-7)
 * @param g Green (0-7)
 * @param b Blue (0-7)
 */
void vdp_set_palette(uint8_t index, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Set display page (MSX2)
 * @param page Page number
 */
void vdp_set_display_page(uint8_t page);

/**
 * @brief Set active (drawing) page (MSX2)
 * @param page Page number
 */
void vdp_set_active_page(uint8_t page);

/**
 * @brief Get current screen mode
 * @return Screen mode (0-12)
 */
uint8_t vdp_get_mode(void);

#endif /* MSXBASIC_VDP_H */
