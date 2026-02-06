/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file msxbasic.h
 * @brief MSX BASIC compatible library for Z88DK
 *
 * This library provides C functions that emulate MSX BASIC commands and functions.
 * Compatible with MSX1, MSX2, MSX2+, and MSX turbo R.
 */

#ifndef MSXBASIC_H
#define MSXBASIC_H

#include <stdint.h>

/* Include all sub-modules */
#include "screen.h"
#include "graphics.h"
#include "sound.h"
#include "input.h"
#include "bstring.h"    /* Named bstring.h to avoid conflict with standard string.h */
#include "bmath.h"      /* Named bmath.h to avoid conflict with standard math.h */
#include "system.h"
#include "vdp.h"        /* VDP access functions (MSX2+) */

/* MSX system type detection */
#define MSX_TYPE_MSX1     0
#define MSX_TYPE_MSX2     1
#define MSX_TYPE_MSX2P    2
#define MSX_TYPE_TURBO_R  3

/**
 * @brief Get the MSX system type
 * @return MSX type constant (MSX_TYPE_MSX1, MSX_TYPE_MSX2, etc.)
 */
uint8_t basic_get_msx_type(void);

/**
 * @brief Check if the system is MSX2 or later
 * @return 1 if MSX2 or later, 0 otherwise
 */
uint8_t basic_is_msx2(void);

/**
 * @brief Check if the system is MSX2+ or later
 * @return 1 if MSX2+ or later, 0 otherwise
 */
uint8_t basic_is_msx2plus(void);

/**
 * @brief Check if the system is MSX turbo R
 * @return 1 if MSX turbo R, 0 otherwise
 */
uint8_t basic_is_turbo_r(void);

/**
 * @brief Initialize the BASIC library
 * This should be called at the start of your program.
 */
void basic_init(void);

#endif /* MSXBASIC_H */
