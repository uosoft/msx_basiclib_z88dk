# MSX BASIC Library for Z88DK

A C library for Z88DK that provides MSX BASIC-compatible commands and functions.
Write MSX programs in C using familiar BASIC-like APIs.

[日本語版はこちら](README_ja.md)

## Supported Platforms

- MSX1 (TMS9918)
- MSX2 (V9938)
- MSX2+ (V9958)
- MSX turbo R

## Requirements

- [Z88DK](https://z88dk.org/) (latest nightly build recommended)
- MSX emulator for testing ([openMSX](https://openmsx.org/), blueMSX, etc.)

## Quick Start

### Build the Library

```batch
build.bat
```

### Build an Example

```batch
build_sample.bat 01_hello
```

### Compile Your Program

```batch
zcc +msx -vn -O3 -compiler=sccz80 -lm ^
    -Iinclude -Llib -lmsxbasic ^
    -subtype=rom your_program.c -o your_program -create-app
```

### Hello World

```c
#include <msxbasic/msxbasic.h>

void main(void) {
    basic_screen(1);
    basic_color(15, 1, 1);
    basic_cls();
    basic_print("Hello, MSX World!");
    basic_wait_key();
    while (1);
}
```

## API Reference

### Screen Control (screen.h)

| MSX BASIC | C Function | Description |
|-----------|-----------|-------------|
| `CLS` | `basic_cls()` | Clear screen |
| `SCREEN n` | `basic_screen(mode)` | Set screen mode (0-12) |
| `SCREEN n,s,k` | `basic_screen_ex(mode, sprite, click)` | Set screen mode with options |
| `COLOR f,b,d` | `basic_color(fg, bg, border)` | Set colors |
| `COLOR f` | `basic_color_fg(fg)` | Set foreground color only |
| `LOCATE x,y` | `basic_locate(x, y)` | Move cursor (0-based) |
| `LOCATE x,y,c` | `basic_locate_ex(x, y, cursor)` | Move cursor with visibility |
| `PRINT` | `basic_print(s)` | Print string |
| `PRINT` | `basic_println(s)` | Print string with newline |
| `PRINT n` | `basic_print_int(n)` | Print integer |
| `PRINT CHR$(c)` | `basic_print_char(c)` | Print character |
| `WIDTH n` | `basic_width(w)` | Set screen width |
| `CSRLIN` | `basic_csrlin()` | Get cursor row (1-based) |
| `POS(0)` | `basic_pos(0)` | Get cursor column (1-based) |
| `TAB(n)` | `basic_tab(n)` | Move cursor to column |
| `SPC(n)` | `basic_spc(n)` | Print n spaces |
| `SCREEN$(x,y)` | `basic_screen_char(x, y)` | Read character at position |
| `COLOR=(p,r,g,b)` | `basic_set_palette(p, r, g, b)` | Set palette (MSX2) |
| `PRINT USING` | `basic_print_using_int(dest, fmt, val)` | Format integer |
| `PRINT USING` | `basic_print_using_float(dest, fmt, val)` | Format float |
| `PRINT USING` | `basic_print_using_str(dest, fmt, val)` | Format string |
| `WAIT` | `basic_wait(port, and, xor)` | Wait for I/O port value |
| - | `basic_get_screen_mode()` | Get current screen mode |
| - | `basic_cursor(visible)` | Show/hide cursor |

### Graphics (graphics.h)

#### Drawing

| MSX BASIC | C Function | Description |
|-----------|-----------|-------------|
| `PSET (x,y),c` | `basic_pset(x, y, color)` | Set pixel |
| `PSET (x,y)` | `basic_pset_c(x, y)` | Set pixel (foreground color) |
| `PSET STEP(dx,dy)` | `basic_pset_step(dx, dy, color)` | Set pixel (relative) |
| `PRESET (x,y)` | `basic_preset(x, y)` | Clear pixel (background color) |
| `LINE (x1,y1)-(x2,y2),c` | `basic_line(x1, y1, x2, y2, color)` | Draw line |
| `LINE ...,B` | `basic_box(x1, y1, x2, y2, color)` | Draw box outline |
| `LINE ...,BF` | `basic_boxfill(x1, y1, x2, y2, color)` | Draw filled box |
| `CIRCLE (x,y),r,c` | `basic_circle(x, y, r, color)` | Draw circle |
| `CIRCLE ...,s,e,a` | `basic_circle_ex(x, y, r, c, start, end, aspect)` | Draw arc/ellipse |
| - | `basic_ellipse(x, y, rx, ry, color)` | Draw ellipse |
| - | `basic_circle_fill(x, y, r, color)` | Draw filled circle |
| - | `basic_ellipse_fill(x, y, rx, ry, color)` | Draw filled ellipse |
| `PAINT (x,y),c,b` | `basic_paint(x, y, color, border)` | Flood fill |
| `PAINT (x,y),c` | `basic_paint_c(x, y, color)` | Flood fill (color=border) |
| `DRAW cmd$` | `basic_draw(cmd)` | Execute DRAW commands |
| `POINT(x,y)` | `basic_point(x, y)` | Get pixel color |
| - | `basic_init_grp()` | Initialize SCREEN 2 color table |

**DRAW Command Reference:** `U`p, `D`own, `L`eft, `R`ight, `E`(up-right), `F`(down-right), `G`(down-left), `H`(up-left), `M`x,y (move), `B`(pen up), `N`(no update), `C`n (color), `A`n (angle 0-3), `S`n (scale)

#### GET/PUT Block Operations

| MSX BASIC | C Function | Description |
|-----------|-----------|-------------|
| - | `basic_get_size(w, h)` | Calculate buffer size needed |
| `GET (x1,y1)-(x2,y2),A` | `basic_get(x1, y1, x2, y2, buf)` | Save screen area |
| `PUT (x,y),A,op` | `basic_put(x, y, buf, op)` | Restore screen area |

PUT operations: `PUT_PSET`(0), `PUT_AND`(1), `PUT_OR`(2), `PUT_XOR`(3), `PUT_PRESET`(4)

#### Sprites

| MSX BASIC | C Function | Description |
|-----------|-----------|-------------|
| `SCREEN n,size` | `basic_sprite_size(size)` | Set sprite size (0-3) |
| `SPRITE$(n)=pat$` | `basic_sprite_pattern(n, data)` | Define sprite pattern |
| `PUT SPRITE n,(x,y),c,p` | `basic_put_sprite(n, x, y, color, pat)` | Display sprite |
| - | `basic_sprite_off(n)` | Hide sprite |
| - | `basic_sprites_off()` | Hide all sprites |
| - | `basic_sprite_collision()` | Check sprite collision |

#### COPY / Page (MSX2)

| MSX BASIC | C Function | Description |
|-----------|-----------|-------------|
| `COPY` | `basic_copy(sx, sy, w, h, dx, dy)` | Copy screen area |
| `COPY ...TO ...,page` | `basic_copy_page(sx, sy, w, h, sp, dx, dy, dp)` | Copy between pages |
| `SET PAGE d,a` | `basic_set_page(display, active)` | Set display/active page |

### Sound (sound.h)

#### Basic Sound

| MSX BASIC | C Function | Description |
|-----------|-----------|-------------|
| `BEEP` | `basic_beep()` | Play beep |
| `SOUND r,v` | `basic_sound(reg, val)` | Write PSG register |
| - | `basic_sound_read(reg)` | Read PSG register |
| - | `basic_sound_off()` | Silence all channels |

#### Music (MML)

| MSX BASIC | C Function | Description |
|-----------|-----------|-------------|
| `PLAY s$` | `basic_play(mml)` | Play MML on channel A |
| `PLAY a$,b$,c$` | `basic_play_3ch(a, b, c)` | Play 3-channel simultaneous |
| - | `basic_play_note(ch, note, oct, dur)` | Play single note |

**MML Reference:** `C`-`B` (notes), `R` (rest), `O`n (octave), `L`n (length), `T`n (tempo), `V`n (volume), `Q`n (gate time), `<` `>` (octave shift), `#`/`+` (sharp), `-` (flat), `.` (dotted)

#### Tone Control

| C Function | Description |
|-----------|-------------|
| `basic_set_tone(ch, freq)` | Set channel frequency |
| `basic_set_volume(ch, vol)` | Set channel volume (0-15) |
| `basic_set_noise(freq)` | Set noise frequency |
| `basic_set_mixer(tone, noise)` | Enable/disable tone/noise |
| `basic_set_envelope(period, shape)` | Set envelope |

#### Sound Effects

| C Function | Description |
|-----------|-------------|
| `basic_sfx_explosion()` | Explosion effect |
| `basic_sfx_laser()` | Laser/shoot effect |
| `basic_sfx_jump()` | Jump effect |
| `basic_sfx_coin()` | Coin/pickup effect |
| `basic_sfx_hit()` | Hit/damage effect |

### Input (input.h)

| MSX BASIC | C Function | Description |
|-----------|-----------|-------------|
| `INKEY$` | `basic_inkey()` | Non-blocking key read |
| `INPUT$(n)` | `basic_input_str(buf, n)` | Read n characters |
| `INPUT a$` | `basic_input_line(buf, max)` | Read line |
| `STICK(n)` | `basic_stick(device)` | Read joystick (0-8) |
| `STRIG(n)` | `basic_strig(button)` | Read trigger (0/-1) |
| - | `basic_strig_bool(button)` | Read trigger (0/1) |
| `PADDLE(n)` | `basic_paddle(n)` | Read paddle position |
| `PEEK(a)` | `basic_peek(addr)` | Read memory byte |
| `POKE a,v` | `basic_poke(addr, val)` | Write memory byte |
| - | `basic_peek16(addr)` | Read memory word |
| - | `basic_poke16(addr, val)` | Write memory word |
| `INP(p)` | `basic_inp(port)` | Read I/O port |
| `OUT p,v` | `basic_out(port, val)` | Write I/O port |
| - | `basic_wait_key()` | Wait for key press |
| - | `basic_key_pressed(row, bit)` | Check specific key |
| - | `basic_clear_key_buffer()` | Clear keyboard buffer |
| `KEY n,s$` | `basic_key_def(n, str)` | Define function key |
| `KEY ON/OFF` | `basic_key_display(show)` | Show/hide function keys |

STICK constants: `STICK_KEYBOARD`(0), `STICK_JOY1`(1), `STICK_JOY2`(2)
STRIG constants: `STRIG_SPACE`(0), `STRIG_JOY1_A`(1), `STRIG_JOY2_A`(2), `STRIG_JOY1_B`(3), `STRIG_JOY2_B`(4)

### String Functions (bstring.h)

| MSX BASIC | C Function | Description |
|-----------|-----------|-------------|
| `LEN(s$)` | `basic_len(s)` | String length |
| `LEFT$(s$,n)` | `basic_left(dest, src, n)` | Left substring |
| `RIGHT$(s$,n)` | `basic_right(dest, src, n)` | Right substring |
| `MID$(s$,p,n)` | `basic_mid(dest, src, pos, n)` | Middle substring |
| `MID$(s$,p,n)=r$` | `basic_mid_replace(dest, pos, n, src)` | Replace substring |
| `ASC(s$)` | `basic_asc(s)` | Character code |
| `CHR$(n)` | `basic_chr(dest, code)` | Code to character |
| `STR$(n)` | `basic_str(dest, n)` | Integer to string |
| `STR$(n)` | `basic_str_long(dest, n)` | Long to string |
| `STR$(n)` | `basic_str_float(dest, n)` | Float to string |
| `VAL(s$)` | `basic_val(s)` | String to integer |
| `VAL(s$)` | `basic_val_long(s)` | String to long |
| `VAL(s$)` | `basic_val_float(s)` | String to float (with E notation) |
| `HEX$(n)` | `basic_hex(dest, n)` | Hex string |
| `BIN$(n)` | `basic_bin(dest, n)` | Binary string |
| `OCT$(n)` | `basic_oct(dest, n)` | Octal string |
| `INSTR(s$,t$)` | `basic_instr(s, search, start)` | Find substring |
| `STRING$(n,c)` | `basic_string(dest, n, code)` | Repeat character |
| `SPACE$(n)` | `basic_space(dest, n)` | Create spaces |
| - | `basic_ucase(s)` | Uppercase (in-place) |
| - | `basic_lcase(s)` | Lowercase (in-place) |
| - | `basic_strcmp(s1, s2)` | Compare strings |
| - | `basic_concat(dest, s1, s2)` | Concatenate strings |
| - | `basic_jis_to_sjis(dest, src)` | JIS to Shift-JIS |
| - | `basic_sjis_to_jis(dest, src)` | Shift-JIS to JIS |

### Math Functions (bmath.h)

| MSX BASIC | C Function | Description |
|-----------|-----------|-------------|
| `ABS(n)` | `basic_abs(n)` / `basic_abs_float(n)` | Absolute value |
| `SGN(n)` | `basic_sgn(n)` / `basic_sgn_float(n)` | Sign (-1, 0, 1) |
| `INT(n)` | `basic_int(n)` | Floor |
| `FIX(n)` | `basic_fix(n)` | Truncate toward zero |
| `CINT(n)` | `basic_cint(n)` | Round to integer |
| `CSNG(n)` | `basic_csng(n)` | Convert to single |
| `CDBL(n)` | `basic_cdbl(n)` | Convert to double |
| `SQR(n)` | `basic_sqr(n)` | Square root |
| `SIN(n)` | `basic_sin(n)` | Sine (radians) |
| `COS(n)` | `basic_cos(n)` | Cosine (radians) |
| `TAN(n)` | `basic_tan(n)` | Tangent (radians) |
| `ATN(n)` | `basic_atn(n)` | Arctangent |
| `LOG(n)` | `basic_log(n)` | Natural logarithm |
| - | `basic_log10(n)` | Base-10 logarithm |
| `EXP(n)` | `basic_exp(n)` | Exponential (e^n) |
| - | `basic_pow(base, exp)` | Power |
| `RND(n)` | `basic_rnd(n)` | Random (0.0 - 1.0) |
| `RANDOMIZE` | `basic_randomize(seed)` | Set random seed |
| `RANDOMIZE TIME` | `basic_randomize_time()` | Seed from timer |
| - | `basic_rnd_range(min, max)` | Random integer in range |
| - | `basic_min(a, b)` | Minimum |
| - | `basic_max(a, b)` | Maximum |
| - | `basic_clamp(val, min, max)` | Clamp to range |
| - | `basic_deg_to_rad(deg)` | Degrees to radians |
| - | `basic_rad_to_deg(rad)` | Radians to degrees |

### System (system.h)

| MSX BASIC | C Function | Description |
|-----------|-----------|-------------|
| `TIME` | `basic_time()` | Get timer value |
| `TIME=n` | `basic_time_set(n)` | Set timer value |
| `VDP(n)` | `basic_vdp(reg)` | Read VDP register shadow |
| - | `basic_vdp_set(reg, val)` | Write VDP register |
| `VDP(-1)` | `basic_vdp_status()` | Read VDP status S#0 |
| - | `basic_vdp_status_n(reg)` | Read VDP status S#n (MSX2) |
| `VPEEK(a)` | `basic_vpeek(addr)` | Read VRAM (16KB) |
| `VPOKE a,v` | `basic_vpoke(addr, val)` | Write VRAM (16KB) |
| - | `basic_vpeek_ex(addr)` | Read VRAM (128KB, MSX2) |
| - | `basic_vpoke_ex(addr, val)` | Write VRAM (128KB, MSX2) |
| `BASE(n)` | `basic_base(n)` | Get VRAM base address |
| - | `basic_vram_fill(addr, val, count)` | Fill VRAM block |
| - | `basic_vram_write(dest, src, count)` | RAM to VRAM copy |
| - | `basic_vram_read(dest, src, count)` | VRAM to RAM copy |
| - | `basic_wait_vblank()` | Wait for VBlank |
| - | `basic_wait_frames(n)` | Wait n frames |
| `FRE(0)` | `basic_fre()` | Free memory |
| - | `basic_get_msx_type()` | Get MSX type (0-3) |
| - | `basic_is_msx2()` | Check MSX2 or later |
| - | `basic_is_msx2plus()` | Check MSX2+ or later |
| - | `basic_is_turbo_r()` | Check turbo R |
| - | `basic_break_check()` | Check CTRL+STOP |
| - | `basic_break_enable(on)` | Enable/disable CTRL+STOP |
| - | `basic_bios_call(addr)` | Call BIOS routine |
| - | `basic_slot_read(addr, slot, sub)` | Read from slot |
| - | `basic_slot_write(addr, slot, sub, val)` | Write to slot |

### VDP Direct Access (vdp.h)

Low-level V9938/V9958 access for MSX2/MSX2+:

| C Function | Description |
|-----------|-------------|
| `vdp_write_reg(reg, val)` | Write VDP register |
| `vdp_read_status(reg)` | Read status register |
| `vdp_wait_cmd()` | Wait for command completion |
| `vdp_pset(x, y, color, op)` | Set pixel (with logical op) |
| `vdp_line(x1, y1, x2, y2, color, op)` | Draw line (with logical op) |
| `vdp_fill(x, y, w, h, color)` | Fill rectangle (HMMV) |
| `vdp_copy(sx, sy, dx, dy, w, h)` | Copy rectangle (HMMM) |
| `vdp_set_palette(idx, r, g, b)` | Set palette color |
| `vdp_set_display_page(page)` | Set display page |
| `vdp_set_active_page(page)` | Set active page |

## Examples

| File | Description | Requires |
|------|-------------|----------|
| 01_hello.c | Text, LOCATE, TAB, SPC, PRINT USING | MSX1 |
| 02_graphics.c | PSET, LINE, BOX, CIRCLE, ELLIPSE, filled shapes | MSX1 |
| 03_graphics_msx2.c | SCREEN 5/7/8, palette, page flip, COPY | MSX2 |
| 04_sprite.c | Sprite definition, display, animation, collision | MSX1 |
| 05_sound.c | BEEP, PSG control, MML, 3-channel music, SFX | MSX1 |
| 06_input.c | INKEY$, STICK, STRIG, PEEK/POKE, VDP status | MSX1 |
| 07_draw.c | DRAW commands (U/D/L/R/M/A/S/B/N/C) | MSX1 |
| 08_paint.c | PAINT flood fill, GET/PUT, circle arcs | MSX1 |
| 09_string.c | String functions, conversions, manipulation | MSX1 |
| 10_math.c | Math, trigonometry, random numbers | MSX1 |
| 11_system.c | MSX detection, VDP, TIME, VRAM, SCREEN$ | MSX1 |

### Build an Example

```batch
zcc +msx -vn -O3 -compiler=sccz80 -lm ^
    -Iinclude -Llib -lmsxbasic ^
    -subtype=rom examples\01_hello.c -o examples\01_hello -create-app
```

## Directory Structure

```
lib/
├── include/msxbasic/
│   ├── msxbasic.h       # Main header (includes all)
│   ├── screen.h         # Screen control
│   ├── graphics.h       # Graphics & sprites
│   ├── sound.h          # Sound & music
│   ├── input.h          # Input devices
│   ├── bstring.h        # String functions
│   ├── bmath.h          # Math functions
│   ├── system.h         # System & VRAM
│   └── vdp.h            # VDP direct access
├── src/msxbasic/
│   ├── screen.c         # Screen implementation
│   ├── graphics.c       # Graphics implementation
│   ├── sound.c          # Sound implementation
│   ├── input.c          # Input implementation
│   ├── bstring.c        # String implementation
│   ├── bmath.c          # Math implementation
│   ├── system.c         # System implementation
│   └── vdp.c            # VDP implementation
├── lib/
│   └── msxbasic.lib     # Compiled library
├── examples/            # Sample programs
├── docs/
│   ├── reference.html   # API reference (English)
│   └── reference_ja.html # API reference (Japanese)
├── build.bat            # Build script
└── README.md
```

## Technical Notes

- Compiler: Z88DK with **sccz80** backend (`-compiler=sccz80`)
- Calling convention: sccz80 (left-to-right push, `uint8_t` promoted to 16-bit on stack)
- Screen modes 5-12 use VDP hardware commands for drawing
- Screen modes 2-4 use software rendering via BIOS
- SCREEN 6 has tiled palette (colors 0,1 share palettes; colors 2,3 share palettes)

## References

- [MSX-BASIC Command Reference](https://github.com/fu-sen/MSX-BASIC)
- [MSX.org Wiki - SCREEN modes](https://www.msx.org/wiki/)
- [Z88DK - MSX Platform](https://github.com/z88dk/z88dk/wiki/Platform---MSX)
- [V9938 Technical Data](https://www.msx.org/wiki/V9938)

## License

MIT License - Copyright (c) UO Soft

See [LICENSE](LICENSE) for details.
