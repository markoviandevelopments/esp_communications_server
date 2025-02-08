#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

// Configuration for Device 001 - LED Array
#define NUM_LEDS 300
#define LED_PIN 2  // GPIO2 for data signal

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// WiFi credentials & Server URL
const char *ssid = "Brubaker Wifi";
const char *password = "Pre$ton01";
const char *serverUrl = "http://50.188.120.138:4999/device/001";

int fight_kampf[NUM_LEDS];  // For FIGHTKAMPF effect

void setup() {
    Serial.begin(115200);
    strip.begin();
    strip.show();

    // Connect to WiFi
    Serial.println("\nConnecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    randomSeed(ESP.getCycleCount());
    
    // Initialize fight_kampf array
    for (int i = 0; i < NUM_LEDS; i++) {
        fight_kampf[i] = random(0, 2);
    }
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;

        http.begin(client, serverUrl);
        http.setTimeout(2000);
        
        int httpCode = http.GET();

        if (httpCode > 0) {
            String payload = http.getString();
            payload.trim();
            Serial.print("Server command: ");
            Serial.println(payload);

            // Execute corresponding effect
            if (payload.equalsIgnoreCase("FAST")) {
                rainbowEffect(50);
            } else if (payload.equalsIgnoreCase("SLOW")) {
                bluePulses();
            } else if (payload.equalsIgnoreCase("VALENTINE")) {
                valentineEffect();
            } else if (payload.equalsIgnoreCase("CANDLE")) {
                candleGlowEffect();
            } else if (payload.equalsIgnoreCase("PINKWAVES")) {
                playfulPinkWaves(50);
            } else if (payload.equalsIgnoreCase("FIGHTKAMPF")) {
                fight_kampfen();
            } else if (payload.equalsIgnoreCase("HEARTWAVE")) {
                beatingHeartWave();
            } else if (payload.equalsIgnoreCase("ROMPULSE")) {
                romanticPulse();
            } else if (payload.equalsIgnoreCase("CUPIDSARROW")) {
                cupidsArrow();
            } else {
                setRandomColor();
            }
        } else {
            Serial.print("HTTP error: ");
            Serial.println(httpCode);
        }
        http.end();
    } else {
        Serial.println("WiFi not connected.");
    }
    delay(1000);  // Check server every second
}

// Effect functions (same as original but removed matrix-specific code)
void rainbowEffect(int wait) {
    for (int j = 0; j < 256; j++) {
        for (int i = 0; i < NUM_LEDS; i++) {
            int hue = (i + j) & 255;
            strip.setPixelColor(i, strip.ColorHSV(hue * 256, 255, 255));
        }
        strip.show();
        delay(wait);
    }
}

void bluePulses() {
    int wait = 50;
    for (int j = 0; j < 256; j++) {
        for (int i = 0; i < NUM_LEDS; i++) {
            int hue = (i + j) % 170;
            hue = map(hue, 0, 170, 0, 65535 / 3);
            strip.setPixelColor(i, strip.ColorHSV(hue, 255, 255));
        }
        strip.show();
        delay(wait);
    }
}

void valentineEffect() {
    uint32_t baseColor = strip.Color(150, 0, 50);
    int beats[] = {50, 200, 150, 255, 0};
    int durations[] = {50, 30, 80, 200};

    for (int i = 0; i < 5; i++) {
        float factor = (float)beats[i] / 255.0f;
        uint8_t r = (uint8_t)(150 * factor);
        setStripColor(strip.Color(r, 0, (uint8_t)(50 * factor)));
        strip.show();
        delay(durations[i % 4]);
    }
}

void candleGlowEffect(int cycles = 100) {
    uint8_t minBrightness = 80;
    uint8_t maxBrightness = 200;
    uint8_t r = 255, g = 50, b = 10;

    for(int i=0; i<cycles; i++) {
        int brightness = random(minBrightness, maxBrightness);
        uint32_t color = strip.Color((r * brightness)/255, (g * brightness)/255, (b * brightness)/255);
        setStripColor(color);
        strip.show();
        delay(random(100, 400));
    }
}

// Other effect functions (playfulPinkWaves, fight_kampfen, etc.) remain unchanged
// [Keep all other effect functions from original code except matrix-specific ones]

void setStripColor(uint32_t color) {
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, color);
    }
    strip.show();
}

void setRandomColor() {
    uint8_t r = random(0, 256);
    uint8_t g = random(0, 256);
    uint8_t b = random(0, 256);
    setStripColor(strip.Color(r, g, b));
}