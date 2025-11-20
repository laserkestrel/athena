#include <Arduino.h>
#include <cstddef>
#include <driver/i2s.h>
#include "audio_data.h"   // Your header with ath01...ath11

// ---------------------------------------------------------------------------
// I²S pin definitions (matches your wiring)
// ---------------------------------------------------------------------------
#define I2S_BCLK 26
#define I2S_LRC  25
#define I2S_DOUT 27
#define AMP_SD   13

// ---------------------------------------------------------------------------
// AUDIO PLAYBACK SETTINGS
// ---------------------------------------------------------------------------
#define SAMPLE_RATE 16000      // Set to 8000 if your WAVs are 8 kHz
#define PHRASE_INTERVAL 60000  // 60,000 ms = 1 minute

// ---------------------------------------------------------------------------
// Lookup table of all phrases
// (Keep this in the same order they should be played)
// ---------------------------------------------------------------------------
struct Sound {
    const int16_t* data;
    size_t length;
};

Sound sounds[] = {
    { ath01, ath01_len },
    { ath02, ath02_len },
    { ath03, ath03_len },
    { ath04, ath04_len },
    { ath07, ath07_len },
    { ath09, ath09_len },
    { ath11, ath11_len }
};

const int NUM_SOUNDS = sizeof(sounds) / sizeof(Sound);
int currentSound = 0;

// ---------------------------------------------------------------------------
// INITIALISE I²S
// ---------------------------------------------------------------------------
void setupI2S() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_LRC,
        .data_out_num = I2S_DOUT,
        .data_in_num = -1
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}

void playSound(int index) {
    if (index < 0 || index >= NUM_SOUNDS) return;

    const int16_t* soundData = sounds[index].data;
    size_t totalBytes = sounds[index].length;
    size_t samples = totalBytes / sizeof(int16_t);

    const size_t CHUNK_SAMPLES = 1024;
    int16_t buffer[CHUNK_SAMPLES];

    // Wake amplifier
    digitalWrite(AMP_SD, HIGH);
    delay(5); // let amp settle

    size_t written;

    // Optional short pre-roll silence
    const size_t SILENCE_SAMPLES = 128;
    for (size_t i = 0; i < SILENCE_SAMPLES; i++) buffer[i] = 0;
    i2s_write(I2S_NUM_0, buffer, SILENCE_SAMPLES * sizeof(int16_t), &written, portMAX_DELAY);

    // Play audio in chunks
    for (size_t i = 0; i < samples; i += CHUNK_SAMPLES) {
        size_t chunk = (i + CHUNK_SAMPLES <= samples) ? CHUNK_SAMPLES : (samples - i);
        for (size_t j = 0; j < chunk; j++)
            buffer[j] = pgm_read_word(&soundData[i + j]);

        i2s_write(I2S_NUM_0, buffer, chunk * sizeof(int16_t), &written, portMAX_DELAY);
    }

    // Keep amp on to ensure full tail
    delay(10000);

    // Sleep amplifier
    digitalWrite(AMP_SD, LOW);
}


// ---------------------------------------------------------------------------
// SETUP
// ---------------------------------------------------------------------------
unsigned long lastSpeakTime = 0;

void setup() {
    Serial.begin(115200);

    pinMode(AMP_SD, OUTPUT);
    digitalWrite(AMP_SD, HIGH);

    setupI2S();

    // Athena says her first line right away
    playSound(currentSound);
    currentSound++;
    if (currentSound >= NUM_SOUNDS) currentSound = 0;

    lastSpeakTime = millis();  // next line in 1 minute
}

// ---------------------------------------------------------------------------
// MAIN LOOP — Athena speaks every 1 minute
// ---------------------------------------------------------------------------
void loop() {

    unsigned long now = millis();
    
    if (now - lastSpeakTime >= PHRASE_INTERVAL) {
        lastSpeakTime = now;

        Serial.print("Playing phrase: ");
        Serial.println(currentSound + 1);

        playSound(currentSound);

        currentSound++;
        if (currentSound >= NUM_SOUNDS) {
            currentSound = 0; // wrap around
        }
    }
}