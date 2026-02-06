/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file system.c
 * @brief MSX BASIC system functions (Z88DK compatible)
 */

#include <stdint.h>
#include "../../include/msxbasic/system.h"

#define MSXVER 0x002D
#define sys_read8(addr)  (*(volatile uint8_t*)(addr))
#define sys_write8(addr, val) (*(volatile uint8_t*)(addr) = (val))

#asm

PUBLIC _sys_vpeek
PUBLIC _sys_vpoke
PUBLIC _sys_rdvdp
PUBLIC _sys_halt
PUBLIC _sys_filvrm
PUBLIC _sys_ldirvm
PUBLIC _sys_ldirmv
PUBLIC _sys_snsmat
PUBLIC _sys_calbas

_sys_vpeek:
    pop de
    pop hl
    push hl
    push de
    call 0x004A     ; RDVRM
    ld l, a
    ret

_sys_vpoke:
    pop de
    pop hl
    pop bc
    push bc
    push hl
    push de
    ld a, c
    call 0x004D     ; WRTVRM
    ret

_sys_rdvdp:
    call 0x013E     ; RDVDP
    ld l, a
    ret

_sys_halt:
    halt
    ret

; void sys_filvrm(uint16_t addr, uint16_t count, uint8_t value)
; sccz80 stack: [ret][value][count][addr]
; SP+2=value, SP+4=count, SP+6=addr
; FILVRM: HL=addr, BC=count, A=value
_sys_filvrm:
    ld hl, 2
    add hl, sp
    ld a, (hl)      ; value
    inc hl
    inc hl
    ld c, (hl)      ; count low
    inc hl
    ld b, (hl)      ; count high
    inc hl
    ld e, (hl)      ; addr low
    inc hl
    ld d, (hl)      ; addr high
    ex de, hl       ; HL = addr
    call 0x0056     ; FILVRM
    ret

; void sys_ldirvm(uint16_t dest, const uint8_t* src, uint16_t count)
; sccz80 stack: [ret][count][src][dest]
; SP+2=count, SP+4=src, SP+6=dest
; LDIRVM: HL=RAM src, DE=VRAM dest, BC=count
_sys_ldirvm:
    ld hl, 2
    add hl, sp
    ld c, (hl)      ; count low
    inc hl
    ld b, (hl)      ; count high
    inc hl
    ld e, (hl)      ; src low (RAM)
    inc hl
    ld d, (hl)      ; src high
    inc hl
    push de         ; save RAM src
    ld e, (hl)      ; dest low (VRAM)
    inc hl
    ld d, (hl)      ; dest high
    pop hl          ; HL = RAM src, DE = VRAM dest
    call 0x005C     ; LDIRVM
    ret

; void sys_ldirmv(uint8_t* dest, uint16_t src, uint16_t count)
; sccz80 stack: [ret][count][src][dest]
; SP+2=count, SP+4=src(VRAM), SP+6=dest(RAM)
; LDIRMV: HL=VRAM src, DE=RAM dest, BC=count
_sys_ldirmv:
    ld hl, 2
    add hl, sp
    ld c, (hl)      ; count low
    inc hl
    ld b, (hl)      ; count high
    inc hl
    ld e, (hl)      ; src low (VRAM)
    inc hl
    ld d, (hl)      ; src high
    inc hl
    push de         ; save VRAM src
    ld e, (hl)      ; dest low (RAM)
    inc hl
    ld d, (hl)      ; dest high
    pop hl          ; HL = VRAM src, DE = RAM dest
    call 0x0059     ; LDIRMV
    ret

; uint8_t sys_snsmat(uint8_t row)
; sccz80 stack: [ret][row]
; SP+2=row
; SNSMAT: A=row, returns keyboard matrix in A
_sys_snsmat:
    ld hl, 2
    add hl, sp
    ld a, (hl)      ; row
    call 0x0141     ; SNSMAT
    ld l, a
    ret

; void sys_calbas(uint16_t addr)
; sccz80 stack: [ret][addr]
; SP+2=addr
; CALBAS: IX=address
_sys_calbas:
    ld hl, 2
    add hl, sp
    ld e, (hl)
    inc hl
    ld d, (hl)
    push de
    pop ix
    call 0x0159     ; CALBAS
    ret

; uint8_t sys_breakx(void)
; Check CTRL+STOP via BREAKX BIOS
PUBLIC _sys_breakx
_sys_breakx:
    call 0x00B7     ; BREAKX
    ld l, 0
    ret nc          ; Not pressed
    inc l           ; Pressed (carry set)
    ret

; uint8_t sys_rdvdp_n(uint8_t reg)
; Read VDP status register n (MSX2)
; sccz80 stack: [ret][reg]
PUBLIC _sys_rdvdp_n
_sys_rdvdp_n:
    ld hl, 2
    add hl, sp
    ld a, (hl)      ; status register number
    di
    out (0x99), a   ; Write status register number
    ld a, 0x8F      ; Write to R#15
    out (0x99), a
    in a, (0x99)    ; Read status
    push af
    xor a           ; Reset to status register 0
    out (0x99), a
    ld a, 0x8F
    out (0x99), a
    ei
    pop af
    ld l, a
    ret

; uint8_t sys_rdslt(uint8_t slot, uint16_t addr)
; Read from slot using RDSLT BIOS
; sccz80 stack: [ret][addr][slot]
; SP+2=addr, SP+4=slot
PUBLIC _sys_rdslt
_sys_rdslt:
    ld hl, 4
    add hl, sp
    ld a, (hl)      ; slot
    ld hl, 2
    add hl, sp
    ld e, (hl)
    inc hl
    ld d, (hl)
    ex de, hl       ; HL = address
    call 0x000C     ; RDSLT
    ld l, a
    ret

; void sys_wrslt(uint8_t slot, uint16_t addr, uint8_t val)
; Write to slot using WRSLT BIOS
; sccz80 stack: [ret][val][addr][slot]
; SP+2=val, SP+4=addr, SP+6=slot
PUBLIC _sys_wrslt
_sys_wrslt:
    ld hl, 6
    add hl, sp
    ld a, (hl)      ; slot
    ld hl, 4
    add hl, sp
    ld e, (hl)
    inc hl
    ld d, (hl)
    push de         ; save addr
    ld hl, 2
    add hl, sp
    add hl, sp      ; adjust for pushed de
    ld e, (hl)      ; val
    pop hl          ; HL = address
    call 0x0014     ; WRSLT
    ret

; void sys_calslt(uint8_t slot, uint16_t addr)
; Inter-slot call using CALSLT BIOS
; sccz80 stack: [ret][addr][slot]
; SP+2=addr, SP+4=slot
PUBLIC _sys_calslt
_sys_calslt:
    ld hl, 4
    add hl, sp
    ld a, (hl)      ; slot in IYh
    ld iyh, a
    ld hl, 2
    add hl, sp
    ld e, (hl)
    inc hl
    ld d, (hl)
    push de
    pop ix          ; IX = address
    call 0x001C     ; CALSLT
    ret

; uint8_t sys_vpeek_ex(uint16_t addr_low, uint16_t addr_high)
; Read from extended VRAM (MSX2, 128KB)
; sccz80 stack: [ret][addr_high][addr_low]
; SP+2=addr_high, SP+4=addr_low
; addr_high bit 0 = VRAM A16 (for 128KB)
; addr_low bits 0-15 = VRAM A0-A15
PUBLIC _sys_vpeek_ex
_sys_vpeek_ex:
    di
    ; Calculate R#14 value from addr_high and high bits of addr_low
    ld hl, 2
    add hl, sp
    ld a, (hl)      ; addr_high (bit 0 = A16)
    ld c, a         ; save
    ld hl, 4
    add hl, sp
    inc hl          ; point to high byte of addr_low
    ld a, (hl)      ; bits 8-15 of address
    ld b, a         ; save
    ; R#14 = bits 14-16 of address
    ; bit 14 = addr_low bit 14, bit 15 = addr_low bit 15, bit 16 = addr_high bit 0
    srl a           ; shift right
    srl a           ; now A has bits 14-15 in position 4-5
    and 0x03        ; mask bits 14-15
    ld d, a
    ld a, c
    and 0x01        ; bit 16
    rlca
    rlca            ; shift to bit 2
    or d            ; combine
    ; Set R#14
    out (0x99), a
    ld a, 0x80 + 14
    out (0x99), a
    ; Set read address (bits 0-13)
    ld hl, 4
    add hl, sp
    ld a, (hl)      ; addr_low low byte
    out (0x99), a
    ld a, b         ; addr_low high byte
    and 0x3F        ; mask to 14 bits
    out (0x99), a   ; read mode (bit 6 = 0)
    ; Read data
    in a, (0x98)
    ld l, a
    ; Reset R#14
    xor a
    out (0x99), a
    ld a, 0x80 + 14
    out (0x99), a
    ei
    ret

; void sys_vpoke_ex(uint8_t val, uint16_t addr_low, uint16_t addr_high)
; Write to extended VRAM (MSX2, 128KB)
; sccz80 stack: [ret][addr_high][addr_low][val]
; SP+2=addr_high, SP+4=addr_low, SP+6=val
PUBLIC _sys_vpoke_ex
_sys_vpoke_ex:
    di
    ; Calculate R#14 value
    ld hl, 2
    add hl, sp
    ld a, (hl)      ; addr_high
    ld c, a
    ld hl, 4
    add hl, sp
    inc hl
    ld a, (hl)      ; high byte of addr_low
    ld b, a
    srl a
    srl a
    and 0x03
    ld d, a
    ld a, c
    and 0x01
    rlca
    rlca
    or d
    ; Set R#14
    out (0x99), a
    ld a, 0x80 + 14
    out (0x99), a
    ; Set write address
    ld hl, 4
    add hl, sp
    ld a, (hl)      ; addr_low low byte
    out (0x99), a
    ld a, b
    and 0x3F
    or 0x40         ; write mode
    out (0x99), a
    ; Get value and write
    ld hl, 6
    add hl, sp
    ld a, (hl)      ; val
    out (0x98), a
    ; Reset R#14
    xor a
    out (0x99), a
    ld a, 0x80 + 14
    out (0x99), a
    ei
    ret

#endasm

extern uint8_t sys_vpeek(uint16_t addr);
extern void sys_vpoke(uint16_t addr, uint8_t val);
extern uint8_t sys_rdvdp(void);
extern void sys_halt(void);
extern void sys_filvrm(uint16_t addr, uint16_t count, uint8_t value);
extern void sys_ldirvm(uint16_t dest, const uint8_t* src, uint16_t count);
extern void sys_ldirmv(uint8_t* dest, uint16_t src, uint16_t count);
extern uint8_t sys_snsmat(uint8_t row);
extern void sys_calbas(uint16_t addr);
extern uint8_t sys_breakx(void);
extern uint8_t sys_rdvdp_n(uint8_t reg);
extern uint8_t sys_rdslt(uint8_t slot, uint16_t addr);
extern void sys_wrslt(uint8_t slot, uint16_t addr, uint8_t val);
extern void sys_calslt(uint8_t slot, uint16_t addr);

uint32_t basic_time(void) {
    uint32_t t;
    t = sys_read8(0xFCA0);
    t = (t << 16) | *(volatile uint16_t*)0xFC9E;
    return t;
}

void basic_time_set(uint32_t value) {
    *(volatile uint16_t*)0xFC9E = value & 0xFFFF;
    sys_write8(0xFCA0, (value >> 16) & 0xFF);
}

uint8_t basic_vdp(uint8_t reg) {
    if (reg <= 7) return sys_read8(0xF3DF + reg);
    return 0;
}

void basic_vdp_set(uint8_t reg, uint8_t value) {
    #asm
        pop hl
        pop bc
        pop de
        push de
        push bc
        push hl
        ld a, e
        di
        out (0x99), a
        ld a, c
        or 0x80
        out (0x99), a
        ei
    #endasm
    if (reg <= 7) sys_write8(0xF3DF + reg, value);
}

uint8_t basic_vdp_status(void) { return sys_rdvdp(); }
uint8_t basic_vdp_status_n(uint8_t reg) {
    if (sys_read8(MSXVER) < 1) return sys_rdvdp();  /* MSX1 only has S#0 */
    if (reg > 9) return 0;
    return sys_rdvdp_n(reg);
}
uint8_t basic_vpeek(uint16_t address) { return sys_vpeek(address); }
void basic_vpoke(uint16_t address, uint8_t value) { sys_vpoke(address, value); }
/* Extern declarations for assembly extended VRAM functions */
extern uint8_t sys_vpeek_ex(uint16_t addr_low, uint16_t addr_high);
extern void sys_vpoke_ex(uint8_t val, uint16_t addr_low, uint16_t addr_high);

uint8_t basic_vpeek_ex(uint32_t address) {
    /* MSX1: only 16KB VRAM */
    if (sys_read8(MSXVER) < 1) {
        return sys_vpeek(address & 0x3FFF);
    }
    /* MSX2: use assembly function for 128KB VRAM */
    return sys_vpeek_ex((uint16_t)(address & 0xFFFF), (uint16_t)((address >> 16) & 0x01));
}

void basic_vpoke_ex(uint32_t address, uint8_t value) {
    /* MSX1: only 16KB VRAM */
    if (sys_read8(MSXVER) < 1) {
        sys_vpoke(address & 0x3FFF, value);
        return;
    }
    /* MSX2: use assembly function for 128KB VRAM */
    sys_vpoke_ex(value, (uint16_t)(address & 0xFFFF), (uint16_t)((address >> 16) & 0x01));
}

uint16_t basic_base(uint8_t n) {
    /* Return VRAM base addresses for screen elements */
    /* Based on MSX BASIC BASE() function */
    switch (n) {
        case 0: return sys_read8(0xF3B3) | (sys_read8(0xF3B4) << 8); /* Name table */
        case 1: return sys_read8(0xF3B5) | (sys_read8(0xF3B6) << 8); /* Color table */
        case 2: return sys_read8(0xF3B7) | (sys_read8(0xF3B8) << 8); /* Pattern generator */
        case 5: return sys_read8(0xF3C7) | (sys_read8(0xF3C8) << 8); /* Sprite attribute */
        case 6: return sys_read8(0xF3C9) | (sys_read8(0xF3CA) << 8); /* Sprite pattern */
        default: return 0;
    }
}

void basic_vram_fill(uint16_t address, uint8_t value, uint16_t count) {
    sys_filvrm(address, count, value);
}

void basic_vram_write(uint16_t dest, const uint8_t* src, uint16_t count) {
    sys_ldirvm(dest, src, count);
}

void basic_vram_read(uint8_t* dest, uint16_t src, uint16_t count) {
    sys_ldirmv(dest, src, count);
}

void basic_wait_vblank(void) { sys_halt(); }
void basic_wait_frames(uint16_t frames) { while (frames--) sys_halt(); }

void basic_bios_call(uint16_t address) {
    sys_calbas(address);
}

void basic_inter_slot_call(uint8_t slot, uint16_t address) {
    sys_calslt(slot, address);
}

uint8_t basic_break_check(void) {
    return sys_breakx();
}

void basic_break_enable(uint8_t enable) {
    /* ISCNTC (0x00BA) enables/disables CTRL+STOP check */
    /* Writing to INTFLG (0xFC9B): 0=disabled, 3=enabled */
    sys_write8(0xFC9B, enable ? 3 : 0);
}

uint16_t basic_fre(void) {
    /* Return approximate free memory */
    /* HIMEM - TXTTAB gives BASIC free memory, but we're in C */
    /* Return HIMEM (top of user RAM) minus current stack */
    uint16_t himem = sys_read8(0xFC4A) | (sys_read8(0xFC4B) << 8);
    return himem > 0x8000 ? himem - 0x8000 : 0x8000;
}

uint8_t basic_slot_read(uint16_t address, uint8_t slot, uint8_t subslot) {
    uint8_t full_slot;
    /* Combine slot and subslot into FxxxSSPP format */
    /* Bit 7 set if expanded, bits 3-2 = subslot, bits 1-0 = primary slot */
    if (subslot > 0) {
        full_slot = slot | (subslot << 2) | 0x80;
    } else {
        full_slot = slot;
    }
    return sys_rdslt(full_slot, address);
}

void basic_slot_write(uint16_t address, uint8_t slot, uint8_t subslot, uint8_t value) {
    uint8_t full_slot;
    if (subslot > 0) {
        full_slot = slot | (subslot << 2) | 0x80;
    } else {
        full_slot = slot;
    }
    sys_wrslt(full_slot, address, value);
}
uint8_t basic_chdir(const char* path) { (void)path; return 0xFF; }
uint8_t basic_mkdir(const char* path) { (void)path; return 0xFF; }
uint8_t basic_rmdir(const char* path) { (void)path; return 0xFF; }
void basic_system(const char* command) { (void)command; }

uint8_t basic_get_msx_type(void) { return sys_read8(MSXVER); }
uint8_t basic_is_msx2(void) { return sys_read8(MSXVER) >= 1; }
uint8_t basic_is_msx2plus(void) { return sys_read8(MSXVER) >= 2; }
uint8_t basic_is_turbo_r(void) { return sys_read8(MSXVER) >= 3; }
void basic_init(void) {}
