#pragma once
#include <stdint.h>
#include <cstddef>
#include <pgmspace.h>   // Needed for PROGMEM

// Declare all 11 audio samples in flash
extern const int16_t ath01[] PROGMEM;
extern const size_t ath01_len;

extern const int16_t ath02[] PROGMEM;
extern const size_t ath02_len;

extern const int16_t ath03[] PROGMEM;
extern const size_t ath03_len;

extern const int16_t ath04[] PROGMEM;
extern const size_t ath04_len;

extern const int16_t ath07[] PROGMEM;
extern const size_t ath07_len;

extern const int16_t ath09[] PROGMEM;
extern const size_t ath09_len;

extern const int16_t ath11[] PROGMEM;
extern const size_t ath11_len;
