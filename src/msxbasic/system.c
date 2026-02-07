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
#define CSRSW  0xFCA9
#define sys_read8(addr)  (*(volatile uint8_t*)(addr))
#define sys_write8(addr, val) (*(volatile uint8_t*)(addr) = (val))

/* Forward declaration */
void basic_init(void);

/* Cached MSX version (read from BIOS ROM during init) */
static uint8_t cached_msx_ver = 0;

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
    push hl
    ld hl, 0x004A
    ld (0xC02C), hl
    pop hl
    call 0xC000     ; RDVRM
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
    push hl
    ld hl, 0x004D
    ld (0xC02C), hl
    pop hl
    call 0xC000     ; WRTVRM
    ret

_sys_rdvdp:
    ld hl, 0x013E
    ld (0xC02C), hl
    call 0xC000     ; RDVDP
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
    push hl
    ld hl, 0x0056
    ld (0xC02C), hl
    pop hl
    call 0xC000     ; FILVRM
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
    push hl
    ld hl, 0x005C
    ld (0xC02C), hl
    pop hl
    call 0xC000     ; LDIRVM
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
    push hl
    ld hl, 0x0059
    ld (0xC02C), hl
    pop hl
    call 0xC000     ; LDIRMV
    ret

; uint8_t sys_snsmat(uint8_t row)
; sccz80 stack: [ret][row]
; SP+2=row
; SNSMAT: A=row, returns keyboard matrix in A
_sys_snsmat:
    ld hl, 2
    add hl, sp
    ld a, (hl)      ; row
    ld hl, 0x0141
    ld (0xC02C), hl
    call 0xC000     ; SNSMAT
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
    ld hl, 0x0159
    ld (0xC02C), hl
    call 0xC000     ; CALBAS
    ret

; uint8_t sys_breakx(void)
; Check CTRL+STOP via BREAKX BIOS
PUBLIC _sys_breakx
_sys_breakx:
    ld hl, 0x00B7
    ld (0xC02C), hl
    call 0xC000     ; BREAKX
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
    push hl
    ld hl, 0x000C
    ld (0xC02C), hl
    pop hl
    call 0xC000     ; RDSLT
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
    push hl
    ld hl, 0x0014
    ld (0xC02C), hl
    pop hl
    call 0xC000     ; WRSLT
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
    ld hl, 0x001C
    ld (0xC02C), hl
    call 0xC000     ; CALSLT
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
    basic_init();
    if (cached_msx_ver < 1) return sys_rdvdp();  /* MSX1 only has S#0 */
    if (reg > 9) return 0;
    return sys_rdvdp_n(reg);
}
uint8_t basic_vpeek(uint16_t address) { return sys_vpeek(address); }
void basic_vpoke(uint16_t address, uint8_t value) { sys_vpoke(address, value); }
/* Extern declarations for assembly extended VRAM functions */
extern uint8_t sys_vpeek_ex(uint16_t addr_low, uint16_t addr_high);
extern void sys_vpoke_ex(uint8_t val, uint16_t addr_low, uint16_t addr_high);

uint8_t basic_vpeek_ex(uint32_t address) {
    basic_init();
    /* MSX1: only 16KB VRAM */
    if (cached_msx_ver < 1) {
        return sys_vpeek(address & 0x3FFF);
    }
    /* MSX2: use assembly function for 128KB VRAM */
    return sys_vpeek_ex((uint16_t)(address & 0xFFFF), (uint16_t)((address >> 16) & 0x01));
}

void basic_vpoke_ex(uint32_t address, uint8_t value) {
    basic_init();
    /* MSX1: only 16KB VRAM */
    if (cached_msx_ver < 1) {
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

uint8_t basic_get_msx_type(void) { basic_init(); return cached_msx_ver; }
uint8_t basic_is_msx2(void) { basic_init(); return cached_msx_ver >= 1; }
uint8_t basic_is_msx2plus(void) { basic_init(); return cached_msx_ver >= 2; }
uint8_t basic_is_turbo_r(void) { basic_init(); return cached_msx_ver >= 3; }
void basic_init(void) {
    /*
     * BIOS call trampoline for MSX-DOS compatibility.
     * Copies 72 bytes of Z80 code to 0xC000 (page 3 RAM).
     * The trampoline neutralizes H.KEYI and H.TIMI hooks,
     * switches page 0 to BIOS slot with interrupts disabled,
     * calls BIOS, then restores everything.
     * Callers patch the call address at 0xC02C before calling 0xC000.
     *
     * Memory layout:
     *   0xC000-0xC043: trampoline code (68 bytes)
     *   0xC044:        save_slot (1 byte)
     *   0xC045:        save_keyi (1 byte)
     *   0xC046:        save_timi (1 byte)
     *   0xC047:        reserved  (1 byte)
     */
    static const uint8_t tramp[] = {
        /* Save registers and disable interrupts */
        0xF5,                   /* push af              */  /* 0  */
        0xC5,                   /* push bc              */  /* 1  */
        0xF3,                   /* di                   */  /* 2  */
        /* Save H.KEYI first byte (0xFD9A) */
        0x3A, 0x9A, 0xFD,       /* ld a, (0xFD9A)       */  /* 3  */
        0x32, 0x45, 0xC0,       /* ld (0xC045), a       */  /* 6  */
        /* Save H.TIMI first byte (0xFD9F) */
        0x3A, 0x9F, 0xFD,       /* ld a, (0xFD9F)       */  /* 9  */
        0x32, 0x46, 0xC0,       /* ld (0xC046), a       */  /* 12 */
        /* Neutralize both hooks (write RET instruction) */
        0x3E, 0xC9,             /* ld a, 0xC9 (RET)     */  /* 15 */
        0x32, 0x9A, 0xFD,       /* ld (0xFD9A), a       */  /* 17 */
        0x32, 0x9F, 0xFD,       /* ld (0xFD9F), a       */  /* 20 */
        /* Save current primary slot register */
        0xDB, 0xA8,             /* in a, (0xA8)         */  /* 23 */
        0x32, 0x44, 0xC0,       /* ld (0xC044), a       */  /* 25 */
        /* Switch page 0 to BIOS slot */
        0x3A, 0xC1, 0xFC,       /* ld a, (0xFCC1)       */  /* 28 */
        0xE6, 0x03,             /* and 0x03             */  /* 31 */
        0x4F,                   /* ld c, a              */  /* 33 */
        0xDB, 0xA8,             /* in a, (0xA8)         */  /* 34 */
        0xE6, 0xFC,             /* and 0xFC             */  /* 36 */
        0xB1,                   /* or c                 */  /* 38 */
        0xD3, 0xA8,             /* out (0xA8), a        */  /* 39 */
        /* Restore caller's registers and call BIOS (DI) */
        0xC1,                   /* pop bc               */  /* 41 */
        0xF1,                   /* pop af               */  /* 42 */
        0xCD, 0x00, 0x00,       /* call 0x0000 (patch@44) */ /* 43 */
        /* Post-call: save result and disable interrupts */
        0xF5,                   /* push af              */  /* 46 */
        0xF3,                   /* di                   */  /* 47 */
        /* Restore primary slot register */
        0x3A, 0x44, 0xC0,       /* ld a, (0xC044)       */  /* 48 */
        0xD3, 0xA8,             /* out (0xA8), a        */  /* 51 */
        /* Restore H.KEYI */
        0x3A, 0x45, 0xC0,       /* ld a, (0xC045)       */  /* 53 */
        0x32, 0x9A, 0xFD,       /* ld (0xFD9A), a       */  /* 56 */
        /* Restore H.TIMI */
        0x3A, 0x46, 0xC0,       /* ld a, (0xC046)       */  /* 59 */
        0x32, 0x9F, 0xFD,       /* ld (0xFD9F), a       */  /* 62 */
        /* Enable interrupts and return */
        0xFB,                   /* ei                   */  /* 65 */
        0xF1,                   /* pop af               */  /* 66 */
        0xC9,                   /* ret                  */  /* 67 */
        /* Save areas */
        0x00,                   /* save_slot  (0xC044)  */  /* 68 */
        0x00,                   /* save_keyi  (0xC045)  */  /* 69 */
        0x00,                   /* save_timi  (0xC046)  */  /* 70 */
        0x00                    /* (reserved) (0xC047)  */  /* 71 */
    };
    static uint8_t done = 0;
    volatile uint8_t* dest;
    uint8_t i;
    if (done) return;
    dest = (volatile uint8_t*)0xC000;
    for (i = 0; i < 72; i++) dest[i] = tramp[i];
    done = 1;

    /* Read MSX version from BIOS ROM via RDSLT through trampoline */
    cached_msx_ver = sys_rdslt(sys_read8(0xFCC1), 0x002D);

    /* Hide cursor (prevents white block after CHPUT) */
    sys_write8(CSRSW, 0x00);
    /* Suppress key click sound during BIOS interrupt handling */
    sys_write8(0xF3DB, 0x00);  /* CLIKSW = 0 */
    /* Initialize PSG and clear music queue via GICINI BIOS call.
     * This silences all PSG channels, resets voice control blocks
     * (VCBA/VCBB/VCBC), and clears MUSICF, preventing beep sounds
     * from garbage data in the music queue. */
    #asm
    ld hl, 0x0090
    ld (0xC02C), hl
    call 0xC000
    #endasm
}
