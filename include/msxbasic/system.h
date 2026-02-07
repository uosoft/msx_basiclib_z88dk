/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file system.h
 * @brief MSX BASIC system functions
 *
 * Provides functions equivalent to MSX BASIC's:
 * TIME, VDP, VPEEK, VPOKE, BASE, VARPTR, and CALL commands
 */

#ifndef MSXBASIC_SYSTEM_H
#define MSXBASIC_SYSTEM_H

#include <stdint.h>

/* MSX System Variables Addresses */
#define MSXVER      0x002D  /* MSX version */
#define LINL40      0xF3AE  /* Width for SCREEN 0 */
#define LINL32      0xF3AF  /* Width for SCREEN 1 */
#define LINLEN      0xF3B0  /* Current text width */
#define CRTCNT      0xF3B1  /* Number of lines */
#define CLMLST      0xF3B2  /* Column for last character */
#define CSRY        0xF3DC  /* Cursor Y position */
#define CSRX        0xF3DD  /* Cursor X position */
#define CNSDFG      0xF3DE  /* Function key display flag */
#define FORCLR      0xF3E9  /* Foreground color */
#define BAKCLR      0xF3EA  /* Background color */
#define BDRCLR      0xF3EB  /* Border color */
#define GRPACX      0xFCB7  /* Graphics X position */
#define GRPACY      0xFCB9  /* Graphics Y position */
#define JIESSION    0xF348  /* Kanji mode flag */
#define INTCNT      0xFCA2  /* INTERVAL counter */
#define MUSICF      0xFB16  /* PLAY status flag */
#define RG0SAV      0xF3DF  /* VDP register 0 save */
#define RG1SAV      0xF3E0  /* VDP register 1 save */
#define STATFL      0xF3E7  /* VDP status flag */
#define DPPAGE      0xFAF5  /* Display page (MSX2) */
#define ACPAGE      0xFAF6  /* Active page (MSX2) */

/* VDP I/O Ports */
#define VDP_DATA    0x98    /* VRAM data read/write */
#define VDP_STAT    0x99    /* VDP status/register */

/* BIOS entry points */
#define BIOS_CHKRAM 0x0000
#define BIOS_SYNCHR 0x0008
#define BIOS_RDSLT  0x000C
#define BIOS_CHRGTR 0x0010
#define BIOS_WRSLT  0x0014
#define BIOS_OUTDO  0x0018
#define BIOS_CALSLT 0x001C
#define BIOS_DCOMPR 0x0020
#define BIOS_ENASLT 0x0024
#define BIOS_GETYPR 0x0028
#define BIOS_CALLF  0x0030
#define BIOS_KEYINT 0x0038
#define BIOS_INITIO 0x003B
#define BIOS_INIFNK 0x003E
#define BIOS_DISSCR 0x0041
#define BIOS_ENASCR 0x0044
#define BIOS_WRTVDP 0x0047
#define BIOS_RDVRM  0x004A
#define BIOS_WRTVRM 0x004D
#define BIOS_SETRD  0x0050
#define BIOS_SETWRT 0x0053
#define BIOS_FILVRM 0x0056
#define BIOS_LDIRMV 0x0059
#define BIOS_LDIRVM 0x005C
#define BIOS_CHGMOD 0x005F
#define BIOS_CHGCLR 0x0062
#define BIOS_NMI    0x0066
#define BIOS_CLRSPR 0x0069
#define BIOS_INITXT 0x006C
#define BIOS_INIT32 0x006F
#define BIOS_INIGRP 0x0072
#define BIOS_INIMLT 0x0075
#define BIOS_SETTXT 0x0078
#define BIOS_SETT32 0x007B
#define BIOS_SETGRP 0x007E
#define BIOS_SETMLT 0x0081
#define BIOS_CALPAT 0x0084
#define BIOS_CALATR 0x0087
#define BIOS_GSPSIZ 0x008A
#define BIOS_GRPPRT 0x008D
#define BIOS_GICINI 0x0090
#define BIOS_WRTPSG 0x0093
#define BIOS_RDPSG  0x0096
#define BIOS_STRTMS 0x0099
#define BIOS_CHSNS  0x009C
#define BIOS_CHGET  0x009F
#define BIOS_CHPUT  0x00A2
#define BIOS_LPTOUT 0x00A5
#define BIOS_LPTSTT 0x00A8
#define BIOS_CNVCHR 0x00AB
#define BIOS_PINLIN 0x00AE
#define BIOS_INLIN  0x00B1
#define BIOS_QINLIN 0x00B4
#define BIOS_BREAKX 0x00B7
#define BIOS_ISCNTC 0x00BA
#define BIOS_CKCNTC 0x00BD
#define BIOS_BEEP   0x00C0
#define BIOS_CLS    0x00C3
#define BIOS_POSIT  0x00C6
#define BIOS_FNKSB  0x00C9
#define BIOS_ERAFNK 0x00CC
#define BIOS_DSPFNK 0x00CF
#define BIOS_TOTEXT 0x00D2
#define BIOS_GTSTCK 0x00D5
#define BIOS_GTTRIG 0x00D8
#define BIOS_GTPAD  0x00DB
#define BIOS_GTPDL  0x00DE
#define BIOS_TAPION 0x00E1
#define BIOS_TAPIN  0x00E4
#define BIOS_TAPIOF 0x00E7
#define BIOS_TAPOON 0x00EA
#define BIOS_TAPOUT 0x00ED
#define BIOS_TAPOOF 0x00F0
#define BIOS_STMOTR 0x00F3
#define BIOS_LFTQ   0x00F6
#define BIOS_PUTQ   0x00F9
#define BIOS_RIGHTC 0x00FC
#define BIOS_LEFTC  0x00FF
#define BIOS_UPC    0x0102
#define BIOS_TUPC   0x0105
#define BIOS_DOWNC  0x0108
#define BIOS_TDOWNC 0x010B
#define BIOS_SCALXY 0x010E
#define BIOS_MAPXY  0x0111
#define BIOS_FETCHC 0x0114
#define BIOS_STOREC 0x0117
#define BIOS_SETATR 0x011A
#define BIOS_READC  0x011D
#define BIOS_SETC   0x0120
#define BIOS_NSETCX 0x0123
#define BIOS_GTASPC 0x0126
#define BIOS_PNTINI 0x0129
#define BIOS_SCANR  0x012C
#define BIOS_SCANL  0x012F
#define BIOS_CHGCAP 0x0132
#define BIOS_CHGSND 0x0135
#define BIOS_RSLREG 0x0138
#define BIOS_WSLREG 0x013B
#define BIOS_RDVDP  0x013E
#define BIOS_SNSMAT 0x0141
#define BIOS_PHYDIO 0x0144
#define BIOS_FORMAT 0x0147
#define BIOS_ISFLIO 0x014A
#define BIOS_OUTDLP 0x014D
#define BIOS_GETVCP 0x0150
#define BIOS_GETVC2 0x0153
#define BIOS_KILBUF 0x0156
#define BIOS_CALBAS 0x0159

/* MSX2 BIOS extensions */
#define BIOS_SUBROM 0x015C
#define BIOS_EXTROM 0x015F
#define BIOS_CHKSLZ 0x0162
#define BIOS_CHKNEW 0x0165
#define BIOS_EOL    0x0168
#define BIOS_BIGFIL 0x016B
#define BIOS_NSETRD 0x016E
#define BIOS_NSTWRT 0x0171
#define BIOS_NRDVRM 0x0174
#define BIOS_NWRVRM 0x0177

/**
 * @brief Get system timer value
 * Equivalent to: TIME
 * @return Timer value (increments at VSYNC, ~60Hz NTSC, ~50Hz PAL)
 */
uint32_t basic_time(void);

/**
 * @brief Set system timer value
 * Equivalent to: TIME = n
 * @param value New timer value
 */
void basic_time_set(uint32_t value);

/**
 * @brief Read VDP register
 * Equivalent to: VDP(register)
 * @param reg VDP register number
 * @return Register value
 */
uint8_t basic_vdp(uint8_t reg);

/**
 * @brief Write VDP register
 * @param reg VDP register number
 * @param value Value to write
 */
void basic_vdp_set(uint8_t reg, uint8_t value);

/**
 * @brief Read VDP status register
 * Equivalent to: VDP(-1)
 * @return VDP status
 */
uint8_t basic_vdp_status(void);

/**
 * @brief Read VDP status register n (MSX2)
 * @param reg Status register number (0-9)
 * @return Status register value
 */
uint8_t basic_vdp_status_n(uint8_t reg);

/**
 * @brief Read from VRAM
 * Equivalent to: VPEEK(address)
 * @param address VRAM address
 * @return Byte at address
 */
uint8_t basic_vpeek(uint16_t address);

/**
 * @brief Write to VRAM
 * Equivalent to: VPOKE address, value
 * @param address VRAM address
 * @param value Byte to write
 */
void basic_vpoke(uint16_t address, uint8_t value);

/**
 * @brief Read from extended VRAM (MSX2, address > 16KB)
 * @param address VRAM address (0-131071)
 * @return Byte at address
 */
uint8_t basic_vpeek_ex(uint32_t address);

/**
 * @brief Write to extended VRAM (MSX2, address > 16KB)
 * @param address VRAM address (0-131071)
 * @param value Byte to write
 */
void basic_vpoke_ex(uint32_t address, uint8_t value);

/**
 * @brief Get BASE address for screen element
 * Equivalent to: BASE(n)
 * @param n Element number (0-19)
 * @return VRAM base address
 */
uint16_t basic_base(uint8_t n);

/**
 * @brief Fill VRAM with a value
 * @param address Start VRAM address
 * @param value Fill value
 * @param count Number of bytes
 */
void basic_vram_fill(uint16_t address, uint8_t value, uint16_t count);

/**
 * @brief Copy data from RAM to VRAM
 * @param dest VRAM destination address
 * @param src RAM source address
 * @param count Number of bytes
 */
void basic_vram_write(uint16_t dest, const uint8_t* src, uint16_t count);

/**
 * @brief Copy data from VRAM to RAM
 * @param dest RAM destination address
 * @param src VRAM source address
 * @param count Number of bytes
 */
void basic_vram_read(uint8_t* dest, uint16_t src, uint16_t count);

/**
 * @brief Wait for vertical blank
 */
void basic_wait_vblank(void);

/**
 * @brief Wait for specified number of frames
 * @param frames Number of frames to wait
 */
void basic_wait_frames(uint16_t frames);

/**
 * @brief Call BIOS routine
 * @param address BIOS routine address
 */
void basic_bios_call(uint16_t address);

/**
 * @brief Call inter-slot routine
 * @param slot Slot number
 * @param address Routine address
 */
void basic_inter_slot_call(uint8_t slot, uint16_t address);

/**
 * @brief Check CTRL+STOP
 * @return 1 if CTRL+STOP pressed, 0 otherwise
 */
uint8_t basic_break_check(void);

/**
 * @brief Enable/disable CTRL+STOP
 * @param enable 0=disable, 1=enable
 */
void basic_break_enable(uint8_t enable);

/**
 * @brief Get free memory size
 * Equivalent to: FRE(0)
 * @return Free memory in bytes
 */
uint16_t basic_fre(void);

/**
 * @brief Read slot from address
 * @param address Address to read
 * @param slot Primary slot (0-3)
 * @param subslot Subslot (0-3, or 0 if not expanded)
 * @return Byte at address in specified slot
 */
uint8_t basic_slot_read(uint16_t address, uint8_t slot, uint8_t subslot);

/**
 * @brief Write to slot address
 * @param address Address to write
 * @param slot Primary slot (0-3)
 * @param subslot Subslot (0-3, or 0 if not expanded)
 * @param value Byte to write
 */
void basic_slot_write(uint16_t address, uint8_t slot, uint8_t subslot, uint8_t value);

/* === CALL commands === */

/**
 * @brief Change directory (MSX-DOS2)
 * Equivalent to: CALL CHDIR(path$)
 * @param path Directory path
 * @return 0 on success, error code otherwise
 */
uint8_t basic_chdir(const char* path);

/**
 * @brief Make directory (MSX-DOS2)
 * Equivalent to: CALL MKDIR(path$)
 * @param path Directory path
 * @return 0 on success, error code otherwise
 */
uint8_t basic_mkdir(const char* path);

/**
 * @brief Remove directory (MSX-DOS2)
 * Equivalent to: CALL RMDIR(path$)
 * @param path Directory path
 * @return 0 on success, error code otherwise
 */
uint8_t basic_rmdir(const char* path);

/**
 * @brief Execute MSX-DOS command
 * Equivalent to: CALL SYSTEM(command$)
 * @param command DOS command
 */
void basic_system(const char* command);

#endif /* MSXBASIC_SYSTEM_H */
