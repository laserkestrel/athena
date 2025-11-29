#include <Arduino.h>
#include <cstddef>
#include <driver/i2s.h>
#include "audio_data.h"   // Your header with ath01...ath11
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <RTClib.h>

// ---------------------------------------------------------------------------
// I²S pin definitions
// ---------------------------------------------------------------------------
#define I2S_BCLK 26
#define I2S_LRC  25
#define I2S_DOUT 27
#define AMP_SD   13

// ---------------------------------------------------------------------------
// AUDIO PLAYBACK SETTINGS
// ---------------------------------------------------------------------------
#define SAMPLE_RATE 16000
#define PHRASE_INTERVAL 60000  // 1 minute

// ---------------------------------------------------------------------------
// DEBUG MODE ON OR OFF
// ---------------------------------------------------------------------------
#define DEBUG 1  // Set to 0 to disable all debug prints

#if DEBUG
  #define DBG(x) Serial.println(x)
  #define DBGF(fmt, ...) Serial.printf(fmt, __VA_ARGS__)
#else
  #define DBG(x)
  #define DBGF(fmt, ...)
#endif

// ---------------------------------------------------------------------------
// LOOKUP TABLE OF ALL PHRASES
// ---------------------------------------------------------------------------
struct Sound {
    const int16_t* data;
    size_t length;
};

Sound sounds[] = {
    { ath01, ath01_len },
   // { ath02, ath02_len },
   // { ath03, ath03_len },
    // { ath04, ath04_len },
    // { ath07, ath07_len },
   //  { ath09, ath09_len },
   //  { ath11, ath11_len }
};

const int NUM_SOUNDS = sizeof(sounds) / sizeof(Sound);
int currentSound = 0;

// ---------------------------------------------------------------------------
// I²S INITIALISATION
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

// ---------------------------------------------------------------------------
// PLAY AUDIO
// ---------------------------------------------------------------------------
void playSound(int index) {
    if (index < 0 || index >= NUM_SOUNDS) return;

    const int16_t* soundData = sounds[index].data;
    size_t totalBytes = sounds[index].length;
    size_t samples = totalBytes / sizeof(int16_t);

    const size_t CHUNK_SAMPLES = 1024;
    int16_t buffer[CHUNK_SAMPLES];

    digitalWrite(AMP_SD, HIGH);
    delay(5);

    size_t written;
    const size_t SILENCE_SAMPLES = 128;
    for (size_t i = 0; i < SILENCE_SAMPLES; i++) buffer[i] = 0;
    i2s_write(I2S_NUM_0, buffer, SILENCE_SAMPLES * sizeof(int16_t), &written, portMAX_DELAY);

    for (size_t i = 0; i < samples; i += CHUNK_SAMPLES) {
        size_t chunk = (i + CHUNK_SAMPLES <= samples) ? CHUNK_SAMPLES : (samples - i);
        for (size_t j = 0; j < chunk; j++)
            buffer[j] = pgm_read_word(&soundData[i + j]);
        i2s_write(I2S_NUM_0, buffer, chunk * sizeof(int16_t), &written, portMAX_DELAY);
    }

    delay(10000);
    digitalWrite(AMP_SD, LOW);
}

// ---------------------------------------------------------------------------
// RTC + WIFI CAPTIVE PORTAL
// ---------------------------------------------------------------------------
WebServer server(80);
DNSServer dnsServer;
RTC_DS3231 rtc;
const byte DNS_PORT = 53;

bool setupDone = false;
bool rtcAvailable = false;

String formatTime(DateTime t) {
    char buffer[32];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
            t.year(), t.month(), t.day(),
            t.hour(), t.minute(), t.second());
    return String(buffer);
}

const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Athena Clock Setup</title>
<style>
body { font-family: Arial; display:flex; justify-content:center; align-items:center; height:100vh; background:#f0f0f0; }
#container { max-width:300px; padding:20px; background:white; border-radius:12px; box-shadow:0 0 10px rgba(0,0,0,0.1); text-align:center; }
</style>
</head>
<body>
<div id="container">
<h2>Setting Clock…</h2>
<p>Please wait</p>
</div>
<script>
window.onload = () => {
  let ts = Math.floor(Date.now()/1000);
  fetch("/set?ts=" + ts)
    .then(() => { document.body.innerHTML = "<h1>✔ Time Saved to RTC</h1><p>You may close this page.</p>"; });
};
</script>
</body>
</html>
)rawliteral";

void startCaptivePortal() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ATHENA-SETUP", "");
    delay(300);
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

    server.onNotFound([]() {
        server.send(200, "text/html", HTML_PAGE);
    });

    server.on("/set", []() {
        if (!server.hasArg("ts")) return;
        time_t ts = server.arg("ts").toInt();
        Serial.println("\n=== Time Received From Phone ===");
        Serial.printf("Unix Time: %ld\n", ts);
        DateTime dt = DateTime(ts);
        Serial.print("Human Time (UTC): ");
        Serial.println(formatTime(dt));
        if (rtcAvailable) {
            rtc.adjust(dt);
            Serial.println("RTC Updated.");
        } else {
            Serial.println("RTC NOT FOUND — cannot set hardware clock.");
        }
        setupDone = true;
        server.send(200, "text/plain", "OK");
        server.stop();
        dnsServer.stop();
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_STA);
        Serial.println("Clock setup complete — Wi-Fi AP disabled.");
    });

    server.begin();
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

    if (rtc.begin()) {
        rtcAvailable = true;
        if (!rtc.lostPower()) {
            Serial.println("RTC already has valid time.");
            setupDone = true;
        } else {
            Serial.println("RTC power lost — needs setup.");
        }
    } else {
        Serial.println("ERROR: RTC not found!");
    }

    if (!setupDone) {
        Serial.println("Entering FIRST-TIME SETUP MODE...");
        startCaptivePortal();
    } else {
        // Athena says first line immediately
        playSound(currentSound);
        currentSound++;
        if (currentSound >= NUM_SOUNDS) currentSound = 0;
        lastSpeakTime = millis();
    }
}

// ---------------------------------------------------------------------------
// MAIN LOOP
// ---------------------------------------------------------------------------
bool spokenThisMinute = false;

void loop() {
    if (!setupDone) {
        dnsServer.processNextRequest();
        server.handleClient();
        return;
    }

    if (!rtcAvailable) {
        DBG("No RTC available!");
        delay(2000);
        return;
    }

    DateTime now = rtc.now();

    // Announce at the start of every minute
    if (now.second() == 0 && !spokenThisMinute) {
        // Print current time for debugging
        DBGF("Playing phrase at: %04d-%02d-%02d %02d:%02d:%02d\n",
        now.year(), now.month(), now.day(),
        now.hour(), now.minute(), now.second());


        playSound(currentSound);
        currentSound++;
        if (currentSound >= NUM_SOUNDS) currentSound = 0;
        spokenThisMinute = true;  // prevent multiple triggers
    }

    if (now.second() != 0) {
        spokenThisMinute = false;
    }

    delay(1000); // check once per second
}

