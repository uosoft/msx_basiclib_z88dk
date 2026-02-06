/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file input.h
 * @brief MSX BASIC input functions
 *
 * Provides functions equivalent to MSX BASIC's:
 * INKEY$, INPUT$, STICK, STRIG, PADDLE, PEEK, POKE, INP, OUT
 */

#ifndef MSXBASIC_INPUT_H
#define MSXBASIC_INPUT_H

#include <stdint.h>

/* STICK direction constants */
#define STICK_CENTER    0
#define STICK_UP        1
#define STICK_UP_RIGHT  2
#define STICK_RIGHT     3
#define STICK_DOWN_RIGHT 4
#define STICK_DOWN      5
#define STICK_DOWN_LEFT 6
#define STICK_LEFT      7
#define STICK_UP_LEFT   8

/* Joystick/keyboard selection */
#define STICK_KEYBOARD  0   /* Cursor keys */
#define STICK_JOY1      1   /* Joystick port 1 */
#define STICK_JOY2      2   /* Joystick port 2 */

/* STRIG button selection */
#define STRIG_SPACE     0   /* Space bar */
#define STRIG_JOY1_A    1   /* Joystick 1, button A */
#define STRIG_JOY2_A    2   /* Joystick 2, button A */
#define STRIG_JOY1_B    3   /* Joystick 1, button B */
#define STRIG_JOY2_B    4   /* Joystick 2, button B */

/**
 * @brief Get a key from keyboard buffer (non-blocking)
 * Equivalent to: INKEY$
 * @return Character code of pressed key, or 0 if no key pressed
 */
char basic_inkey(void);

/**
 * @brief Wait and read specified number of characters
 * Equivalent to: INPUT$(n)
 * @param buffer Buffer to store input (must have space for n+1 chars)
 * @param n Number of characters to read
 */
void basic_input_str(char* buffer, uint8_t n);

/**
 * @brief Read a line of input from keyboard
 * Equivalent to: INPUT a$
 * @param buffer Buffer to store input (must be large enough)
 * @param max_len Maximum input length
 */
void basic_input_line(char* buffer, uint8_t max_len);

/**
 * @brief Read joystick/cursor key direction
 * Equivalent to: STICK(n)
 * @param device 0=cursor keys, 1=joystick 1, 2=joystick 2
 * @return Direction (0-8, see STICK_* constants)
 */
uint8_t basic_stick(uint8_t device);

/**
 * @brief Read trigger/button state
 * Equivalent to: STRIG(n)
 * @param button Button to check (see STRIG_* constants)
 * @return 0 if not pressed, -1 (0xFF) if pressed (BASIC convention)
 */
int8_t basic_strig(uint8_t button);

/**
 * @brief Read trigger state (returns 0 or 1)
 * @param button Button to check
 * @return 0 if not pressed, 1 if pressed
 */
uint8_t basic_strig_bool(uint8_t button);

/**
 * @brief Read paddle position
 * Equivalent to: PADDLE(n)
 * @param paddle Paddle number (1-12)
 * @return Paddle position (0-255)
 */
uint8_t basic_paddle(uint8_t paddle);

/**
 * @brief Read paddle button state
 * Equivalent to: PDL(n)
 * @param paddle Paddle number
 * @return Button state
 */
uint8_t basic_pdl(uint8_t paddle);

/**
 * @brief Read a byte from memory
 * Equivalent to: PEEK(address)
 * @param address Memory address
 * @return Byte value at address
 */
uint8_t basic_peek(uint16_t address);

/**
 * @brief Write a byte to memory
 * Equivalent to: POKE address, value
 * @param address Memory address
 * @param value Byte value to write
 */
void basic_poke(uint16_t address, uint8_t value);

/**
 * @brief Read a 16-bit word from memory
 * @param address Memory address
 * @return 16-bit value (little-endian)
 */
uint16_t basic_peek16(uint16_t address);

/**
 * @brief Write a 16-bit word to memory
 * @param address Memory address
 * @param value 16-bit value to write (little-endian)
 */
void basic_poke16(uint16_t address, uint16_t value);

/**
 * @brief Read from I/O port
 * Equivalent to: INP(port)
 * @param port I/O port address
 * @return Byte read from port
 */
uint8_t basic_inp(uint16_t port);

/**
 * @brief Write to I/O port
 * Equivalent to: OUT port, value
 * @param port I/O port address
 * @param value Byte to write
 */
void basic_out(uint16_t port, uint8_t value);

/**
 * @brief Wait for a key press
 * @return Character code of pressed key
 */
char basic_wait_key(void);

/**
 * @brief Check if a specific key is pressed
 * @param row Keyboard matrix row (0-10)
 * @param bit Keyboard matrix bit (0-7)
 * @return 1 if pressed, 0 if not
 */
uint8_t basic_key_pressed(uint8_t row, uint8_t bit);

/**
 * @brief Clear keyboard buffer
 */
void basic_clear_key_buffer(void);

/**
 * @brief Enable/disable key click sound
 * @param enable 0=off, 1=on
 */
void basic_key_click(uint8_t enable);

/**
 * @brief Define function key string
 * Equivalent to: KEY n, string$
 * @param key_num Function key number (1-10)
 * @param str String to assign
 */
void basic_key_def(uint8_t key_num, const char* str);

/**
 * @brief Show/hide function key display
 * Equivalent to: KEY ON / KEY OFF
 * @param show 0=hide, 1=show
 */
void basic_key_display(uint8_t show);

/* === Mouse functions (MSX2 with mouse) === */

/**
 * @brief Read mouse X movement
 * @param port Mouse port (0 or 1)
 * @return X movement since last read
 */
int8_t basic_mouse_x(uint8_t port);

/**
 * @brief Read mouse Y movement
 * @param port Mouse port (0 or 1)
 * @return Y movement since last read
 */
int8_t basic_mouse_y(uint8_t port);

/**
 * @brief Read mouse button state
 * @param port Mouse port (0 or 1)
 * @return Button state (bit 0=left, bit 1=right)
 */
uint8_t basic_mouse_button(uint8_t port);

/**
 * @brief Check if mouse is connected
 * @param port Mouse port (0 or 1)
 * @return 1 if mouse connected, 0 otherwise
 */
uint8_t basic_mouse_check(uint8_t port);

/* === Touchpad functions (MSX2 with touchpad) === */

/**
 * @brief Read touchpad X position
 * @return X position (0-255)
 */
uint8_t basic_touch_x(void);

/**
 * @brief Read touchpad Y position
 * @return Y position (0-255)
 */
uint8_t basic_touch_y(void);

/**
 * @brief Check touchpad status
 * @return 1 if touched, 0 if not
 */
uint8_t basic_touch_status(void);

/* === Light pen functions === */

/**
 * @brief Read light pen X position
 * @return X position
 */
uint16_t basic_pen_x(void);

/**
 * @brief Read light pen Y position
 * @return Y position
 */
uint16_t basic_pen_y(void);

/**
 * @brief Check light pen status
 * @return 1 if pen detected, 0 if not
 */
uint8_t basic_pen_status(void);

#endif /* MSXBASIC_INPUT_H */
