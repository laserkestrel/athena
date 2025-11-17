#include <driver/i2s.h>

// I2S pins
#define I2S_WS   25  // LRC
#define I2S_BCLK 26  // BCLK
#define I2S_DOUT 22  // DIN

// LED pin
#define LED_PIN 2    // On-board LED (adjust if external)

#define SAMPLE_RATE 16000
#define AMPLITUDE 1000
#define TONE_FREQ 440  // Test tone (Hz)
#define BUFFER_SIZE 512

int16_t sampleBuffer[BUFFER_SIZE];

void setup() {
  Serial.begin(115200);

  // Setup LED pin
  pinMode(LED_PIN, OUTPUT);

  // Setup I2S
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
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
}

void loop() {
  // Generate a short sine wave buffer
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sampleBuffer[i] = AMPLITUDE * sin(2 * PI * TONE_FREQ * i / SAMPLE_RATE);
  }

  // Send buffer to MAX98357A via I2S
  size_t bytesWritten;
  i2s_write(I2S_NUM_0, sampleBuffer, BUFFER_SIZE * sizeof(int16_t), &bytesWritten, portMAX_DELAY);

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
