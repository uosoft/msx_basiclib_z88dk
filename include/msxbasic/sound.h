/*
 * Copyright (c) UO Soft
 * SPDX-License-Identifier: MIT
 */

/**
 * @file sound.h
 * @brief MSX BASIC sound functions
 *
 * Provides functions equivalent to MSX BASIC's:
 * BEEP, SOUND, PLAY
 */

#ifndef MSXBASIC_SOUND_H
#define MSXBASIC_SOUND_H

#include <stdint.h>

/* PSG register numbers */
#define PSG_TONE_A_LOW      0
#define PSG_TONE_A_HIGH     1
#define PSG_TONE_B_LOW      2
#define PSG_TONE_B_HIGH     3
#define PSG_TONE_C_LOW      4
#define PSG_TONE_C_HIGH     5
#define PSG_NOISE           6
#define PSG_MIXER           7
#define PSG_VOL_A           8
#define PSG_VOL_B           9
#define PSG_VOL_C           10
#define PSG_ENV_LOW         11
#define PSG_ENV_HIGH        12
#define PSG_ENV_SHAPE       13

/* Envelope shapes */
#define ENV_DECAY           0
#define ENV_ATTACK          4
#define ENV_DECAY_REPEAT    8
#define ENV_SAWTOOTH_DOWN   10
#define ENV_ATTACK_REPEAT   12
#define ENV_SAWTOOTH_UP     14

/**
 * @brief Sound the beep
 * Equivalent to: BEEP
 */
void basic_beep(void);

/**
 * @brief Write to PSG register
 * Equivalent to: SOUND register, value
 * @param reg PSG register number (0-13)
 * @param value Value to write (0-255)
 */
void basic_sound(uint8_t reg, uint8_t value);

/**
 * @brief Read from PSG register
 * @param reg PSG register number (0-13)
 * @return Current value of the register
 */
uint8_t basic_sound_read(uint8_t reg);

/**
 * @brief Play a single note on a channel
 * @param channel Channel (0=A, 1=B, 2=C)
 * @param note Note (0=C, 1=C#, 2=D, ... 11=B)
 * @param octave Octave (1-8)
 * @param duration_ms Duration in frames (1/60 sec units)
 */
void basic_play_note(uint8_t channel, uint8_t note, uint8_t octave, uint16_t duration_ms);

/**
 * @brief Play MML (Music Macro Language) string
 * Equivalent to: PLAY string$
 * Supported commands: CDEFGAB (notes), R (rest), O (octave), L (length),
 *                    T (tempo), V (volume), < > (octave down/up)
 * @param mml MML string (null-terminated)
 */
void basic_play(const char* mml);

/**
 * @brief Play MML on specific channels
 * Equivalent to: PLAY string1$, string2$, string3$
 * @param mml_a MML string for channel A (can be NULL)
 * @param mml_b MML string for channel B (can be NULL)
 * @param mml_c MML string for channel C (can be NULL)
 */
void basic_play_3ch(const char* mml_a, const char* mml_b, const char* mml_c);

/**
 * @brief Check if PLAY is still playing
 * Equivalent to: PLAY(0)
 * @return Non-zero if music is still playing
 */
uint8_t basic_play_check(void);

/**
 * @brief Stop all sound
 */
void basic_sound_off(void);

/**
 * @brief Set PSG tone for a channel
 * @param channel Channel (0=A, 1=B, 2=C)
 * @param frequency Frequency divider value (0-4095)
 */
void basic_set_tone(uint8_t channel, uint16_t frequency);

/**
 * @brief Set PSG volume for a channel
 * @param channel Channel (0=A, 1=B, 2=C)
 * @param volume Volume (0-15, or 16 for envelope)
 */
void basic_set_volume(uint8_t channel, uint8_t volume);

/**
 * @brief Set PSG noise frequency
 * @param frequency Noise frequency (0-31)
 */
void basic_set_noise(uint8_t frequency);

/**
 * @brief Set PSG mixer (enable/disable tone and noise per channel)
 * @param tone_mask Tone enable mask (bit 0=A, 1=B, 2=C, 0=enable)
 * @param noise_mask Noise enable mask (bit 0=A, 1=B, 2=C, 0=enable)
 */
void basic_set_mixer(uint8_t tone_mask, uint8_t noise_mask);

/**
 * @brief Set PSG envelope
 * @param period Envelope period (0-65535)
 * @param shape Envelope shape (0-15)
 */
void basic_set_envelope(uint16_t period, uint8_t shape);

/* === Sound effects === */

/**
 * @brief Play explosion sound effect
 */
void basic_sfx_explosion(void);

/**
 * @brief Play laser/shoot sound effect
 */
void basic_sfx_laser(void);

/**
 * @brief Play jump sound effect
 */
void basic_sfx_jump(void);

/**
 * @brief Play coin/pickup sound effect
 */
void basic_sfx_coin(void);

/**
 * @brief Play hit/damage sound effect
 */
void basic_sfx_hit(void);

/* === MSX-MUSIC (FM) functions (MSX2 with FMPAC/MSX-MUSIC) === */

/**
 * @brief Initialize MSX-MUSIC
 * Equivalent to: CALL MUSIC
 * @return 1 if MSX-MUSIC is available, 0 otherwise
 */
uint8_t basic_music_init(void);

/**
 * @brief Play FM sound using MSX-MUSIC
 * Equivalent to: CALL MUSIC, PLAY#2, string$
 * @param mml MML string for FM
 */
void basic_music_play(const char* mml);

/* === MSX-AUDIO functions (MSX2 with MSX-AUDIO) === */

/**
 * @brief Check if MSX-AUDIO is available
 * @return 1 if MSX-AUDIO is available, 0 otherwise
 */
uint8_t basic_audio_check(void);

/* === PCM functions (turbo R) === */

/**
 * @brief Play PCM data (turbo R only)
 * @param data Pointer to PCM data
 * @param length Length of data
 * @param frequency Playback frequency
 */
void basic_pcm_play(const uint8_t* data, uint16_t length, uint16_t frequency);

/**
 * @brief Stop PCM playback (turbo R only)
 */
void basic_pcm_stop(void);

#endif /* MSXBASIC_SOUND_H */
