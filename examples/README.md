# MSX BASIC Library - Examples

## Example List

| File | Description | Features | Requires |
|------|-------------|----------|----------|
| 01_hello.c | Text output demo | SCREEN, COLOR, CLS, LOCATE, PRINT, TAB, SPC, PRINT USING | MSX1 |
| 02_graphics.c | MSX1 graphics | PSET, LINE, BOX, BOXFILL, CIRCLE, ELLIPSE, filled shapes | MSX1 |
| 03_graphics_msx2.c | MSX2 graphics | SCREEN 5/7/8, palette, page flip, COPY | MSX2 |
| 04_sprite.c | Sprite demo | Sprite patterns, PUT SPRITE, animation, collision | MSX1 |
| 05_sound.c | Sound and music | BEEP, PSG, MML (PLAY), 3-channel, sound effects | MSX1 |
| 06_input.c | Input devices | INKEY$, STICK, STRIG, PEEK/POKE, INP/OUT | MSX1 |
| 07_draw.c | DRAW command | U/D/L/R, M (move), A (angle), S (scale), B/N, C (color) | MSX1 |
| 08_paint.c | PAINT and GET/PUT | Flood fill, GET/PUT block operations, circle arcs | MSX1 |
| 09_string.c | String functions | LEN, LEFT$, RIGHT$, MID$, STR$, VAL, HEX$, BIN$, OCT$ | MSX1 |
| 10_math.c | Math functions | ABS, SIN, COS, TAN, SQR, LOG, EXP, RND, MIN/MAX | MSX1 |
| 11_system.c | System functions | MSX detection, VDP, TIME, VRAM, SCREEN$, BASE | MSX1 |

## Building

### Build the library first

```batch
build.bat
```

### Build an example

```batch
build_sample.bat 01_hello
```

This will create `examples/01_hello.rom`.

### Using zcc directly

**ROM cartridge:**

```batch
zcc +msx -vn -O3 -compiler=sccz80 -lm ^
    -Iinclude -Llib -lmsxbasic ^
    -subtype=rom examples/01_hello.c -o examples/01_hello -create-app
```

**MSX-DOS COM file:**

```batch
zcc +msx -vn -O3 -compiler=sccz80 -lm ^
    -Iinclude -Llib -lmsxbasic ^
    -subtype=msxdos examples/01_hello.c -o examples/01_hello.com
```

## Requirements

- [Z88DK](https://z88dk.org/) compiler (zcc)
- MSX emulator ([openMSX](https://openmsx.org/), [blueMSX](http://www.bluemsx.com/), etc.) for testing

## Notes

- Example 03 requires MSX2 or later
- `build_sample.bat` builds 32KB ROM cartridge (.rom)
- Examples can also be built as MSX-DOS COM files (.com)
- Examples 01, 02, 04-11 are compatible with MSX1
