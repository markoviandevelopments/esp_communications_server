#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#define NUM_LEDS 300
#define LED_PIN 2

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

const char *ssid = "Brubaker Wifi";
const char *password = "Pre$ton01";
const char *serverUrl = "http://10.1.10.79:4999/device/001";

// Hitchhiker’s Guide-inspired effects
enum EffectState
{
    IDLE,
    DONT_PANIC,    // Friendly pulsing text-like effect
    RAINBOW,       // Galactic rainbow (space vibes)
    BABEL_FISH,    // Translation swarm
    PAN_GALACTIC,  // Explosive drink effect
    IMPROBABILITY, // Chaos of the Infinite Improbability Drive
    VOGON_POETRY,  // Grim, jarring pulses
    GOLD_TRAIL,    // Heart of Gold comet trail
    TOWEL_WAVE,    // Soothing towel ripple
    ZAPHOD_DUAL,   // Two-headed flashy effect
    GUIDE_MATRIX   // Scrolling "GUIDE" text
};
EffectState currentEffect = IDLE;

volatile bool newCommandReceived = false;
unsigned long lastServerCheck = 0;
const int serverCheckInterval = 300;

// Matrix text for "GUIDE"
#define HEIGHT 8
#define WIDTH_G 5
#define WIDTH_U 5
#define WIDTH_I 3
#define WIDTH_D 5
#define WIDTH_E 5
int letter_G[HEIGHT][WIDTH_G] = {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}, {0, 0, 0, 0, 0}};
int letter_U[HEIGHT][WIDTH_U] = {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}, {0, 0, 0, 0, 0}};
int letter_I[HEIGHT][WIDTH_I] = {{0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 0, 0}};
int letter_D[HEIGHT][WIDTH_D] = {{1, 1, 1, 0, 0}, {1, 0, 0, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 1, 0}, {1, 1, 1, 0, 0}, {0, 0, 0, 0, 0}};
int letter_E[HEIGHT][WIDTH_E] = {{1, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}, {0, 0, 0, 0, 0}};
int text_scroll_x = 0, text_scroll_dir = 1;

uint32_t pulseColor = strip.Color(0, 100, 150); // Default "Don't Panic" green-blue
int pulseBrightness = 0, pulseDirection = 1;

void setup()
{
    Serial.begin(115200);
    strip.begin();
    strip.show();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to the Guide Network! IP: " + WiFi.localIP().toString());

    randomSeed(ESP.getCycleCount());
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED && millis() - lastServerCheck >= serverCheckInterval)
    {
        checkServer();
        lastServerCheck = millis();
    }

    switch (currentEffect)
    {
    case DONT_PANIC:
        dontPanicPulse();
        break;
    case RAINBOW:
        rainbowEffect(50);
        break;
    case BABEL_FISH:
        babelFishSwarm();
        break;
    case PAN_GALACTIC:
        panGalacticGargleBlaster();
        break;
    case IMPROBABILITY:
        infiniteImprobabilityDrive();
        break;
    case VOGON_POETRY:
        vogonPoetrySlam();
        break;
    case GOLD_TRAIL:
        heartOfGoldTrail();
        break;
    case TOWEL_WAVE:
        towelWave();
        break;
    case ZAPHOD_DUAL:
        zaphodDualHeads();
        break;
    case GUIDE_MATRIX:
        guideMatrix(500);
        break;
    case IDLE:
        delay(10);
        break;
    }
}

void checkServer()
{
    WiFiClient client;
    HTTPClient http;
    http.begin(client, serverUrl);
    http.setTimeout(1000);

    int httpCode = http.GET();
    if (httpCode > 0)
    {
        String payload = http.getString();
        payload.trim();
        Serial.println("Guide Command: " + payload);
        newCommandReceived = true;

        if (payload.startsWith("COLORPULSE"))
        {
            int colonIndex = payload.indexOf(':');
            if (colonIndex != -1)
                pulseColor = parseColor(payload.substring(colonIndex + 1));
            currentEffect = DONT_PANIC;
        }
        else if (payload == "RAINBOW")
            currentEffect = RAINBOW;
        else if (payload == "BABELFISH")
            currentEffect = BABEL_FISH;
        else if (payload == "PANGLACTIC")
            currentEffect = PAN_GALACTIC;
        else if (payload == "IMPROBABILITY")
            currentEffect = IMPROBABILITY;
        else if (payload == "VOGONPOETRY")
            currentEffect = VOGON_POETRY;
        else if (payload == "GOLDTRAIL")
            currentEffect = GOLD_TRAIL;
        else if (payload == "TOWELWAVE")
            currentEffect = TOWEL_WAVE;
        else if (payload == "ZAPHODDUAL")
            currentEffect = ZAPHOD_DUAL;
        else if (payload == "GUIDEMATRIX")
            currentEffect = GUIDE_MATRIX;
        else if (payload == "RESET")
        {
            strip.clear();
            strip.show();
            currentEffect = IDLE;
        }
        else
            setRandomColor();
    }
    else
    {
        Serial.println("Guide Network Error: " + String(httpCode));
    }
    http.end();
}

// Effect Functions (Updated or Kept)
void rainbowEffect(int wait)
{
    static int j = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        strip.setPixelColor(i, strip.ColorHSV(((i + j) & 255) * 256, 255, 255));
    }
    strip.show();
    j = (j + 1) % 256;
    delay(wait);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void babelFishSwarm()
{
    static int j = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int hue = (i + j) % 170 * (65535 / 170);
        strip.setPixelColor(i, strip.ColorHSV(hue, 255, 255));
        if (random(20) == 0)
            strip.setPixelColor(i, strip.Color(0, 255, 255)); // Babel Fish dart
    }
    strip.show();
    j = (j + 1) % 170;
    delay(50);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void panGalacticGargleBlaster()
{
    static int cycle = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int r = random(2) ? 0 : 255 * (i % 3 == 0);
        int g = random(2) ? 255 * (i % 3 == 1) : 0;
        int b = random(2) ? 0 : 255 * (i % 3 == 2);
        if (random(20) == 0)
            strip.setPixelColor(i, strip.Color(255, 255, 255)); // Explosive fizz
        else
            strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();
    delay(random(30, 100));
    if (cycle++ % 10 == 0)
    {
        strip.clear();
        strip.show();
        delay(50);
    } // Blackout
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void infiniteImprobabilityDrive()
{
    static int cycle = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        if (cycle % 20 < 5)
        {
            strip.setPixelColor(i, strip.ColorHSV((i * 256 + cycle * 100) % 65535, 255, 255));
        }
        else
        {
            strip.setPixelColor(i, strip.Color(random(256), random(256), random(256)));
        }
    }
    strip.show();
    cycle++;
    delay(40);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void vogonPoetrySlam()
{
    static int j = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int brightness = (sin((i + j) * 0.1) + 1) * 127;
        strip.setPixelColor(i, strip.Color(0, brightness * 0.8, brightness * 0.4));
        if (random(50) == 0)
            strip.setPixelColor(i, strip.Color(255, 0, 0)); // Painful flash
    }
    strip.show();
    j++;
    delay(80);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void heartOfGoldTrail()
{
    static int pos = 0, dir = 1;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        uint32_t c = strip.getPixelColor(i);
        strip.setPixelColor(i, strip.Color(((c >> 16) & 0xFF) * 0.9, ((c >> 8) & 0xFF) * 0.9, (c & 0xFF) * 0.9));
        if (i == pos)
            strip.setPixelColor(i, strip.Color(255, 215, 0)); // Gold comet
        else if (abs(i - pos) < 10 && random(10) == 0)
            strip.setPixelColor(i, strip.Color(0, 0, random(100, 255))); // Stars
    }
    strip.show();
    pos += dir;
    if (pos >= NUM_LEDS || pos <= 0)
        dir = -dir;
    delay(20);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void dontPanicPulse()
{
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 80)
    {
        pulseBrightness += pulseDirection * 10;
        if (pulseBrightness >= 255 || pulseBrightness <= 20)
            pulseDirection *= -1;
        for (int i = 0; i < NUM_LEDS; i++)
        {
            strip.setPixelColor(i, dimColor(pulseColor, pulseBrightness));
        }
        strip.show();
        lastUpdate = millis();
    }
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void towelWave()
{
    static int wavePos = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int brightness = (sin((i + wavePos) * 0.1) + 1) * 127;
        strip.setPixelColor(i, strip.Color(brightness, brightness * 0.8, 0)); // Towel beige
    }
    strip.show();
    wavePos++;
    delay(50);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void zaphodDualHeads()
{
    static int cycle = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        if (i % 2 == cycle % 2)
        {
            strip.setPixelColor(i, strip.Color(255, 0, 255)); // Head 1: Magenta
        }
        else
        {
            strip.setPixelColor(i, strip.Color(0, 255, 0)); // Head 2: Green
        }
    }
    strip.show();
    cycle++;
    delay(200);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void guideMatrix(uint16_t speed)
{
    int is_on[NUM_LEDS] = {0};
    int total_width = WIDTH_G + WIDTH_U + WIDTH_I + WIDTH_D + WIDTH_E + 4; // Spaces between letters
    for (int x = 0; x < 32; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            int index = (x % 2 == 0) ? (y + x * 8) : ((7 - y) + x * 8);
            if (x - text_scroll_x >= 0 && x - text_scroll_x < WIDTH_G)
                is_on[index] = letter_G[y][x - text_scroll_x];
            else if (x - text_scroll_x >= WIDTH_G + 1 && x - text_scroll_x < WIDTH_G + WIDTH_U + 1)
                is_on[index] = letter_U[y][x - text_scroll_x - WIDTH_G - 1];
            else if (x - text_scroll_x >= WIDTH_G + WIDTH_U + 2 && x - text_scroll_x < WIDTH_G + WIDTH_U + WIDTH_I + 2)
                is_on[index] = letter_I[y][x - text_scroll_x - WIDTH_G - WIDTH_U - 2];
            else if (x - text_scroll_x >= WIDTH_G + WIDTH_U + WIDTH_I + 3 && x - text_scroll_x < WIDTH_G + WIDTH_U + WIDTH_I + WIDTH_D + 3)
                is_on[index] = letter_D[y][x - text_scroll_x - WIDTH_G - WIDTH_U - WIDTH_I - 3];
            else if (x - text_scroll_x >= WIDTH_G + WIDTH_U + WIDTH_I + WIDTH_D + 4 && x - text_scroll_x < total_width)
                is_on[index] = letter_E[y][x - text_scroll_x - WIDTH_G - WIDTH_U - WIDTH_I - WIDTH_D - 4];
        }
    }
    for (int i = 0; i < NUM_LEDS; i++)
    {
        strip.setPixelColor(i, strip.Color(is_on[i] ? 0 : 0, is_on[i] ? 255 : 0, is_on[i] ? 0 : 0));
    }
    strip.show();
    text_scroll_x += text_scroll_dir;
    if (text_scroll_x >= 32 - total_width)
        text_scroll_dir = -1;
    else if (text_scroll_x <= 0)
        text_scroll_dir = 1;
    delay(speed);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

// Utility Functions
uint32_t parseColor(String hexStr)
{
    hexStr.trim();
    if (hexStr.startsWith("#"))
        hexStr = hexStr.substring(1);
    if (hexStr.length() != 6)
        return strip.Color(0, 100, 150); // Default Guide green-blue
    long number = strtol(hexStr.c_str(), NULL, 16);
    return strip.Color((number >> 16) & 0xFF, (number >> 8) & 0xFF, number & 0xFF);
}

uint32_t dimColor(uint32_t color, uint8_t brightness)
{
    float scale = brightness / 255.0;
    return strip.Color(((color >> 16) & 0xFF) * scale, ((color >> 8) & 0xFF) * scale, (color & 0xFF) * scale);
}

void setRandomColor()
{
    setStripColor(strip.Color(random(256), random(256), random(256)));
    currentEffect = IDLE;
}

void setStripColor(uint32_t color)
{
    for (int i = 0; i < NUM_LEDS; i++)
        strip.setPixelColor(i, color);
    strip.show();
}