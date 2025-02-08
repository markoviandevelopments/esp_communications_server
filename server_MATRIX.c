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

void playfulPinkWaves(int wait) {
    for (int j = 0; j < 256; j++) { // Animation loop
        for (int i = 0; i < NUM_LEDS; i++) {
            // Create a wave effect with a sine function for smooth rolling motion
            int wave = sin((i + j) * 0.2) * 127 + 128; // Generates values from 0-255 smoothly
            int hue = map(wave, 0, 255, 48000, 65535); // Focus on pink/magenta hues

            strip.setPixelColor(i, strip.ColorHSV(hue, 255, 255)); // Keep full saturation and brightness
        }
        strip.show();
        delay(wait);
    }
}

void fight_kampfen() {
    uint8_t waveSize = 20;
    uint16_t speed = 50;
    int is_r = 0;
    int is_b = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
        if (fight_kampf[i] == 0) {
            is_r = 1;
            is_b = 0;
        }
        else {
            is_r = 0;
            is_b = 1;
        }
        uint8_t r = (uint8_t)(255 * is_r);
        uint8_t g = (uint8_t)(42);
        uint8_t b = (uint8_t)(255 * is_b);

        strip.setPixelColor(i, strip.Color(r, g, b));

        if (random(100) == 0 && i < NUM_LEDS - 1) {
            fight_kampf[i] = fight_kampf[i + 1];
        }
        if (random(100) == 0 && i > 0) {
            fight_kampf[i] = fight_kampf[i - 1];
        }
    }
    strip.show();
    delay(speed);
}

void beatingHeartWave() {
    uint32_t heartColor = strip.Color(255, 20, 100); // Romantic pink
    const int heartSize = 7;                         // Size of the heart wave
    const int pulseSpeed = 35;

    for (int pos = 0; pos < NUM_LEDS + heartSize; pos++) {
        strip.clear();

        // Create heart pattern with brightness curve
        for (int i = -heartSize / 2; i <= heartSize / 2; i++) {
            float distance = abs(i);
            float intensity = 1.0 - (distance / (heartSize / 2.0));
            intensity = pow(intensity, 2); // Quadratic falloff

            int ledPos = pos + i;
            if (ledPos >= 0 && ledPos < NUM_LEDS) {
                strip.setPixelColor(ledPos, strip.Color( 255 * intensity, 20 * intensity, 100 * intensity));
            }
        }

        // Add fading tail
        for (int t = 1; t < 8; t++) {
            int trailPos = pos - t * 2;
            if (trailPos >= 0 && trailPos < NUM_LEDS) {
                float trailIntensity = 1.0 - (t * 0.15);
                strip.setPixelColor(trailPos, strip.Color(
                                                  255 * trailIntensity,
                                                  20 * trailIntensity,
                                                  100 * trailIntensity));
            }
        }

        strip.show();
        delay(pulseSpeed);
    }
}

void romanticPulse() {
    const int cycleTime = 15; // Lower = faster
    const float redPhase = 0.0;
    const float pinkPhase = PI; // 180° out of phase

    for (int t = 0; t < 1000; t++) { // Run for ~15 seconds
        float timeFactor = millis() * 0.001;

        // Calculate brightness waves
        float redBrightness = (sin(timeFactor * 2 + redPhase) + 1) * 0.5;
        float pinkBrightness = (sin(timeFactor * 2 + pinkPhase) + 1) * 0.5;

        // Create color gradients
        for (int i = 0; i < NUM_LEDS; i++) {
            float posFactor = (float)i / NUM_LEDS;
            uint8_t r = 255 * redBrightness * (0.7 + 0.3 * sin(posFactor * 4 * PI));
            uint8_t g = 20 * pinkBrightness;
            uint8_t b = 100 * pinkBrightness * (0.5 + 0.5 * cos(posFactor * 2 * PI));

            strip.setPixelColor(i, strip.Color(r, g, b));
        }

        strip.show();
        delay(cycleTime);
    }
}

void cupidsArrow() {
    uint32_t arrowColor = strip.Color(255, 50, 150); // Bright pink
    const int arrowSpeed = 25;
    const int tailLength = 15;

    for (int pos = 0; pos < NUM_LEDS + tailLength; pos++) {
        strip.clear();

        // Arrow head
        if (pos < NUM_LEDS) {
            strip.setPixelColor(pos, arrowColor);
        }

        // Sparkling tail
        for (int i = 1; i < tailLength; i++) {
            int trailPos = pos - i;
            if (trailPos >= 0 && trailPos < NUM_LEDS)
            {
                float intensity = 1.0 - (float)i / tailLength;
                uint8_t sparkle = random(0, 2) ? 255 : 150; // Random twinkle
                strip.setPixelColor(trailPos, strip.Color(
                                                  255 * intensity * sparkle / 255,
                                                  50 * intensity * sparkle / 255,
                                                  150 * intensity * sparkle / 255));
            }
        }

        strip.show();
        delay(arrowSpeed);
    }
}


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