/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file bstring.c
 * @brief MSX BASIC string functions (Z88DK compatible)
 */

#include <stdint.h>
#include "../../include/msxbasic/bstring.h"

uint8_t basic_len(const char* s) {
    uint8_t len = 0;
    while (*s++) len++;
    return len;
}

void basic_left(char* dest, const char* src, uint8_t n) {
    while (n > 0 && *src) { *dest++ = *src++; n--; }
    *dest = '\0';
}

void basic_right(char* dest, const char* src, uint8_t n) {
    uint8_t len = basic_len(src);
    if (n >= len) {
        while (*src) *dest++ = *src++;
    } else {
        src += len - n;
        while (*src) *dest++ = *src++;
    }
    *dest = '\0';
}

void basic_mid(char* dest, const char* src, uint8_t start, uint8_t length) {
    if (start == 0) start = 1;
    start--;
    while (start > 0 && *src) { src++; start--; }
    while (length > 0 && *src) { *dest++ = *src++; length--; }
    *dest = '\0';
}

void basic_mid_replace(char* dest, uint8_t start, uint8_t length, const char* src) {
    if (start == 0) start = 1;
    dest += start - 1;
    while (length > 0 && *src && *dest) { *dest++ = *src++; length--; }
}

uint8_t basic_asc(const char* s) { return *s; }

void basic_chr(char* dest, uint8_t code) {
    dest[0] = code;
    dest[1] = '\0';
}

void basic_str(char* dest, int16_t n) {
    char buffer[8];
    char* p = buffer + 7;
    uint16_t un;
    uint8_t neg = 0;
    *p = '\0';
    if (n < 0) { neg = 1; un = -n; } else { un = n; }
    if (un == 0) { *--p = '0'; }
    else { while (un > 0) { *--p = '0' + (un % 10); un /= 10; } }
    if (neg) { *--p = '-'; }
    while (*p) *dest++ = *p++;
    *dest = '\0';
}

void basic_str_long(char* dest, int32_t n) {
    char buffer[12];
    char* p = buffer + 11;
    uint32_t un;
    uint8_t neg = 0;
    *p = '\0';
    if (n < 0) { neg = 1; un = -n; } else { un = n; }
    if (un == 0) { *--p = '0'; }
    else { while (un > 0) { *--p = '0' + (un % 10); un /= 10; } }
    if (neg) { *--p = '-'; }
    while (*p) *dest++ = *p++;
    *dest = '\0';
}

void basic_str_float(char* dest, float n) {
    char* p = dest;
    int32_t integer_part;
    float frac_part;
    int i;
    int precision = 6;  /* Default precision for MSX BASIC */

    /* Handle negative numbers */
    if (n < 0) {
        *p++ = '-';
        n = -n;
    }

    /* Get integer and fractional parts */
    integer_part = (int32_t)n;
    frac_part = n - (float)integer_part;

    /* Convert integer part */
    {
        char int_buf[12];
        char* ip = int_buf + 11;
        *ip = '\0';
        if (integer_part == 0) {
            *--ip = '0';
        } else {
            while (integer_part > 0) {
                *--ip = '0' + (integer_part % 10);
                integer_part /= 10;
            }
        }
        while (*ip) *p++ = *ip++;
    }

    /* Add decimal point and fractional part */
    if (frac_part > 0.0000001f) {
        *p++ = '.';

        /* Convert fractional part */
        for (i = 0; i < precision && frac_part > 0.0000001f; i++) {
            frac_part *= 10.0f;
            int digit = (int)frac_part;
            *p++ = '0' + digit;
            frac_part -= (float)digit;
        }

        /* Remove trailing zeros */
        while (p > dest + 2 && *(p - 1) == '0') p--;
        if (*(p - 1) == '.') p--;  /* Remove trailing decimal point */
    }

    *p = '\0';
}

int16_t basic_val(const char* s) {
    int16_t result = 0;
    int8_t sign = 1;
    while (*s == ' ') s++;
    if (*s == '-') { sign = -1; s++; }
    else if (*s == '+') s++;
    while (*s >= '0' && *s <= '9') { result = result * 10 + (*s - '0'); s++; }
    return result * sign;
}

int32_t basic_val_long(const char* s) {
    int32_t result = 0;
    int8_t sign = 1;
    while (*s == ' ') s++;
    if (*s == '-') { sign = -1; s++; }
    else if (*s == '+') s++;
    while (*s >= '0' && *s <= '9') { result = result * 10 + (*s - '0'); s++; }
    return result * sign;
}

float basic_val_float(const char* s) {
    float result = 0.0f;
    float fraction = 0.0f;
    float divisor = 10.0f;
    int8_t sign = 1;
    int8_t exp_sign = 1;
    int16_t exponent = 0;

    /* Skip leading whitespace */
    while (*s == ' ') s++;

    /* Handle sign */
    if (*s == '-') { sign = -1; s++; }
    else if (*s == '+') s++;

    /* Parse integer part */
    while (*s >= '0' && *s <= '9') {
        result = result * 10.0f + (float)(*s - '0');
        s++;
    }

    /* Parse decimal part */
    if (*s == '.') {
        s++;
        while (*s >= '0' && *s <= '9') {
            fraction += (float)(*s - '0') / divisor;
            divisor *= 10.0f;
            s++;
        }
        result += fraction;
    }

    /* Parse exponent (E or D notation) */
    if (*s == 'E' || *s == 'e' || *s == 'D' || *s == 'd') {
        s++;
        if (*s == '-') { exp_sign = -1; s++; }
        else if (*s == '+') s++;

        while (*s >= '0' && *s <= '9') {
            exponent = exponent * 10 + (*s - '0');
            s++;
        }

        /* Apply exponent */
        exponent *= exp_sign;
        while (exponent > 0) {
            result *= 10.0f;
            exponent--;
        }
        while (exponent < 0) {
            result /= 10.0f;
            exponent++;
        }
    }

    return result * sign;
}

void basic_hex(char* dest, uint16_t n) {
    static const char hex[] = "0123456789ABCDEF";
    char buffer[5];
    char* p = buffer + 4;
    *p = '\0';
    if (n == 0) { *--p = '0'; }
    else { while (n > 0) { *--p = hex[n & 0x0F]; n >>= 4; } }
    while (*p) *dest++ = *p++;
    *dest = '\0';
}

void basic_bin(char* dest, uint16_t n) {
    char buffer[17];
    char* p = buffer + 16;
    *p = '\0';
    if (n == 0) { *--p = '0'; }
    else { while (n > 0) { *--p = (n & 1) ? '1' : '0'; n >>= 1; } }
    while (*p) *dest++ = *p++;
    *dest = '\0';
}

void basic_oct(char* dest, uint16_t n) {
    char buffer[7];
    char* p = buffer + 6;
    *p = '\0';
    if (n == 0) { *--p = '0'; }
    else { while (n > 0) { *--p = '0' + (n & 7); n >>= 3; } }
    while (*p) *dest++ = *p++;
    *dest = '\0';
}

uint8_t basic_instr(const char* s, const char* search, uint8_t start) {
    uint8_t pos = 1;
    const char* p, *q;
    if (start > 1) { while (start > 1 && *s) { s++; pos++; start--; } }
    while (*s) {
        p = s; q = search;
        while (*q && *p == *q) { p++; q++; }
        if (*q == '\0') return pos;
        s++; pos++;
    }
    return 0;
}

void basic_string(char* dest, uint8_t n, uint8_t code) {
    while (n > 0) { *dest++ = code; n--; }
    *dest = '\0';
}

void basic_string_str(char* dest, uint8_t n, const char* s) {
    while (n > 0) { const char* p = s; while (*p) *dest++ = *p++; n--; }
    *dest = '\0';
}

void basic_space(char* dest, uint8_t n) {
    basic_string(dest, n, ' ');
}

void basic_ucase(char* s) {
    while (*s) { if (*s >= 'a' && *s <= 'z') *s = *s - 32; s++; }
}

void basic_lcase(char* s) {
    while (*s) { if (*s >= 'A' && *s <= 'Z') *s = *s + 32; s++; }
}

int8_t basic_strcmp(const char* s1, const char* s2) {
    while (*s1 && *s2 && *s1 == *s2) { s1++; s2++; }
    return *s1 - *s2;
}

void basic_concat(char* dest, const char* s1, const char* s2) {
    while (*s1) *dest++ = *s1++;
    while (*s2) *dest++ = *s2++;
    *dest = '\0';
}

void basic_jis_to_sjis(char* dest, const char* src) {
    uint8_t c1, c2;
    uint8_t s1, s2;

    while (*src) {
        c1 = (uint8_t)*src++;

        /* Check for 2-byte JIS character (0x21-0x7E range) */
        if (c1 >= 0x21 && c1 <= 0x7E && *src) {
            c2 = (uint8_t)*src;
            if (c2 >= 0x21 && c2 <= 0x7E) {
                src++;  /* Consume second byte */

                /* Convert JIS to SJIS */
                /* JIS row (c1 - 0x21) maps to SJIS */
                if (c1 & 1) {
                    /* Odd row */
                    s2 = c2 + 0x1F;
                    if (c2 >= 0x60) s2++;
                } else {
                    /* Even row */
                    s2 = c2 + 0x7E;
                }

                if (c1 <= 0x5E) {
                    s1 = ((c1 - 0x21) >> 1) + 0x81;
                } else {
                    s1 = ((c1 - 0x5F) >> 1) + 0xE0;
                }

                *dest++ = (char)s1;
                *dest++ = (char)s2;
                continue;
            }
        }

        /* Single byte character - copy as is */
        *dest++ = (char)c1;
    }
    *dest = '\0';
}

void basic_sjis_to_jis(char* dest, const char* src) {
    uint8_t c1, c2;
    uint8_t j1, j2;

    while (*src) {
        c1 = (uint8_t)*src++;

        /* Check for SJIS 2-byte character */
        if (((c1 >= 0x81 && c1 <= 0x9F) || (c1 >= 0xE0 && c1 <= 0xEF)) && *src) {
            c2 = (uint8_t)*src;
            if ((c2 >= 0x40 && c2 <= 0x7E) || (c2 >= 0x80 && c2 <= 0xFC)) {
                src++;  /* Consume second byte */

                /* Convert SJIS to JIS */
                if (c1 >= 0xE0) {
                    c1 -= 0x40;
                }
                c1 -= 0x81;
                c1 *= 2;
                c1 += 0x21;

                if (c2 >= 0x80) c2--;
                if (c2 >= 0x9E) {
                    j1 = c1 + 1;
                    j2 = c2 - 0x7D;
                } else {
                    j1 = c1;
                    j2 = c2 - 0x1F;
                }

                *dest++ = (char)j1;
                *dest++ = (char)j2;
                continue;
            }
        }

        /* Single byte character - copy as is */
        *dest++ = (char)c1;
    }
    *dest = '\0';
}
