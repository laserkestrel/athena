#include <Arduino.h>
#include "driver/i2s.h"
#include <math.h>

// I2S pins
#define I2S_WS   25   // LRC
#define I2S_BCLK 26   // BCLK
#define I2S_DOUT 27   // DIN
#define SD_PIN   13   // MAX98357A shutdown (active HIGH)

// LED pin
#define LED_PIN 2     // On-board LED

// Audio parameters
#define SAMPLE_RATE 16000   // 16 kHz sample rate
#define AMPLITUDE 1000      // Sine wave amplitude
#define TONE_FREQ 440       // Frequency of sine wave (Hz)
#define BUFFER_SIZE 512     // Samples per buffer

int16_t sampleBuffer[BUFFER_SIZE * 2]; // Stereo interleaved buffer

void setup() {
  Serial.begin(115200);

  // Enable MAX98357A
  pinMode(SD_PIN, OUTPUT);
  digitalWrite(SD_PIN, HIGH);  // enable amplifier

  // Setup LED pin
  pinMode(LED_PIN, OUTPUT);

  // Setup I2S
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_I2S,
      .intr_alloc_flags = 0,
      .dma_buf_count = 4,
      .dma_buf_len = 512,
      .use_apll = false
  };

  i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_BCLK,
      .ws_io_num = I2S_WS,
      .data_out_num = I2S_DOUT,
      .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);

  // Precompute sine wave buffer (stereo interleaved)
  for (int i = 0; i < BUFFER_SIZE; i++) {
    int16_t sample = AMPLITUDE * sin(2 * PI * TONE_FREQ * i / SAMPLE_RATE);
    sampleBuffer[i * 2]     = sample;  // left
    sampleBuffer[i * 2 + 1] = sample;  // right
  }
}

void loop() {
  // Send sine wave buffer via I2S
  size_t bytesWritten;
  i2s_write(I2S_NUM_0, sampleBuffer, sizeof(sampleBuffer), &bytesWritten, portMAX_DELAY);

  // LED breathing effect
  for (int i = 0; i <= 255; i++) {      // Brighten
    analogWrite(LED_PIN, i);
    delay(5);
  }
  for (int i = 255; i >= 0; i--) {      // Dim
    analogWrite(LED_PIN, i);
    delay(5);
  }
}
