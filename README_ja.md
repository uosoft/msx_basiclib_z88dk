# MSX BASIC Library for Z88DK

MSX BASICのコマンドや関数をZ88DKのC言語から利用できるライブラリです。
BASIC風のAPIでMSXプログラムをCで記述できます。

[English version](README.md)

## 対応機種

- MSX1 (TMS9918)
- MSX2 (V9938)
- MSX2+ (V9958)
- MSX turbo R

## 必要環境

- [Z88DK](https://z88dk.org/) (最新のnightly build推奨)
- MSXエミュレータ ([openMSX](https://openmsx.org/), blueMSX 等)

## クイックスタート

### ライブラリのビルド

```batch
build.bat
```

### サンプルのビルド

```batch
build_sample.bat 01_hello
```

### プログラムのコンパイル

**ROMカートリッジ:**

```batch
zcc +msx -vn -O3 -compiler=sccz80 -lm ^
    -Iinclude -Llib -lmsxbasic ^
    -subtype=rom -pragma-define:CRT_ORG_BSS=0xC048 ^
    your_program.c -o your_program -create-app
```

**MSX-DOS COMファイル:**

```batch
zcc +msx -vn -O3 -compiler=sccz80 -lm ^
    -Iinclude -Llib -lmsxbasic ^
    -subtype=msxdos your_program.c -o your_program.com
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

## API リファレンス

### 画面制御 (screen.h)

| MSX BASIC | C関数 | 説明 |
|-----------|-------|------|
| `CLS` | `basic_cls()` | 画面クリア |
| `SCREEN n` | `basic_screen(mode)` | 画面モード設定 (0-12) |
| `SCREEN n,s,k` | `basic_screen_ex(mode, sprite, click)` | 画面モード設定（拡張） |
| `COLOR f,b,d` | `basic_color(fg, bg, border)` | 色設定 |
| `COLOR f` | `basic_color_fg(fg)` | 前景色のみ設定 |
| `LOCATE x,y` | `basic_locate(x, y)` | カーソル移動 (0起点) |
| `LOCATE x,y,c` | `basic_locate_ex(x, y, cursor)` | カーソル移動+表示設定 |
| `PRINT` | `basic_print(s)` | 文字列表示 |
| `PRINT` | `basic_println(s)` | 文字列表示+改行 |
| `PRINT n` | `basic_print_int(n)` | 整数表示 |
| `PRINT CHR$(c)` | `basic_print_char(c)` | 文字表示 |
| `WIDTH n` | `basic_width(w)` | 画面幅設定 |
| `CSRLIN` | `basic_csrlin()` | カーソル行取得 (1起点) |
| `POS(0)` | `basic_pos(0)` | カーソル列取得 (1起点) |
| `TAB(n)` | `basic_tab(n)` | 指定列へ移動 |
| `SPC(n)` | `basic_spc(n)` | n個のスペース出力 |
| `SCREEN$(x,y)` | `basic_screen_char(x, y)` | 画面上の文字を取得 |
| `COLOR=(p,r,g,b)` | `basic_set_palette(p, r, g, b)` | パレット設定 (MSX2) |
| `PRINT USING` | `basic_print_using_int(dest, fmt, val)` | 整数のフォーマット出力 |
| `PRINT USING` | `basic_print_using_float(dest, fmt, val)` | 実数のフォーマット出力 |
| `PRINT USING` | `basic_print_using_str(dest, fmt, val)` | 文字列のフォーマット出力 |
| `WAIT` | `basic_wait(port, and, xor)` | I/Oポート値の待機 |
| - | `basic_get_screen_mode()` | 現在の画面モード取得 |
| - | `basic_cursor(visible)` | カーソル表示/非表示 |

### グラフィックス (graphics.h)

#### 描画

| MSX BASIC | C関数 | 説明 |
|-----------|-------|------|
| `PSET (x,y),c` | `basic_pset(x, y, color)` | 点を打つ |
| `PSET (x,y)` | `basic_pset_c(x, y)` | 点を打つ（前景色） |
| `PSET STEP(dx,dy)` | `basic_pset_step(dx, dy, color)` | 相対座標で点を打つ |
| `PRESET (x,y)` | `basic_preset(x, y)` | 点を消す（背景色） |
| `LINE (x1,y1)-(x2,y2),c` | `basic_line(x1, y1, x2, y2, color)` | 線描画 |
| `LINE ...,B` | `basic_box(x1, y1, x2, y2, color)` | 矩形描画 |
| `LINE ...,BF` | `basic_boxfill(x1, y1, x2, y2, color)` | 塗りつぶし矩形 |
| `CIRCLE (x,y),r,c` | `basic_circle(x, y, r, color)` | 円描画 |
| `CIRCLE ...,s,e,a` | `basic_circle_ex(x, y, r, c, start, end, aspect)` | 円弧/楕円描画 |
| - | `basic_ellipse(x, y, rx, ry, color)` | 楕円描画 |
| - | `basic_circle_fill(x, y, r, color)` | 塗りつぶし円 |
| - | `basic_ellipse_fill(x, y, rx, ry, color)` | 塗りつぶし楕円 |
| `PAINT (x,y),c,b` | `basic_paint(x, y, color, border)` | 塗りつぶし |
| `PAINT (x,y),c` | `basic_paint_c(x, y, color)` | 塗りつぶし（色=境界色） |
| `DRAW cmd$` | `basic_draw(cmd)` | DRAWコマンド実行 |
| `POINT(x,y)` | `basic_point(x, y)` | 点の色を取得 |
| - | `basic_init_grp()` | SCREEN 2カラーテーブル初期化 |

**DRAWコマンド一覧:** `U`(上), `D`(下), `L`(左), `R`(右), `E`(右上), `F`(右下), `G`(左下), `H`(左上), `M`x,y(移動), `B`(ペンアップ), `N`(位置更新なし), `C`n(色変更), `A`n(角度 0-3), `S`n(スケール)

#### GET/PUT ブロック操作

| MSX BASIC | C関数 | 説明 |
|-----------|-------|------|
| - | `basic_get_size(w, h)` | 必要バッファサイズ計算 |
| `GET (x1,y1)-(x2,y2),A` | `basic_get(x1, y1, x2, y2, buf)` | 画面領域をバッファに保存 |
| `PUT (x,y),A,op` | `basic_put(x, y, buf, op)` | バッファから画面に復元 |

PUT操作モード: `PUT_PSET`(0:上書き), `PUT_AND`(1), `PUT_OR`(2), `PUT_XOR`(3), `PUT_PRESET`(4:反転)

#### スプライト

| MSX BASIC | C関数 | 説明 |
|-----------|-------|------|
| `SCREEN n,size` | `basic_sprite_size(size)` | スプライトサイズ設定 (0-3) |
| `SPRITE$(n)=pat$` | `basic_sprite_pattern(n, data)` | スプライトパターン定義 |
| `PUT SPRITE n,(x,y),c,p` | `basic_put_sprite(n, x, y, color, pat)` | スプライト表示 |
| - | `basic_sprite_off(n)` | スプライト非表示 |
| - | `basic_sprites_off()` | 全スプライト非表示 |
| - | `basic_sprite_collision()` | 衝突判定 |

#### COPY / ページ (MSX2)

| MSX BASIC | C関数 | 説明 |
|-----------|-------|------|
| `COPY` | `basic_copy(sx, sy, w, h, dx, dy)` | 画面領域コピー |
| `COPY ...TO ...,page` | `basic_copy_page(sx, sy, w, h, sp, dx, dy, dp)` | ページ間コピー |
| `SET PAGE d,a` | `basic_set_page(display, active)` | 表示/アクティブページ設定 |

### サウンド (sound.h)

#### 基本サウンド

| MSX BASIC | C関数 | 説明 |
|-----------|-------|------|
| `BEEP` | `basic_beep()` | ビープ音 |
| `SOUND r,v` | `basic_sound(reg, val)` | PSGレジスタ書き込み |
| - | `basic_sound_read(reg)` | PSGレジスタ読み出し |
| - | `basic_sound_off()` | 全チャンネル消音 |

#### 音楽 (MML)

| MSX BASIC | C関数 | 説明 |
|-----------|-------|------|
| `PLAY s$` | `basic_play(mml)` | MML演奏（チャンネルA） |
| `PLAY a$,b$,c$` | `basic_play_3ch(a, b, c)` | 3チャンネル同時演奏 |
| - | `basic_play_note(ch, note, oct, dur)` | 単音再生 |

**MMLコマンド:** `C`-`B`(音名), `R`(休符), `O`n(オクターブ), `L`n(音長), `T`n(テンポ), `V`n(音量), `Q`n(ゲートタイム), `<` `>`(オクターブ上下), `#`/`+`(シャープ), `-`(フラット), `.`(付点)

#### トーン制御

| C関数 | 説明 |
|-------|------|
| `basic_set_tone(ch, freq)` | チャンネル周波数設定 |
| `basic_set_volume(ch, vol)` | チャンネル音量設定 (0-15) |
| `basic_set_noise(freq)` | ノイズ周波数設定 |
| `basic_set_mixer(tone, noise)` | トーン/ノイズ有効化設定 |
| `basic_set_envelope(period, shape)` | エンベロープ設定 |

#### 効果音

| C関数 | 説明 |
|-------|------|
| `basic_sfx_explosion()` | 爆発音 |
| `basic_sfx_laser()` | レーザー音 |
| `basic_sfx_jump()` | ジャンプ音 |
| `basic_sfx_coin()` | コイン取得音 |
| `basic_sfx_hit()` | ヒット音 |

### 入力 (input.h)

| MSX BASIC | C関数 | 説明 |
|-----------|-------|------|
| `INKEY$` | `basic_inkey()` | キー入力（ノンブロック） |
| `INPUT$(n)` | `basic_input_str(buf, n)` | n文字入力 |
| `INPUT a$` | `basic_input_line(buf, max)` | 1行入力 |
| `STICK(n)` | `basic_stick(device)` | ジョイスティック方向 (0-8) |
| `STRIG(n)` | `basic_strig(button)` | トリガーボタン (0/-1) |
| - | `basic_strig_bool(button)` | トリガーボタン (0/1) |
| `PADDLE(n)` | `basic_paddle(n)` | パドル位置 |
| `PEEK(a)` | `basic_peek(addr)` | メモリ1バイト読み出し |
| `POKE a,v` | `basic_poke(addr, val)` | メモリ1バイト書き込み |
| - | `basic_peek16(addr)` | メモリ2バイト読み出し |
| - | `basic_poke16(addr, val)` | メモリ2バイト書き込み |
| `INP(p)` | `basic_inp(port)` | I/Oポート読み出し |
| `OUT p,v` | `basic_out(port, val)` | I/Oポート書き込み |
| - | `basic_wait_key()` | キー入力待ち |
| - | `basic_key_pressed(row, bit)` | 特定キーの押下判定 |
| - | `basic_clear_key_buffer()` | キーバッファクリア |
| `KEY n,s$` | `basic_key_def(n, str)` | ファンクションキー定義 |
| `KEY ON/OFF` | `basic_key_display(show)` | ファンクションキー表示 |

STICKデバイス定数: `STICK_KEYBOARD`(0), `STICK_JOY1`(1), `STICK_JOY2`(2)
STICK方向定数: `STICK_UP`(1), `STICK_UP_RIGHT`(2), `STICK_RIGHT`(3), `STICK_DOWN_RIGHT`(4), `STICK_DOWN`(5), `STICK_DOWN_LEFT`(6), `STICK_LEFT`(7), `STICK_UP_LEFT`(8)
STRIG定数: `STRIG_SPACE`(0), `STRIG_JOY1_A`(1), `STRIG_JOY2_A`(2), `STRIG_JOY1_B`(3), `STRIG_JOY2_B`(4)

### 文字列関数 (bstring.h)

| MSX BASIC | C関数 | 説明 |
|-----------|-------|------|
| `LEN(s$)` | `basic_len(s)` | 文字列の長さ |
| `LEFT$(s$,n)` | `basic_left(dest, src, n)` | 左からn文字抽出 |
| `RIGHT$(s$,n)` | `basic_right(dest, src, n)` | 右からn文字抽出 |
| `MID$(s$,p,n)` | `basic_mid(dest, src, pos, n)` | 中間文字列抽出 |
| `MID$(s$,p,n)=r$` | `basic_mid_replace(dest, pos, n, src)` | 文字列置換 |
| `ASC(s$)` | `basic_asc(s)` | 文字コード取得 |
| `CHR$(n)` | `basic_chr(dest, code)` | 文字コードから文字生成 |
| `STR$(n)` | `basic_str(dest, n)` | 整数を文字列に変換 |
| `STR$(n)` | `basic_str_long(dest, n)` | 長整数を文字列に変換 |
| `STR$(n)` | `basic_str_float(dest, n)` | 実数を文字列に変換 |
| `VAL(s$)` | `basic_val(s)` | 文字列を整数に変換 |
| `VAL(s$)` | `basic_val_long(s)` | 文字列を長整数に変換 |
| `VAL(s$)` | `basic_val_float(s)` | 文字列を実数に変換（指数表記対応） |
| `HEX$(n)` | `basic_hex(dest, n)` | 16進文字列に変換 |
| `BIN$(n)` | `basic_bin(dest, n)` | 2進文字列に変換 |
| `OCT$(n)` | `basic_oct(dest, n)` | 8進文字列に変換 |
| `INSTR(s$,t$)` | `basic_instr(s, search, start)` | 文字列検索 |
| `STRING$(n,c)` | `basic_string(dest, n, code)` | 文字の繰り返し |
| `SPACE$(n)` | `basic_space(dest, n)` | スペース文字列生成 |
| - | `basic_ucase(s)` | 大文字変換（その場） |
| - | `basic_lcase(s)` | 小文字変換（その場） |
| - | `basic_strcmp(s1, s2)` | 文字列比較 |
| - | `basic_concat(dest, s1, s2)` | 文字列結合 |
| - | `basic_jis_to_sjis(dest, src)` | JIS → Shift-JIS変換 |
| - | `basic_sjis_to_jis(dest, src)` | Shift-JIS → JIS変換 |

### 数学関数 (bmath.h)

| MSX BASIC | C関数 | 説明 |
|-----------|-------|------|
| `ABS(n)` | `basic_abs(n)` / `basic_abs_float(n)` | 絶対値 |
| `SGN(n)` | `basic_sgn(n)` / `basic_sgn_float(n)` | 符号 (-1, 0, 1) |
| `INT(n)` | `basic_int(n)` | 切り捨て (floor) |
| `FIX(n)` | `basic_fix(n)` | ゼロ方向への切り捨て |
| `CINT(n)` | `basic_cint(n)` | 四捨五入して整数化 |
| `CSNG(n)` | `basic_csng(n)` | 単精度に変換 |
| `CDBL(n)` | `basic_cdbl(n)` | 倍精度に変換 |
| `SQR(n)` | `basic_sqr(n)` | 平方根 |
| `SIN(n)` | `basic_sin(n)` | 正弦（ラジアン） |
| `COS(n)` | `basic_cos(n)` | 余弦（ラジアン） |
| `TAN(n)` | `basic_tan(n)` | 正接（ラジアン） |
| `ATN(n)` | `basic_atn(n)` | 逆正接 |
| `LOG(n)` | `basic_log(n)` | 自然対数 |
| - | `basic_log10(n)` | 常用対数 |
| `EXP(n)` | `basic_exp(n)` | 指数関数 (e^n) |
| - | `basic_pow(base, exp)` | べき乗 |
| `RND(n)` | `basic_rnd(n)` | 乱数 (0.0〜1.0) |
| `RANDOMIZE` | `basic_randomize(seed)` | 乱数シード設定 |
| `RANDOMIZE TIME` | `basic_randomize_time()` | タイマーからシード設定 |
| - | `basic_rnd_range(min, max)` | 範囲指定の整数乱数 |
| - | `basic_min(a, b)` | 最小値 |
| - | `basic_max(a, b)` | 最大値 |
| - | `basic_clamp(val, min, max)` | 範囲内に制限 |
| - | `basic_deg_to_rad(deg)` | 度→ラジアン変換 |
| - | `basic_rad_to_deg(rad)` | ラジアン→度変換 |

### システム (system.h)

| MSX BASIC | C関数 | 説明 |
|-----------|-------|------|
| `TIME` | `basic_time()` | タイマー値取得 |
| `TIME=n` | `basic_time_set(n)` | タイマー値設定 |
| `VDP(n)` | `basic_vdp(reg)` | VDPレジスタシャドウ読み出し |
| - | `basic_vdp_set(reg, val)` | VDPレジスタ書き込み |
| `VDP(-1)` | `basic_vdp_status()` | VDPステータス S#0 読み出し |
| - | `basic_vdp_status_n(reg)` | VDPステータス S#n 読み出し (MSX2) |
| `VPEEK(a)` | `basic_vpeek(addr)` | VRAM読み出し (16KB) |
| `VPOKE a,v` | `basic_vpoke(addr, val)` | VRAM書き込み (16KB) |
| - | `basic_vpeek_ex(addr)` | VRAM読み出し (128KB, MSX2) |
| - | `basic_vpoke_ex(addr, val)` | VRAM書き込み (128KB, MSX2) |
| `BASE(n)` | `basic_base(n)` | VRAMベースアドレス取得 |
| - | `basic_vram_fill(addr, val, count)` | VRAMブロック充填 |
| - | `basic_vram_write(dest, src, count)` | RAM→VRAMブロック転送 |
| - | `basic_vram_read(dest, src, count)` | VRAM→RAMブロック転送 |
| - | `basic_wait_vblank()` | VBlank待ち |
| - | `basic_wait_frames(n)` | nフレーム待ち |
| `FRE(0)` | `basic_fre()` | 空きメモリ量 |
| - | `basic_get_msx_type()` | MSXタイプ取得 (0-3) |
| - | `basic_is_msx2()` | MSX2以上か判定 |
| - | `basic_is_msx2plus()` | MSX2+以上か判定 |
| - | `basic_is_turbo_r()` | turbo Rか判定 |
| - | `basic_break_check()` | CTRL+STOP判定 |
| - | `basic_break_enable(on)` | CTRL+STOP有効/無効 |
| - | `basic_bios_call(addr)` | BIOSルーチン呼び出し |
| - | `basic_slot_read(addr, slot, sub)` | スロットからの読み出し |
| - | `basic_slot_write(addr, slot, sub, val)` | スロットへの書き込み |

### VDP直接アクセス (vdp.h)

MSX2/MSX2+向けのV9938/V9958低レベルアクセス:

| C関数 | 説明 |
|-------|------|
| `vdp_write_reg(reg, val)` | VDPレジスタ書き込み |
| `vdp_read_status(reg)` | ステータスレジスタ読み出し |
| `vdp_wait_cmd()` | コマンド完了待ち |
| `vdp_pset(x, y, color, op)` | ピクセル設定（論理演算付き） |
| `vdp_line(x1, y1, x2, y2, color, op)` | 線描画（論理演算付き） |
| `vdp_fill(x, y, w, h, color)` | 矩形充填 (HMMV) |
| `vdp_copy(sx, sy, dx, dy, w, h)` | 矩形コピー (HMMM) |
| `vdp_set_palette(idx, r, g, b)` | パレット色設定 |
| `vdp_set_display_page(page)` | 表示ページ設定 |
| `vdp_set_active_page(page)` | アクティブページ設定 |

## サンプルプログラム

| ファイル | 内容 | 対応機種 |
|---------|------|---------|
| 01_hello.c | テキスト表示、LOCATE、TAB、SPC、PRINT USING | MSX1〜 |
| 02_graphics.c | PSET、LINE、BOX、CIRCLE、ELLIPSE、塗りつぶし | MSX1〜 |
| 03_graphics_msx2.c | SCREEN 5/7/8、パレット、ページ切替、COPY | MSX2〜 |
| 04_sprite.c | スプライト定義・表示・アニメーション・衝突判定 | MSX1〜 |
| 05_sound.c | BEEP、PSG制御、MML演奏、3ch同時演奏、効果音 | MSX1〜 |
| 06_input.c | INKEY$、STICK、STRIG、PEEK/POKE、VDPステータス | MSX1〜 |
| 07_draw.c | DRAWコマンド全機能 (U/D/L/R/M/A/S/B/N/C) | MSX1〜 |
| 08_paint.c | PAINT塗りつぶし、GET/PUT、円弧描画 | MSX1〜 |
| 09_string.c | 文字列関数、型変換、文字列操作 | MSX1〜 |
| 10_math.c | 数学関数、三角関数、乱数 | MSX1〜 |
| 11_system.c | MSX判別、VDP、TIME、VRAM操作、SCREEN$ | MSX1〜 |

### サンプルのビルド

**ROM:**

```batch
zcc +msx -vn -O3 -compiler=sccz80 -lm ^
    -Iinclude -Llib -lmsxbasic ^
    -subtype=rom -pragma-define:CRT_ORG_BSS=0xC048 ^
    examples\01_hello.c -o examples\01_hello -create-app
```

**MSX-DOS COM:**

```batch
zcc +msx -vn -O3 -compiler=sccz80 -lm ^
    -Iinclude -Llib -lmsxbasic ^
    -subtype=msxdos examples\01_hello.c -o examples\01_hello.com
```

## ディレクトリ構造

```
lib/
├── include/msxbasic/
│   ├── msxbasic.h       # メインヘッダ（全モジュールをインクルード）
│   ├── screen.h         # 画面制御
│   ├── graphics.h       # グラフィックス・スプライト
│   ├── sound.h          # サウンド・音楽
│   ├── input.h          # 入力デバイス
│   ├── bstring.h        # 文字列関数
│   ├── bmath.h          # 数学関数
│   ├── system.h         # システム・VRAM
│   └── vdp.h            # VDP直接アクセス
├── src/msxbasic/
│   ├── screen.c         # 画面制御の実装
│   ├── graphics.c       # グラフィックスの実装
│   ├── sound.c          # サウンドの実装
│   ├── input.c          # 入力の実装
│   ├── bstring.c        # 文字列関数の実装
│   ├── bmath.c          # 数学関数の実装
│   ├── system.c         # システムの実装
│   └── vdp.c            # VDPの実装
├── lib/
│   └── msxbasic.lib     # コンパイル済みライブラリ
├── examples/            # サンプルプログラム
├── docs/
│   ├── reference.html   # APIリファレンス（英語）
│   └── reference_ja.html # APIリファレンス（日本語）
├── build.bat            # ビルドスクリプト
├── README.md            # 英語版README
└── README_ja.md         # 日本語版README
```

## 技術情報

- コンパイラ: Z88DK **sccz80** バックエンド (`-compiler=sccz80`)
- 呼び出し規約: sccz80（左→右プッシュ、`uint8_t`はスタック上で16ビットに昇格）
- SCREEN 5-12: VDPハードウェアコマンドで描画
- SCREEN 2-4: BIOSによるソフトウェア描画
- SCREEN 6: タイルドパレット方式（色0,1がパレット0,1を共有、色2,3がパレット2,3を共有）

## 参考資料

- [MSX-BASIC コマンドリファレンス](https://github.com/fu-sen/MSX-BASIC)
- [MSX.org Wiki - スクリーンモード](https://www.msx.org/wiki/)
- [Z88DK - MSXプラットフォーム](https://github.com/z88dk/z88dk/wiki/Platform---MSX)
- [V9938 技術資料](https://www.msx.org/wiki/V9938)

## ライセンス

MIT License - Copyright (c) UO Soft

詳細は [LICENSE](LICENSE) を参照してください。
