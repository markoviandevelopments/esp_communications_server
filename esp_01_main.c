#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <EEPROM.h>

// Configuration for Device 001 - LED Array
#define NUM_LEDS 300
#define LED_PIN 2  // GPIO2 for data signal

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// WiFi credentials & Server URL
const char *ssid = "Brubaker Wifi";
const char *password = "Pre$ton01";
const char *serverUrl = "http://10.1.10.79:4999/device/001";

volatile bool newCommandReceived = false; // this flag monitors state changes

unsigned long lastServerCheck = 0;
const int serverCheckInterval = 100;

int fight_kampf[NUM_LEDS];  // For FIGHTKAMPF effect

int bee_x = 16;
int bee_y = 4;

uint32_t pulseColor = strip.Color(0, 0, 0);
int pulseBrightness = 0;
int pulseDirection = 1;

#define HEIGHT 8
#define WIDTH_P 4
#define WIDTH_PLUS 5
#define WIDTH_W 7

// Define the letter "P"
int letter_P[HEIGHT][WIDTH_P] = {
    {1, 1, 1, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 1, 1, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0}
};

// Define the letter "+"
int letter_PLUS[HEIGHT][WIDTH_PLUS] = {
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {1, 1, 1, 1, 1},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}
};

// Define the letter "W"
int letter_W[HEIGHT][WIDTH_W] = {
    {1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1},
    {1, 1, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 1}
};

int text_sroll_x_dir = 1;
int text_sroll_x = 0;


void setup() {
    Serial.begin(115200);
    strip.begin();
    strip.show();
    EEPROM.begin(16);

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

    // Restore saved color from EEPROM
    uint8_t r = EEPROM.read(0);
    uint8_t g = EEPROM.read(1);
    uint8_t b = EEPROM.read(2);
    pulseColor = strip.Color(r, g, b);
    Serial.printf("🔄 Restored Color: #%02X%02X%02X\n", r, g, b);

    // Initialize fight_kampf array
    for (int i = 0; i < NUM_LEDS; i++) {
        fight_kampf[i] = random(0, 2);
    }
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        if (millis() - lastServerCheck >= serverCheckInterval) {
            lastServerCheck = millis();

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

                newCommandReceived = false; // Reset flag before processing new command

                if (payload.startsWith("COLORPULSE")) {
                    strip.clear();
                    strip.show();
                    int colonIndex = payload.indexOf(':');
                    if (colonIndex != -1) {
                        String colorStr = payload.substring(colonIndex + 1);
                        pulseColor = parseColor(colorStr);
                    }
                    runColorPulse();
                }
                newCommandReceived = false; // ✅ Only reset after executing

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
                else if (payload.equalsIgnoreCase("TEXTMATRIX")) {
                    uint16_t speed = 1000;
                    matrix_text(speed);
                }
                else if (payload.equalsIgnoreCase("TEXTMATRIXFAST")) {
                    uint16_t speed = 100;
                    matrix_text(speed);
                }
                else if (payload.equalsIgnoreCase("TEXTMATRIXBLUE")) {
                    uint16_t speed = 1000;
                    matrix_text(speed, 1);
                }
                else if (payload.equalsIgnoreCase("RESET")) {
                    Serial.println("🔄 Resetting LED system!");
                    strip.clear();
                    strip.show();
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
        else
        {
            Serial.println("WiFi not connected.");
        }
    }
}

void rainbowEffect(int wait) {
    unsigned long startTime = millis();

    for (int j = 0; j < 256; j++) {
        for (int i = 0; i < NUM_LEDS; i++) {
            int hue = (i + j) & 255;
            strip.setPixelColor(i, strip.ColorHSV(hue * 256, 255, 255));
        }
        strip.show();

        if (newCommandReceived)
            return;

        while (millis() - startTime < wait) {
            if (newCommandReceived)
                return;
            delay(1);
        }
        startTime = millis();
    }
}

void bluePulses() {
    unsigned long startTime = millis();

    for (int j = 0; j < 256; j++) {
        for (int i = 0; i < NUM_LEDS; i++) {
            int hue = (i + j) % 170;
            int adjustedHue = hue * (65535 / 170); // Precompute mapping
            strip.setPixelColor(i, strip.ColorHSV(adjustedHue, 255, 255));
        }
        strip.show();

        if (newCommandReceived)
            return;

        while (millis() - startTime < 50) { // Ensures precise timing without blocking
            if (newCommandReceived)
                return;
            delay(1);
        }
        startTime = millis();
    }
}

void valentineEffect() {
    uint32_t baseColor = strip.Color(150, 0, 50);
    int beats[] = {50, 200, 150, 255, 0};
    int durations[] = {50, 30, 80, 200};
    unsigned long startTime = millis();

    for (int i = 0; i < 5; i++) {
        float factor = (float)beats[i] / 255.0f;
        uint8_t r = (uint8_t)(150 * factor);
        setStripColor(strip.Color(r, 0, (uint8_t)(50 * factor)));
        strip.show();

        if (newCommandReceived)
            return;

        while (millis() - startTime < durations[i % 4]) {
            if (newCommandReceived)
                return;
            delay(1);
        }
        startTime = millis();
    }
}

void playfulPinkWaves() {
    unsigned long startTime = millis();

    for (int j = 0; j < 256; j++) {
        float sinValues[NUM_LEDS];

        // Precompute sine values to avoid excessive calculations
        for (int i = 0; i < NUM_LEDS; i++) {
            sinValues[i] = sin((i + j) * 0.2) * 127 + 128;
        }

        for (int i = 0; i < NUM_LEDS; i++) {
            int hue = map(sinValues[i], 0, 255, 48000, 65535);
            strip.setPixelColor(i, strip.ColorHSV(hue, 255, 255));
        }
        strip.show();

        if (newCommandReceived)
            return;

        while (millis() - startTime < 100)
        {
            if (newCommandReceived)
                return;
            delay(1);
        }
        startTime = millis();
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

int get_index(int x_in, int y_in) {
    int index_out;
    if (x_in % 2 == 0) {
        index_out = y_in + x_in * 8;
    }
    else {
        index_out = (7 - y_in) + x_in * 8;
    }

    return index_out;
}

// A bee will move via Brownian Motion across an 8x32 array
void matrix_bee() {
    uint8_t waveSize = 20;
    uint16_t speed = 1000;
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
        int bee_index;
        if (bee_x % 2 == 0) {
            bee_index = bee_y + bee_x * 8;
        }
        else {
            bee_index = (7 - bee_y) + bee_x * 8;
        }
        
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

void matrix_text(uint16_t speed) {
    matrix_text(speed, 0);  // Call the full version with default blue=0
}

void matrix_text(uint16_t speed, int is_blue_background) {
    uint8_t waveSize = 20;
    int is_r = 0;
    int is_b = 0;
    int index;
    int text_in;

    int is_on[300] = {0};

    for (int x=0; x<32;x++) {
        for (int y=0;y<8;y++) {
            index = get_index(x, y);
            if ((x - text_sroll_x) < WIDTH_P && (x - text_sroll_x) >= 0 ) {
                is_on[index] = letter_P[y][(x - text_sroll_x)];
            }
            else if ((x - text_sroll_x) < WIDTH_P + WIDTH_PLUS + 1 && (x - text_sroll_x) >= WIDTH_P + 1) {
                is_on[index] = letter_PLUS[y][(x - text_sroll_x) - WIDTH_P - 1];
            }
            else if ((x - text_sroll_x) < WIDTH_P + WIDTH_PLUS + WIDTH_W + 2 && (x - text_sroll_x) >= WIDTH_P + WIDTH_PLUS + 2) {
                is_on[index] = letter_W[y][(x - text_sroll_x) - WIDTH_P - WIDTH_PLUS - 2];
            }
            else {
                is_on[index] = 0;
            }
        }
    }

    for (int i = 0; i < NUM_LEDS; i++) {
        
        if (is_on[i] == 1) {
            is_r = 1;
            is_b = 0;
        }
        else {
            is_r = 0;
            is_b = is_blue_background;
        }
        uint8_t r = (uint8_t)(255 * is_r);
        uint8_t g = (uint8_t)(255 * is_r);
        uint8_t b = (uint8_t)(255 * is_b);

        strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();

    text_sroll_x += text_sroll_x_dir;
    if (text_sroll_x > 31 - 2 - WIDTH_P - WIDTH_PLUS - WIDTH_W) {
        text_sroll_x_dir = -1;
    }
    else if (text_sroll_x <= 0) {
        text_sroll_x_dir = 1;
    }
    delay(speed);
}

void beatingHeartWave()
{
    const int pulseSpeed = 60;
    const float maxBrightness = 0.8;

    for (int beat = 0; beat < 3; beat++)
    {
        float heartSize = 8.0;
        float pulse = (beat % 2 == 0) ? 0.8 : 1.2;

        unsigned long startTime = millis();
        while (millis() - startTime < pulseSpeed * 50)
        { // Non-blocking loop
            strip.clear();
            float timeFactor = (millis() % pulseSpeed) / (float)pulseSpeed;
            float throb = heartSize * (1.0 + sin(timeFactor * PI)) * pulse;

            for (int pos = 0; pos < NUM_LEDS; pos++)
            {
                float distance = abs(pos - NUM_LEDS / 2);
                float intensity = maxBrightness * constrain(1.0 - distance / throb, 0, 1);
                intensity = pow(intensity, 1.5);

                // Crimson core with golden sparkles
                if (random(100) > 95)
                {
                    strip.setPixelColor(pos, strip.Color(255, 210, 50));
                }
                else
                {
                    strip.setPixelColor(pos, strip.Color(180 * intensity, 10 * intensity, 20 * intensity));
                }
            }
            strip.show();

            // ✅ Allow commands to interrupt!
            if (newCommandReceived)
                return;
            delay(10);
        }
    }
}

void romanticPulse()
{
    const int cycleTime = 13;
    const float pulseFrequency = 2.5;
    const int glitterChance = 50;
    unsigned long startTime = millis();

    while (millis() - startTime < 3000)
    { // Limit to 3 seconds max
        float timeFactor = millis() * 0.001;
        float baseWave = sin(timeFactor * pulseFrequency);
        float harmonic = sin(timeFactor * pulseFrequency * 3.7) * 0.3;
        float pulseWave = (baseWave + harmonic + 1.2) * 0.6;

        for (int i = 0; i < NUM_LEDS; i++)
        {
            float posWave = sin((i * 0.3) + timeFactor * 3);
            float ripple = cos((i * 0.15) - timeFactor * 4);

            uint8_t r = constrain(220 * pulseWave * (0.8 + posWave * 0.3), 50, 255);
            uint8_t g = constrain(30 * pulseWave * (0.5 + ripple * 0.2), 0, 40);
            uint8_t b = constrain(25 * (1.2 - pulseWave), 0, 30);

            if (random(1000) < glitterChance)
            {
                r = constrain(r + 150, 0, 255);
                g = constrain(g + 40, 0, 80);
                b = 0;
            }

            if (i % 3 == (millis() / cycleTime) % 3)
            {
                r *= 0.8;
                g *= 0.5;
                b *= 0.5;
            }

            strip.setPixelColor(i, strip.Color(r, g, b));
        }
        strip.show();

        if (newCommandReceived)
            return;
        delay(cycleTime);
    }
}

void cupidsArrow()
{
    const int numArrows = 2;
    const int tailLength = 10;

    for (int arrow = 0; arrow < numArrows; arrow++)
    {
        bool reverse = arrow % 2;

        for (int pos = 0; pos < NUM_LEDS + tailLength; pos++)
        {
            int actualPos = reverse ? (NUM_LEDS - pos) : pos;

            // Instead of clearing, fade the trail
            for (int i = 0; i < NUM_LEDS; i++)
            {
                uint32_t currentColor = strip.getPixelColor(i);
                uint8_t r = (currentColor >> 16) & 0xFF;
                uint8_t g = (currentColor >> 8) & 0xFF;
                uint8_t b = currentColor & 0xFF;

                r *= 0.8;
                g *= 0.8;
                b *= 0.8;

                strip.setPixelColor(i, strip.Color(r, g, b));
            }

            if (actualPos >= 0 && actualPos < NUM_LEDS)
            {
                strip.setPixelColor(actualPos, strip.Color(255, 50, 70));
            }

            strip.show();

            if (newCommandReceived)
                return;
            delay(15);
        }
    }
}

uint32_t parseColor(String hexStr)
{
    hexStr.trim();
    if (hexStr.startsWith("#"))
    {
        hexStr = hexStr.substring(1);
    }

    if (hexStr.length() != 6 || !isHexadecimal(hexStr))
    {
        Serial.println("❌ ERROR: Invalid color format! Defaulting to white.");
        return strip.Color(255, 255, 255);
    }

    long number = strtol(hexStr.c_str(), NULL, 16);
    uint8_t r = (number >> 16) & 0xFF;
    uint8_t g = (number >> 8) & 0xFF;
    uint8_t b = number & 0xFF;

    Serial.printf("✅ Parsed Color - R: %d G: %d B: %d\n", r, g, b);

    // ✅ Save color to EEPROM so it persists after a restart
    EEPROM.write(0, r);
    EEPROM.write(1, g);
    EEPROM.write(2, b);
    EEPROM.commit();

    return strip.Color(r, g, b);
}

// Helper function to check if string is a valid hex code
bool isHexadecimal(String str) {
    for (unsigned int i = 0; i < str.length(); i++) {
        char c = str.charAt(i);
        if (!isxdigit(c)) {
            return false;
        }
    }
    return true;
}

void runColorPulse()
{
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 80)
    { // Adjust timing dynamically
        lastUpdate = millis();
        pulseBrightness += pulseDirection * 1;
        if (pulseBrightness >= 255 || pulseBrightness <= 20)
        {
            pulseDirection *= -1;
        }
        uint32_t dimmedColor = dimColor(pulseColor, pulseBrightness);
        setStripColor(dimmedColor);
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

    if (httpCode > 0)
    {
        String newPayload = http.getString();
        newPayload.trim();
        http.end();

        if (!newPayload.startsWith("COLORPULSE"))
        {
            Serial.println("🔄 New command detected!");
            newCommandReceived = true; // ✅ Set flag to break loop in `runColorPulse`
            return true;
        }
    }
    http.end();
    return false;
}

void setStripColor(uint32_t color) {
    static uint32_t lastColor = 0;
    if (lastColor != color) { // ✅ Only update if necessary
        for (int i = 0; i < NUM_LEDS; i++) {
            strip.setPixelColor(i, color);
        }
        strip.show();
        lastColor = color;
    }
}

void setRandomColor() {
    uint8_t r = random(0, 256);
    uint8_t g = random(0, 256);
    uint8_t b = random(0, 256);
    setStripColor(strip.Color(r, g, b));
}