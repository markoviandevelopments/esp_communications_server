#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_NeoPixel.h>

// WiFi credentials
const char* ssid     = "Brubaker Wifi";
const char* password = "Pre$ton01";

// Server URL
const char* serverUrl = "http://50.188.120.138:5000";

// LED Strip Configuration
#define LED_PIN 2  // Use GPIO2 for data signal
#define NUM_LEDS 300

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(115200);
    strip.begin();
    strip.show(); // Initialize all LEDs to OFF

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
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;

        Serial.println("Attempting to connect to server...");
        http.begin(client, serverUrl);
        int httpCode = http.GET();

        if (httpCode > 0) {  // Successful response
            String payload = http.getString();
            payload.trim(); // Remove whitespace

            Serial.print("Server response: ");
            Serial.println(payload);

            if (payload.equalsIgnoreCase("FAST")) {
                rainbowEffect(50);  // Run a moving rainbow effect
            } else if (payload.equalsIgnoreCase("SLOW")) {
                bluePulses();  // Slow blue pulses with red/green bursts
            } else if (payload.equalsIgnoreCase("VALENTINE")) { // Will's addition
                valentineEffect(); // Heartbeat
            } else if (payload.equalsIgnoreCase("CANDLE")) { // Will's addition
                candleGlowEffect(); // Heartbeat
            } else if (payload.equalsIgnoreCase("PINKWAVES")) { // Will's addition
                playfulPinkWaves(); // Heartbeat
            } else {
                Serial.println("Unrecognized response; setting random color.");
                setRandomColor();
            }
        } else {  // Server error or no response
            Serial.print("HTTP error: ");
            Serial.println(httpCode);
            setRandomColor();
        }

        http.end(); // Close connection
    } else {
        Serial.println("WiFi not connected.");
    }

    delay(100); // Wait .1 seconds before polling again
}

// 🟠 FAST Mode: Rainbow effect moving down the strip
void rainbowEffect(int wait) {
    for (int j = 0; j < 256; j++) {  // Full rainbow cycle
        for (int i = 0; i < NUM_LEDS; i++) {
            int hue = (i + j) & 255;  // Shift colors along the strip
            strip.setPixelColor(i, strip.ColorHSV(hue * 256, 255, 255));
        }
        strip.show();
        delay(wait);
    }
}

// 🔵 SLOW Mode: Blue pulses with occasional red/green bursts
void bluePulses() {
    for (int i = 0; i < NUM_LEDS; i += 10) {
        setStripColor(strip.Color(0, 0, 255)); // Full strip blue
        strip.show();
        delay(400);

        // Occasionally send a red or green burst
        if (random(0, 10) < 3) {  // 30% chance of burst
            uint32_t burstColor = (random(0, 2) == 0) ? strip.Color(255, 0, 0) : strip.Color(0, 255, 0);
            for (int j = 0; j < 20; j++) { // Burst for a short duration
                strip.setPixelColor((i + j) % NUM_LEDS, burstColor);
            }
            strip.show();
            delay(300);
        }
    }
}

// <3 Valentine effect why the emojis??
void valentineEffect()
{
    uint32_t baseColor = strip.Color(150, 0, 50);
    int beats[] = {50, 200, 150, 255, 0};
    int durations[] = {50, 30, 80, 200};

    for (int i = 0; i < 5; i++)
    {
        float factor = (float)beats[i] / 255.0f;

        uint8_t r = (uint8_t)(150 * factor);
        uint8_t g = 0;
        uint8_t bl = (uint8_t)(50 * factor);

        setStripColor(strip.Color(r, g, bl));
        strip.show();
        delay(durations[i % 4]);
    }
}

void candleGlowEffect()
{
    uint8_t minBrightness = 80;
    uint8_t maxBrightness = 200;
    uint8_t r = 255, g = 50, b = 10;

    while (true)
    {
        int brightness = random(minBrightness, maxBrightness);
        uint32_t color = strip.Color((r * brightness) / 255, (g * brightness) / 255, (b * brightness) / 255);
        setStripColor(color);
        strip.show();
        delay(random(100, 400));
    }
}

void playfulPinkWaves()
{
    uint8_t waveSize = 20;
    uint16_t speed = 50; 

    for (int offset = 0; offset < NUM_LEDS; offset++)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            
            float brightness = (1 + sin((i + offset) * 0.15)) / 2;
            uint8_t r = (uint8_t)(255 * brightness);
            uint8_t g = (uint8_t)(20 * brightness);
            uint8_t b = (uint8_t)(100 + 50 * brightness);

            strip.setPixelColor(i, strip.Color(r, g, b));
        }
        strip.show();
        delay(speed);
    }
}

// Function to set the whole strip to a color
void setStripColor(uint32_t color) {
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, color);
    }
    strip.show();
}

// Function to set a random color
void setRandomColor() {
    uint8_t r = random(0, 256);
    uint8_t g = random(0, 256);
    uint8_t b = random(0, 256);
    setStripColor(strip.Color(r, g, b));
}
