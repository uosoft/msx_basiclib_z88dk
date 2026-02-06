/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file bstring.h
 * @brief MSX BASIC string functions
 *
 * Provides functions equivalent to MSX BASIC's:
 * LEN, LEFT$, RIGHT$, MID$, ASC, CHR$, STR$, VAL, HEX$, BIN$, OCT$,
 * INSTR, STRING$, SPACE$
 *
 * Note: Named bstring.h to avoid conflict with standard string.h
 */

#ifndef MSXBASIC_STRING_H
#define MSXBASIC_STRING_H

#include <stdint.h>

/* Maximum string buffer size for static allocations */
#define BASIC_STRING_BUFFER_SIZE    256

/**
 * @brief Get length of a string
 * Equivalent to: LEN(string$)
 * @param s Input string
 * @return Length of string
 */
uint8_t basic_len(const char* s);

/**
 * @brief Extract left portion of a string
 * Equivalent to: LEFT$(string$, n)
 * @param dest Destination buffer
 * @param src Source string
 * @param n Number of characters from left
 */
void basic_left(char* dest, const char* src, uint8_t n);

/**
 * @brief Extract right portion of a string
 * Equivalent to: RIGHT$(string$, n)
 * @param dest Destination buffer
 * @param src Source string
 * @param n Number of characters from right
 */
void basic_right(char* dest, const char* src, uint8_t n);

/**
 * @brief Extract middle portion of a string
 * Equivalent to: MID$(string$, start, length)
 * @param dest Destination buffer
 * @param src Source string
 * @param start Start position (1-based, as in BASIC)
 * @param length Number of characters to extract
 */
void basic_mid(char* dest, const char* src, uint8_t start, uint8_t length);

/**
 * @brief Replace portion of a string (MID$ statement)
 * Equivalent to: MID$(dest$, start, length) = src$
 * @param dest Destination string to modify
 * @param start Start position (1-based)
 * @param length Maximum length to replace
 * @param src Source string
 */
void basic_mid_replace(char* dest, uint8_t start, uint8_t length, const char* src);

/**
 * @brief Get ASCII code of first character
 * Equivalent to: ASC(string$)
 * @param s Input string
 * @return ASCII code of first character (0 if empty)
 */
uint8_t basic_asc(const char* s);

/**
 * @brief Create string from character code
 * Equivalent to: CHR$(code)
 * @param dest Destination buffer (needs 2 bytes)
 * @param code Character code
 */
void basic_chr(char* dest, uint8_t code);

/**
 * @brief Convert integer to string
 * Equivalent to: STR$(number)
 * @param dest Destination buffer
 * @param n Integer value
 */
void basic_str(char* dest, int16_t n);

/**
 * @brief Convert long integer to string
 * @param dest Destination buffer
 * @param n Long integer value
 */
void basic_str_long(char* dest, int32_t n);

/**
 * @brief Convert float to string
 * @param dest Destination buffer
 * @param n Float value
 */
void basic_str_float(char* dest, float n);

/**
 * @brief Convert string to integer
 * Equivalent to: VAL(string$)
 * @param s Input string
 * @return Integer value
 */
int16_t basic_val(const char* s);

/**
 * @brief Convert string to long integer
 * @param s Input string
 * @return Long integer value
 */
int32_t basic_val_long(const char* s);

/**
 * @brief Convert string to float
 * @param s Input string
 * @return Float value
 */
float basic_val_float(const char* s);

/**
 * @brief Convert integer to hexadecimal string
 * Equivalent to: HEX$(number)
 * @param dest Destination buffer
 * @param n Integer value
 */
void basic_hex(char* dest, uint16_t n);

/**
 * @brief Convert integer to binary string
 * Equivalent to: BIN$(number)
 * @param dest Destination buffer
 * @param n Integer value
 */
void basic_bin(char* dest, uint16_t n);

/**
 * @brief Convert integer to octal string
 * Equivalent to: OCT$(number)
 * @param dest Destination buffer
 * @param n Integer value
 */
void basic_oct(char* dest, uint16_t n);

/**
 * @brief Find position of substring
 * Equivalent to: INSTR(string$, search$) or INSTR(start, string$, search$)
 * @param s Main string
 * @param search Substring to find
 * @param start Start position (1-based, 1 = from beginning)
 * @return Position of first match (1-based), or 0 if not found
 */
uint8_t basic_instr(const char* s, const char* search, uint8_t start);

/**
 * @brief Create string of repeated characters
 * Equivalent to: STRING$(n, code) or STRING$(n, string$)
 * @param dest Destination buffer
 * @param n Number of repetitions
 * @param code Character code to repeat
 */
void basic_string(char* dest, uint8_t n, uint8_t code);

/**
 * @brief Create string of repeated substring
 * @param dest Destination buffer
 * @param n Number of repetitions
 * @param s String to repeat
 */
void basic_string_str(char* dest, uint8_t n, const char* s);

/**
 * @brief Create string of spaces
 * Equivalent to: SPACE$(n)
 * @param dest Destination buffer
 * @param n Number of spaces
 */
void basic_space(char* dest, uint8_t n);

/**
 * @brief Convert string to uppercase
 * @param s String to convert (modified in place)
 */
void basic_ucase(char* s);

/**
 * @brief Convert string to lowercase
 * @param s String to convert (modified in place)
 */
void basic_lcase(char* s);

/**
 * @brief Compare two strings
 * @param s1 First string
 * @param s2 Second string
 * @return <0 if s1<s2, 0 if equal, >0 if s1>s2
 */
int8_t basic_strcmp(const char* s1, const char* s2);

/**
 * @brief Concatenate strings
 * @param dest Destination buffer
 * @param s1 First string
 * @param s2 Second string
 */
void basic_concat(char* dest, const char* s1, const char* s2);

/* === Kanji functions (Japanese MSX) === */

/**
 * @brief Convert JIS to Shift-JIS
 * @param dest Destination buffer
 * @param src JIS string
 */
void basic_jis_to_sjis(char* dest, const char* src);

/**
 * @brief Convert Shift-JIS to JIS
 * @param dest Destination buffer
 * @param src Shift-JIS string
 */
void basic_sjis_to_jis(char* dest, const char* src);

#endif /* MSXBASIC_STRING_H */
