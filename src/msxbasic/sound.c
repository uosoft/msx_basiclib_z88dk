/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file sound.c
 * @brief MSX BASIC sound functions (Z88DK compatible)
 */

#include <stdint.h>
#include "../../include/msxbasic/sound.h"

/* BIOS wrappers */
#asm

PUBLIC _snd_beep
PUBLIC _snd_wrtpsg
PUBLIC _snd_rdpsg
PUBLIC _snd_gtstck
PUBLIC _snd_gttrig

_snd_beep:
    ret             ; Not used (basic_beep is PSG-based now)

; void snd_wrtpsg(uint8_t reg, uint8_t val)
; Direct I/O port access (works in both ROM and MSX-DOS)
_snd_wrtpsg:
    ld hl, 2
    add hl, sp
    ld e, (hl)      ; val
    inc hl
    inc hl
    ld a, (hl)      ; reg
    out (0xA0), a   ; PSG register select
    ld a, e
    out (0xA1), a   ; PSG data write
    ret

; uint8_t snd_rdpsg(uint8_t reg)
; Direct I/O port access
_snd_rdpsg:
    ld hl, 2
    add hl, sp
    ld a, (hl)      ; reg
    out (0xA0), a   ; PSG register select
    in a, (0xA2)    ; PSG data read
    ld l, a
    ret

; uint8_t snd_gtstck(uint8_t stick)
_snd_gtstck:
    ld hl, 2
    add hl, sp
    ld a, (hl)
    ld hl, 0x00D5
    ld (0xC02C), hl
    call 0xC000     ; GTSTCK via trampoline
    ld l, a
    ret

; uint8_t snd_gttrig(uint8_t trigger)
_snd_gttrig:
    ld hl, 2
    add hl, sp
    ld a, (hl)
    ld hl, 0x00D8
    ld (0xC02C), hl
    call 0xC000     ; GTTRIG via trampoline
    ld l, a
    ret

#endasm

extern void snd_beep(void);
extern void snd_wrtpsg(uint8_t reg, uint8_t val);
extern uint8_t snd_rdpsg(uint8_t reg);
extern uint8_t snd_gtstck(uint8_t stick);
extern uint8_t snd_gttrig(uint8_t trigger);

/* PSG frequency table for notes C to B (octave 4, A4=440Hz)
 * PSG freq = 3579545 / (16 * frequency)
 * Values for octave 4: C4=262Hz, D4=294Hz, E4=330Hz, F4=349Hz, G4=392Hz, A4=440Hz, B4=494Hz
 */
static const uint16_t note_freq[12] = {
    851,    /* C  */
    803,    /* C# */
    758,    /* D  */
    715,    /* D# */
    675,    /* E  */
    637,    /* F  */
    601,    /* F# */
    568,    /* G  */
    536,    /* G# */
    506,    /* A  */
    478,    /* A# */
    451     /* B  */
};

/* MML state for each channel */
static uint8_t mml_octave[3] = {4, 4, 4};
static uint8_t mml_length[3] = {4, 4, 4};
static uint8_t mml_tempo = 120;
static uint8_t mml_volume[3] = {8, 8, 8};
static uint8_t mml_envelope_shape = 0;
static uint16_t mml_envelope_period = 255;
static uint8_t mml_gate[3] = {8, 8, 8};  /* Q command: gate time 1-8 (8 = full length) */

void basic_beep(void) {
    uint8_t i;

    /* Direct PSG beep (BIOS BEEP may not work in ROM cartridge) */
    basic_sound(PSG_MIXER, 0xBE);  /* Tone A only */
    basic_set_tone(0, 170);         /* ~1316 Hz (standard MSX BEEP) */
    basic_set_volume(0, 13);

    for (i = 0; i < 10; i++) {     /* ~1/6 sec at 60Hz */
        #asm
        halt
        #endasm
    }

    basic_sound_off();
}

void basic_sound(uint8_t reg, uint8_t value) {
    snd_wrtpsg(reg, value);
}

uint8_t basic_sound_read(uint8_t reg) {
    return snd_rdpsg(reg);
}

/* Parse a number from MML string */
static uint16_t parse_number(const char** mml) {
    uint16_t num = 0;
    while (**mml >= '0' && **mml <= '9') {
        num = num * 10 + (**mml - '0');
        (*mml)++;
    }
    return num;
}

/* Calculate duration in frames (1/60 sec) from note length */
static uint16_t calc_duration(uint8_t length, uint8_t dotted) {
    /* At 60fps: quarter note at 120 BPM = 30 frames (0.5 sec) */
    /* frames = (60fps * 60sec / tempo) * (4 / length) */
    uint16_t quarter = (uint16_t)30 * 120 / mml_tempo;
    uint16_t dur = (quarter * 4) / length;
    if (dotted) {
        dur = dur + dur / 2;  /* Dotted note = 1.5x length */
    }
    return dur;
}

/* Get frequency divider for note number (0-95) */
static uint16_t get_note_freq(uint8_t note_num) {
    uint8_t octave, semitone;
    uint16_t freq;

    if (note_num == 0) return 0;  /* Rest */
    if (note_num > 96) note_num = 96;

    note_num--;  /* Convert 1-96 to 0-95 */
    octave = note_num / 12 + 1;
    semitone = note_num % 12;

    freq = note_freq[semitone];

    /* Adjust for octave (base table is octave 4) */
    if (octave < 4) {
        freq <<= (4 - octave);
    } else if (octave > 4) {
        freq >>= (octave - 4);
    }

    return freq;
}

/* Play a single note on a channel */
void basic_play_note(uint8_t channel, uint8_t note, uint8_t octave, uint16_t duration_ms) {
    uint16_t freq;
    uint16_t wait;
    uint16_t gate_time;
    uint8_t mixer;

    if (channel > 2 || note > 11) return;

    /* Calculate frequency for octave */
    freq = note_freq[note];
    if (octave < 4) {
        freq <<= (4 - octave);
    } else if (octave > 4) {
        freq >>= (octave - 4);
    }

    /* Set tone */
    basic_set_tone(channel, freq);

    /* Set volume (or envelope mode) */
    if (mml_volume[channel] == 16) {
        /* Use envelope */
        basic_sound(PSG_VOL_A + channel, 0x10);
        basic_sound(PSG_ENV_LOW, mml_envelope_period & 0xFF);
        basic_sound(PSG_ENV_HIGH, (mml_envelope_period >> 8) & 0xFF);
        basic_sound(PSG_ENV_SHAPE, mml_envelope_shape);
    } else {
        basic_set_volume(channel, mml_volume[channel]);
    }

    /* Enable tone on channel */
    mixer = basic_sound_read(PSG_MIXER);
    mixer &= ~(1 << channel);  /* Enable tone */
    basic_sound(PSG_MIXER, mixer);

    /* Calculate gate time (Q command) */
    gate_time = (duration_ms * mml_gate[channel]) / 8;

    /* Wait for gate time (note sounds) */
    for (wait = 0; wait < gate_time; wait++) {
        #asm
        halt
        #endasm
    }

    /* Turn off channel for remaining time */
    basic_set_volume(channel, 0);

    /* Wait for remaining duration */
    for (wait = gate_time; wait < duration_ms; wait++) {
        #asm
        halt
        #endasm
    }
}

/* Full MML parser */
void basic_play(const char* mml) {
    uint8_t ch = 0;
    uint8_t note;
    uint16_t duration;
    uint16_t num;
    uint8_t dotted;
    uint8_t note_num;

    /* Reset to defaults */
    mml_octave[0] = 4;
    mml_length[0] = 4;
    mml_volume[0] = 8;
    mml_gate[0] = 8;

    /* Enable tone on channel A, disable noise */
    basic_sound(PSG_MIXER, 0xB8);

    while (*mml) {
        char c = *mml++;
        int8_t modifier = 0;
        dotted = 0;

        /* Skip whitespace */
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            continue;
        }

        /* Check for sharp/flat after note letter */
        if ((c >= 'A' && c <= 'G') || (c >= 'a' && c <= 'g')) {
            if (*mml == '+' || *mml == '#') {
                modifier = 1;
                mml++;
            } else if (*mml == '-') {
                modifier = -1;
                mml++;
            }
        }

        switch (c) {
            case 'C': case 'c': note = 0; break;
            case 'D': case 'd': note = 2; break;
            case 'E': case 'e': note = 4; break;
            case 'F': case 'f': note = 5; break;
            case 'G': case 'g': note = 7; break;
            case 'A': case 'a': note = 9; break;
            case 'B': case 'b': note = 11; break;

            case 'N': case 'n':
                /* Note by number (0-96) */
                num = parse_number(&mml);
                if (num == 0) {
                    /* N0 is rest */
                    duration = calc_duration(mml_length[ch], 0);
                    for (num = 0; num < duration; num++) {
                        #asm
                        halt
                        #endasm
                    }
                } else if (num <= 96) {
                    /* Play note by number */
                    uint16_t freq = get_note_freq((uint8_t)num);
                    if (freq > 0) {
                        basic_set_tone(ch, freq);
                        basic_set_volume(ch, mml_volume[ch]);
                        basic_sound(PSG_MIXER, basic_sound_read(PSG_MIXER) & ~(1 << ch));
                        duration = calc_duration(mml_length[ch], 0);
                        for (num = 0; num < duration; num++) {
                            #asm
                            halt
                            #endasm
                        }
                        basic_set_volume(ch, 0);
                    }
                }
                continue;

            case 'R': case 'r':
                /* Rest */
                num = parse_number(&mml);
                if (num == 0) num = mml_length[ch];
                /* Check for dot */
                if (*mml == '.') {
                    dotted = 1;
                    mml++;
                }
                duration = calc_duration((uint8_t)num, dotted);
                /* Silence during rest */
                basic_set_volume(ch, 0);
                for (num = 0; num < duration; num++) {
                    #asm
                    halt
                    #endasm
                }
                continue;

            case 'O': case 'o':
                num = parse_number(&mml);
                if (num >= 1 && num <= 8) mml_octave[ch] = (uint8_t)num;
                continue;

            case 'L': case 'l':
                num = parse_number(&mml);
                if (num >= 1 && num <= 64) mml_length[ch] = (uint8_t)num;
                continue;

            case 'T': case 't':
                num = parse_number(&mml);
                if (num >= 32 && num <= 255) mml_tempo = (uint8_t)num;
                continue;

            case 'V': case 'v':
                num = parse_number(&mml);
                if (num <= 15) mml_volume[ch] = (uint8_t)num;
                continue;

            case 'S': case 's':
                /* Envelope shape (0-15) */
                num = parse_number(&mml);
                if (num <= 15) {
                    mml_envelope_shape = (uint8_t)num;
                    /* Enable envelope mode for this channel */
                    mml_volume[ch] = 16;
                }
                continue;

            case 'M': case 'm':
                /* Envelope period (1-65535) */
                num = parse_number(&mml);
                if (num >= 1) {
                    mml_envelope_period = num;
                }
                continue;

            case 'Q': case 'q':
                /* Gate time (1-8) */
                num = parse_number(&mml);
                if (num >= 1 && num <= 8) mml_gate[ch] = (uint8_t)num;
                continue;

            case '<':
                if (mml_octave[ch] > 1) mml_octave[ch]--;
                continue;

            case '>':
                if (mml_octave[ch] < 8) mml_octave[ch]++;
                continue;

            case '&':
                /* Tie - skip, handled by not adding silence */
                continue;

            default:
                continue;
        }

        /* This is a note (C-B) */
        /* Apply sharp/flat modifier */
        note = (uint8_t)((note + modifier + 12) % 12);

        /* Get note length if specified */
        num = parse_number(&mml);
        if (num == 0) num = mml_length[ch];

        /* Check for dotted note */
        if (*mml == '.') {
            dotted = 1;
            mml++;
        }

        duration = calc_duration((uint8_t)num, dotted);

        /* Play the note */
        basic_play_note(ch, note, mml_octave[ch], duration);
    }

    /* Turn off sound */
    basic_set_volume(0, 0);
}

/* MML channel state for simultaneous playback */
typedef struct {
    const char* mml;       /* Current position in MML string */
    uint16_t remaining;    /* Frames remaining for current note */
    uint16_t gate_end;     /* Frame when note should stop (gate time) */
    uint8_t octave;
    uint8_t length;
    uint8_t volume;
    uint8_t gate;
    uint8_t active;        /* Is this channel active? */
} MmlChannel;

/* Parse next note from MML and set channel state */
static void parse_mml_note(uint8_t ch, MmlChannel* state) {
    const char* mml = state->mml;
    uint8_t note = 0xFF;
    int8_t modifier = 0;
    uint8_t dotted = 0;
    uint16_t num;
    uint16_t freq;
    uint16_t duration;

    if (!mml || !*mml) {
        state->active = 0;
        return;
    }

    while (*mml) {
        char c = *mml++;

        /* Skip whitespace */
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') continue;

        /* Check for sharp/flat after note letter */
        if ((c >= 'A' && c <= 'G') || (c >= 'a' && c <= 'g')) {
            if (*mml == '+' || *mml == '#') { modifier = 1; mml++; }
            else if (*mml == '-') { modifier = -1; mml++; }
        }

        switch (c) {
            case 'C': case 'c': note = 0; break;
            case 'D': case 'd': note = 2; break;
            case 'E': case 'e': note = 4; break;
            case 'F': case 'f': note = 5; break;
            case 'G': case 'g': note = 7; break;
            case 'A': case 'a': note = 9; break;
            case 'B': case 'b': note = 11; break;

            case 'R': case 'r':
                /* Rest */
                num = 0;
                while (*mml >= '0' && *mml <= '9') {
                    num = num * 10 + (*mml - '0'); mml++;
                }
                if (num == 0) num = state->length;
                if (*mml == '.') { dotted = 1; mml++; }
                duration = calc_duration((uint8_t)num, dotted);
                state->mml = mml;
                state->remaining = duration;
                state->gate_end = 0;  /* No sound during rest */
                basic_set_volume(ch, 0);
                return;

            case 'O': case 'o':
                num = 0;
                while (*mml >= '0' && *mml <= '9') {
                    num = num * 10 + (*mml - '0'); mml++;
                }
                if (num >= 1 && num <= 8) state->octave = (uint8_t)num;
                continue;

            case 'L': case 'l':
                num = 0;
                while (*mml >= '0' && *mml <= '9') {
                    num = num * 10 + (*mml - '0'); mml++;
                }
                if (num >= 1 && num <= 64) state->length = (uint8_t)num;
                continue;

            case 'T': case 't':
                num = 0;
                while (*mml >= '0' && *mml <= '9') {
                    num = num * 10 + (*mml - '0'); mml++;
                }
                if (num >= 32 && num <= 255) mml_tempo = (uint8_t)num;
                continue;

            case 'V': case 'v':
                num = 0;
                while (*mml >= '0' && *mml <= '9') {
                    num = num * 10 + (*mml - '0'); mml++;
                }
                if (num <= 15) state->volume = (uint8_t)num;
                continue;

            case 'Q': case 'q':
                num = 0;
                while (*mml >= '0' && *mml <= '9') {
                    num = num * 10 + (*mml - '0'); mml++;
                }
                if (num >= 1 && num <= 8) state->gate = (uint8_t)num;
                continue;

            case '<':
                if (state->octave > 1) state->octave--;
                continue;
            case '>':
                if (state->octave < 8) state->octave++;
                continue;

            default:
                continue;
        }

        /* This is a note (C-B) */
        note = (uint8_t)((note + modifier + 12) % 12);

        /* Get note length if specified */
        num = 0;
        while (*mml >= '0' && *mml <= '9') {
            num = num * 10 + (*mml - '0'); mml++;
        }
        if (num == 0) num = state->length;

        /* Check for dotted note */
        if (*mml == '.') { dotted = 1; mml++; }

        duration = calc_duration((uint8_t)num, dotted);

        /* Calculate frequency */
        freq = note_freq[note];
        if (state->octave < 4) {
            freq <<= (4 - state->octave);
        } else if (state->octave > 4) {
            freq >>= (state->octave - 4);
        }

        /* Set up the channel */
        basic_set_tone(ch, freq);
        basic_set_volume(ch, state->volume);

        state->mml = mml;
        state->remaining = duration;
        state->gate_end = (duration * state->gate) / 8;
        return;
    }

    /* End of MML string */
    state->mml = mml;
    state->active = 0;
    basic_set_volume(ch, 0);
}

/* Play MML on 3 channels simultaneously */
void basic_play_3ch(const char* mml_a, const char* mml_b, const char* mml_c) {
    MmlChannel channels[3];
    uint8_t i;
    uint8_t any_active;

    /* Initialize channels */
    for (i = 0; i < 3; i++) {
        channels[i].octave = 4;
        channels[i].length = 4;
        channels[i].volume = 8;
        channels[i].gate = 8;
        channels[i].remaining = 0;
        channels[i].gate_end = 0;
        channels[i].active = 0;
    }

    if (mml_a && *mml_a) { channels[0].mml = mml_a; channels[0].active = 1; }
    if (mml_b && *mml_b) { channels[1].mml = mml_b; channels[1].active = 1; }
    if (mml_c && *mml_c) { channels[2].mml = mml_c; channels[2].active = 1; }

    /* Reset tempo */
    mml_tempo = 120;

    /* Enable all tone channels */
    basic_sound(PSG_MIXER, 0xB8);

    /* Parse first note for each active channel */
    for (i = 0; i < 3; i++) {
        if (channels[i].active) {
            parse_mml_note(i, &channels[i]);
        }
    }

    /* Main playback loop */
    do {
        /* Wait one frame */
        #asm
        halt
        #endasm

        any_active = 0;

        /* Process each channel */
        for (i = 0; i < 3; i++) {
            if (!channels[i].active) continue;

            any_active = 1;

            if (channels[i].remaining > 0) {
                channels[i].remaining--;

                /* Check if we've passed gate time (note off) */
                if (channels[i].gate_end > 0 && channels[i].remaining < channels[i].gate_end) {
                    /* Actually check if we're past gate time */
                    if (channels[i].remaining == channels[i].gate_end - 1) {
                        basic_set_volume(i, 0);
                    }
                }

                if (channels[i].remaining == 0) {
                    /* Note finished, parse next */
                    parse_mml_note(i, &channels[i]);
                }
            }
        }
    } while (any_active);

    /* Turn off all channels */
    basic_sound_off();
}

uint8_t basic_play_check(void) {
    /* For synchronous play, always returns 0 (not playing) */
    return 0;
}

void basic_sound_off(void) {
    basic_sound(PSG_VOL_A, 0);
    basic_sound(PSG_VOL_B, 0);
    basic_sound(PSG_VOL_C, 0);
    basic_sound(PSG_MIXER, 0xBF);
}

void basic_set_tone(uint8_t channel, uint16_t frequency) {
    uint8_t reg = channel * 2;
    basic_sound(reg, frequency & 0xFF);
    basic_sound(reg + 1, (frequency >> 8) & 0x0F);
}

void basic_set_volume(uint8_t channel, uint8_t volume) {
    basic_sound(PSG_VOL_A + channel, volume & 0x1F);
}

void basic_set_noise(uint8_t frequency) {
    basic_sound(PSG_NOISE, frequency & 0x1F);
}

void basic_set_mixer(uint8_t tone_mask, uint8_t noise_mask) {
    basic_sound(PSG_MIXER, 0x80 | (noise_mask << 3) | tone_mask);
}

void basic_set_envelope(uint16_t period, uint8_t shape) {
    basic_sound(PSG_ENV_LOW, period & 0xFF);
    basic_sound(PSG_ENV_HIGH, (period >> 8) & 0xFF);
    basic_sound(PSG_ENV_SHAPE, shape & 0x0F);
}

/* Sound effects */
void basic_sfx_explosion(void) {
    uint16_t i;

    /* Use noise with envelope */
    basic_sound(PSG_NOISE, 15);
    basic_sound(PSG_MIXER, 0x87);  /* Enable noise on all channels */
    basic_sound(PSG_VOL_A, 0x10);  /* Use envelope */
    basic_sound(PSG_ENV_LOW, 0x00);
    basic_sound(PSG_ENV_HIGH, 0x10);
    basic_sound(PSG_ENV_SHAPE, ENV_DECAY);

    for (i = 0; i < 30; i++) {
        #asm
        halt
        #endasm
    }
    basic_sound_off();
}

void basic_sfx_laser(void) {
    uint16_t freq;
    uint8_t i;

    basic_sound(PSG_MIXER, 0xBE);  /* Tone A only */
    basic_sound(PSG_VOL_A, 15);

    /* Descending frequency sweep */
    for (freq = 100; freq < 500; freq += 20) {
        basic_sound(PSG_TONE_A_LOW, freq & 0xFF);
        basic_sound(PSG_TONE_A_HIGH, (freq >> 8) & 0x0F);
        for (i = 0; i < 2; i++) {
            #asm
            halt
            #endasm
        }
    }
    basic_sound_off();
}

void basic_sfx_jump(void) {
    uint16_t freq;
    uint8_t i;

    basic_sound(PSG_MIXER, 0xBE);  /* Tone A only */
    basic_sound(PSG_VOL_A, 12);

    /* Ascending frequency sweep */
    for (freq = 400; freq > 100; freq -= 15) {
        basic_sound(PSG_TONE_A_LOW, freq & 0xFF);
        basic_sound(PSG_TONE_A_HIGH, (freq >> 8) & 0x0F);
        for (i = 0; i < 2; i++) {
            #asm
            halt
            #endasm
        }
    }
    basic_sound_off();
}

void basic_sfx_coin(void) {
    uint8_t i;

    basic_sound(PSG_MIXER, 0xBE);  /* Tone A only */

    /* High pitched double beep */
    basic_set_tone(0, 200);
    basic_sound(PSG_VOL_A, 12);
    for (i = 0; i < 5; i++) {
        #asm
        halt
        #endasm
    }
    basic_set_tone(0, 150);
    for (i = 0; i < 8; i++) {
        #asm
        halt
        #endasm
    }
    basic_sound_off();
}

void basic_sfx_hit(void) {
    uint8_t i;

    /* Short noise burst */
    basic_sound(PSG_NOISE, 8);
    basic_sound(PSG_MIXER, 0x87);  /* Noise on A */
    basic_sound(PSG_VOL_A, 15);

    for (i = 0; i < 6; i++) {
        #asm
        halt
        #endasm
    }
    basic_sound_off();
}

uint8_t basic_music_init(void) { return 0; }
void basic_music_play(const char* mml) { (void)mml; }
uint8_t basic_audio_check(void) { return 0; }
void basic_pcm_play(const uint8_t* data, uint16_t length, uint16_t frequency) {
    (void)data; (void)length; (void)frequency;
}
void basic_pcm_stop(void) {}
