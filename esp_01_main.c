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
const char *serverUrl = "http://10.1.10.79:4999/device/001";

int fight_kampf[NUM_LEDS];  // For FIGHTKAMPF effect

int bee_x = 16;
int bee_y = 4;

uint32_t pulseColor = strip.Color(0, 0, 0);
int pulseBrightness = 0;
int pulseDirection = 1;

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

            if (payload.startsWith("COLORPULSE")) {
                int colonIndex = payload.indexOf(':');
                if (colonIndex != -1) {
                    String colorStr = payload.substring(colonIndex + 1);
                    pulseColor = parseColor(colorStr);
                }
                runColorPulse(); // Call function
            }
            else if (payload.equalsIgnoreCase("FAST")) {
                rainbowEffect(50);
            }
            else if (payload.equalsIgnoreCase("SLOW")) {
                bluePulses();
            }
            else if (payload.equalsIgnoreCase("VALENTINE")) {
                valentineEffect();
            }
            else if (payload.equalsIgnoreCase("PINKWAVES")) {
                playfulPinkWaves();
            }
            else if (payload.equalsIgnoreCase("FIGHTKAMPF")) {
                fight_kampfen();
            }
            else if (payload.equalsIgnoreCase("HEARTWAVE")) {
                beatingHeartWave();
            }
            else if (payload.equalsIgnoreCase("ROMPULSE")) {
                romanticPulse();
            }
            else if (payload.equalsIgnoreCase("CUPIDSARROW")) {
                cupidsArrow();
            }
            else if (payload.equalsIgnoreCase("BEEMATRIX")) {
                matrix_bee();
            }
            else {
                setRandomColor();
            }
        }
        else {
            Serial.print("HTTP error: ");
            Serial.println(httpCode);
        }
        http.end();
    }
    else {
        Serial.println("WiFi not connected.");
    }
    delay(1000); // Check server every second
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

void playfulPinkWaves() {
    for (int j = 0; j < 256; j++) { // Animation loop
        for (int i = 0; i < NUM_LEDS; i++) {
            // Create a wave effect with a sine function for smooth rolling motion
            int wave = sin((i + j) * 0.2) * 127 + 128; // Generates values from 0-255 smoothly
            int hue = map(wave, 0, 255, 48000, 65535); // Focus on pink/magenta hues

            strip.setPixelColor(i, strip.ColorHSV(hue, 255, 255)); // Keep full saturation and brightness
        }
        strip.show();
        delay(100);
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

// A bee will move via Brownian Motion across an 8x32 array
void matrix_bee() {
    uint8_t waveSize = 20;
    uint16_t speed = 50;
    int is_r = 0;
    int is_b = 0;
    int rr = random(4);

    // Bounds are 0-31 for x, and 0-7 for y
    if (rr == 0 && bee_x < 31) {
        bee_x++;
    } 
    else if (rr == 1 && bee_x > 0) {
        bee_x--;
    }
    else if (rr == 2 && bee_y < 7) {
        bee_y++;
    }
    else if (rr == 3 && bee_y > 0) {
        bee_y--;
    }

    for (int i = 0; i < NUM_LEDS; i++) {
        int bee_index = bee_y + bee_x * 8;
        if (i == bee_index) {
            is_r = 1;
            is_b = 0;
        }
        else {
            is_r = 0;
            is_b = 1;
        }
        uint8_t r = (uint8_t)(255 * is_r);
        uint8_t g = (uint8_t)(255 * is_r);
        uint8_t b = (uint8_t)(255 * is_b);

        strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();
    delay(speed);
}

void beatingHeartWave()
{
    const int pulseSpeed = 60;
    const float maxBrightness = 0.8; // Reduced max for better pulse effect

    for (int beat = 0; beat < 3; beat++)
    { // Triple heartbeat pattern
        float heartSize = 8.0;
        float pulse = (beat % 2 == 0) ? 0.8 : 1.2; // Alternating strong/weak beats

        for (int i = 0; i < 50; i++)
        { // Per-beat animation cycle
            strip.clear();
            float throb = heartSize * (1.0 + sin(i / 8.0)) * pulse;

            // Dynamic core heartbeat
            for (int pos = 0; pos < NUM_LEDS; pos++)
            {
                float distance = abs(pos - NUM_LEDS / 2);
                float intensity = maxBrightness * constrain(1.0 - distance / throb, 0, 1);
                intensity = pow(intensity, 1.5); // Sharper falloff

                // Crimson core with golden sparkles
                if (random(100) > 95)
                {                                                        // 5% sparkle chance
                    strip.setPixelColor(pos, strip.Color(255, 210, 50)); // Gold sparkle
                }
                else
                {
                    strip.setPixelColor(pos, strip.Color(180 * intensity, 10 * intensity, 20 * intensity));
                }
            }

            strip.show();
            delay(pulseSpeed / 2);
        }
    }
}

void romanticPulse()
{
    const int cycleTime = 13;         // Slightly faster tempo
    const float pulseFrequency = 2.5; // More dynamic movement
    const int glitterChance = 50;     // 2% chance per LED for glitter

    for (int t = 0; t < 1000; t++)
    {
        float timeFactor = millis() * 0.001;

        // Multi-layer waveform with harmonics
        float baseWave = sin(timeFactor * pulseFrequency);
        float harmonic = sin(timeFactor * pulseFrequency * 3.7) * 0.3;
        float pulseWave = (baseWave + harmonic + 1.2) * 0.6;

        // Create crimson core with golden accents
        for (int i = 0; i < NUM_LEDS; i++)
        {
            float posWave = sin((i * 0.3) + timeFactor * 3);
            float ripple = cos((i * 0.15) - timeFactor * 4);

            // Dynamic red gradients
            uint8_t r = constrain(220 * pulseWave * (0.8 + posWave * 0.3), 50, 255);
            uint8_t g = constrain(30 * pulseWave * (0.5 + ripple * 0.2), 0, 40);
            uint8_t b = constrain(25 * (1.2 - pulseWave), 0, 30);

            // Add random ruby sparkles
            if (random(1000) < glitterChance)
            {
                r = constrain(r + 150, 0, 255);
                g = constrain(g + 40, 0, 80);
                b = 0; // Kill blue for pure fire effect
            }

            // Velvet fade effect on every 3rd LED
            if (i % 3 == (t % 3))
            {
                r *= 0.8;
                g *= 0.5;
                b *= 0.5;
            }

            strip.setPixelColor(i, strip.Color(r, g, b));
        }

        // Add traveling "love surge" wave
        static float surgePos = 0;
        surgePos = fmod(surgePos + 0.7, NUM_LEDS);
        for (int s = -2; s <= 2; s++)
        {
            int pos = (int)surgePos + s;
            if (pos >= 0 && pos < NUM_LEDS)
            {
                float intensity = 1.0 - abs(s) / 3.0;
                uint8_t r = strip.getPixelColor(pos) >> 16;
                r = constrain(r + 80 * intensity, 0, 255);
                strip.setPixelColor(pos, strip.Color(r, 0, 0));
            }
        }

        strip.show();
        delay(cycleTime);
    }
}

void cupidsArrow()
{
    const int numArrows = 2;
    const int tailLength = 10;
    uint32_t coreColor = strip.Color(220, 40, 150); // Electric crimson

    for (int arrow = 0; arrow < numArrows; arrow++)
    {
        bool reverse = arrow % 2; // Alternate directions

        for (int pos = 0; pos < NUM_LEDS + tailLength; pos++)
        {
            strip.clear();
            int actualPos = reverse ? (NUM_LEDS - pos) : pos;

            // Arrowhead with plasma effect
            if (actualPos >= 0 && actualPos < NUM_LEDS)
            {
                uint8_t flicker = 200 + random(55);
                strip.setPixelColor(actualPos, strip.Color(flicker, 40 * flicker / 255, 50 * flicker / 255));
            }

            // Ionized trail
            for (int t = 1; t < tailLength; t++)
            {
                int trailPos = actualPos - (reverse ? -t : t);
                if (trailPos >= 0 && trailPos < NUM_LEDS)
                {
                    float intensity = 1.0 - (float)t / (tailLength * 0.8);
                    uint8_t red = 255 * intensity;
                    uint8_t spark = random(0, 100) < (30 * intensity) ? 255 : 0; // Random sparks

                    strip.setPixelColor(trailPos, strip.Color(
                                                      red,
                                                      20 * intensity + spark,
                                                      30 * intensity + spark));
                }
            }

            // Leading shockwave
            if (random(10) > 6)
            { // 40% chance for shockwave
                int wavePos = actualPos + (reverse ? -2 : 2);
                if (wavePos >= 0 && wavePos < NUM_LEDS)
                {
                    strip.setPixelColor(wavePos, strip.Color(100, 10, 30));
                }
            }

            strip.show();
            delay(arrow % 2 ? 15 : 20); // Vary speed slightly
        }

        // Impact effect
        if (reverse)
        {
            for (int flash = 0; flash < 3; flash++)
            {
                strip.fill(strip.Color(255, 50, 70), 0, NUM_LEDS);
                strip.show();
                delay(30);
                strip.clear();
                strip.show();
                delay(30);
            }
        }
    }
}

uint32_t parseColor(String hexStr)
{
    hexStr.trim();
    if (hexStr.startsWith("#"))
    {
        hexStr = hexStr.substring(1); // Remove leading '#'
    }

    if (hexStr.length() != 6)
    {
        Serial.println("Invalid color format!");
        return strip.Color(255, 255, 255); // Default to white if parsing fails
    }

    long number = strtol(hexStr.c_str(), NULL, 16);
    uint8_t r = (number >> 16) & 0xFF;
    uint8_t g = (number >> 8) & 0xFF;
    uint8_t b = number & 0xFF;

    Serial.print("Parsed Color - R: ");
    Serial.print(r);
    Serial.print(" G: ");
    Serial.print(g);
    Serial.print(" B: ");
    Serial.println(b);

    return strip.Color(r, g, b);
}

void runColorPulse() {
    int delayTime = 50;
    int maxBrightness = 255;
    int minBrightness = 10;
    unsigned long lastCheckTime = millis();

    while (true)
    {
        pulseBrightness += pulseDirection * 5;
        if (pulseBrightness >= maxBrightness || pulseBrightness <= minBrightness)
        {
            pulseDirection *= -1;
        }

        uint32_t dimmedColor = dimColor(pulseColor, pulseBrightness);
        setStripColor(dimmedColor);
        delay(delayTime);

        // Check for new commands every 5 seconds
        if (millis() - lastCheckTime >= 5000)
        {
            lastCheckTime = millis();
            if (checkForNewCommand())
            {
                Serial.println("New command detected, exiting pulse effect.");
                return;
            }
        }
    }
}

uint32_t dimColor(uint32_t color, uint8_t brightness) {
    float scale = brightness / 255.0; // Ensure floating-point scaling

    uint8_t r = ((color >> 16) & 0xFF) * scale;
    uint8_t g = ((color >> 8) & 0xFF) * scale;
    uint8_t b = (color & 0xFF) * scale;

    Serial.print("Dimmed Color - R: ");
    Serial.print(r);
    Serial.print(" G: ");
    Serial.print(g);
    Serial.print(" B: ");
    Serial.println(b);

    return strip.Color(r, g, b);
}

bool checkForNewCommand() {
    HTTPClient http;
    WiFiClient client;
    http.begin(client, serverUrl);
    int httpCode = http.GET();

    if (httpCode > 0) {
        String newPayload = http.getString();
        newPayload.trim();
        http.end();

        if (!newPayload.startsWith("COLORPULSE"))
        {
            Serial.println("New command received!");
            return true;
        }
    }
    http.end();
    return false;
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