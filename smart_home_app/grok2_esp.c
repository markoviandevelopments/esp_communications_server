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

// HHGTTG-inspired effects with new additions
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
    GUIDE_MATRIX,  // Scrolling "GUIDE" text
    BEE_SWARM,     // Brownian bee motion
    LOVE_PRESTON,  // "I LOVE YOU, PRESTON" scrolling text
    MARVIN_MOAN,   // Depressed flickering
    FORD_SPARKLE,  // Random towel-inspired sparkles
    ARTHUR_TEA     // Warm tea glow effect
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

// "I LOVE YOU, PRESTON" letters
#define WIDTH_L 5
#define WIDTH_O 5
#define WIDTH_V 5
#define WIDTH_Y 5
#define WIDTH_P 5
#define WIDTH_R 5
#define WIDTH_S 5
#define WIDTH_T 5
#define WIDTH_N 5
int letter_L[HEIGHT][WIDTH_L] = {{1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}, {0, 0, 0, 0, 0}};
int letter_O[HEIGHT][WIDTH_O] = {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}, {0, 0, 0, 0, 0}};
int letter_V[HEIGHT][WIDTH_V] = {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, 0, 0}};
int letter_Y[HEIGHT][WIDTH_Y] = {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, 0, 0}};
int letter_P[HEIGHT][WIDTH_P] = {{1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
int letter_R[HEIGHT][WIDTH_R] = {{1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 0}, {1, 0, 0, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 0, 0, 0, 0}};
int letter_S[HEIGHT][WIDTH_S] = {{0, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {0, 1, 1, 1, 0}, {0, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {1, 1, 1, 1, 0}, {0, 0, 0, 0, 0}};
int letter_T[HEIGHT][WIDTH_T] = {{1, 1, 1, 1, 1}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, 0, 0}};
int letter_N[HEIGHT][WIDTH_N] = {{1, 0, 0, 0, 1}, {1, 1, 0, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 0, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 0, 0, 0, 0}};

// Global variables
int text_scroll_x = 0, text_scroll_dir = 1;
int bee_x = 16, bee_y = 4;
uint32_t pulseColor = strip.Color(0, 255, 42); // Default HHGTTG green
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
    case BEE_SWARM:
        matrixBee();
        break;
    case LOVE_PRESTON:
        lovePrestonMatrix(500);
        break;
    case MARVIN_MOAN:
        marvinMoan();
        break;
    case FORD_SPARKLE:
        fordSparkle();
        break;
    case ARTHUR_TEA:
        arthurTea();
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
                pulseColor = parseColor(payload.substring(colonIndex + 1)); // Use color picker value
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
        else if (payload == "BEESWARM")
            currentEffect = BEE_SWARM;
        else if (payload == "LOVEPRESTON")
            currentEffect = LOVE_PRESTON;
        else if (payload == "MARVINMOAN")
            currentEffect = MARVIN_MOAN;
        else if (payload == "FORDSPARKLE")
            currentEffect = FORD_SPARKLE;
        else if (payload == "ARTHURTEA")
            currentEffect = ARTHUR_TEA;
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

// Effect Functions
void rainbowEffect(int wait)
{
    unsigned long startTime = millis();
    for (int j = 0; j < 256; j++)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int hue = (i + j) & 255;
            strip.setPixelColor(i, strip.ColorHSV(hue * 256, 255, 255));
        }
        strip.show();
        if (newCommandReceived)
        {
            newCommandReceived = false;
            currentEffect = IDLE;
            return;
        }
        while (millis() - startTime < wait)
        {
            if (newCommandReceived)
            {
                newCommandReceived = false;
                currentEffect = IDLE;
                return;
            }
            delay(1);
        }
        startTime = millis();
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
            strip.setPixelColor(i, strip.Color(255, 255, 0)); // Yellow Babel Fish dart
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
    }
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
            strip.setPixelColor(i, strip.ColorHSV((i * 256 + cycle * 100) % 65535, 255, 255));
        else
            strip.setPixelColor(i, strip.Color(random(256), random(256), random(256)));
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
            strip.setPixelColor(i, dimColor(pulseColor, pulseBrightness)); // Uses color picker value
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
            strip.setPixelColor(i, strip.Color(255, 215, 0)); // Gold for Zaphod
        else
            strip.setPixelColor(i, strip.Color(0, 255, 42)); // HHGTTG green
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
    int total_width = WIDTH_G + WIDTH_U + WIDTH_I + WIDTH_D + WIDTH_E + 4;
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
        strip.setPixelColor(i, is_on[i] ? strip.Color(0, 255, 42) : strip.Color(255, 215, 0)); // Green vs gold
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

void matrixBee()
{
    int rr = random(4);
    if (rr == 0 && bee_x < 31)
        bee_x++;
    else if (rr == 1 && bee_x > 0)
        bee_x--;
    else if (rr == 2 && bee_y < 7)
        bee_y++;
    else if (rr == 3 && bee_y > 0)
        bee_y--;

    int bee_index = (bee_x % 2 == 0) ? (bee_y + bee_x * 8) : ((7 - bee_y) + bee_x * 8);
    for (int i = 0; i < NUM_LEDS; i++)
    {
        if (i == bee_index)
            strip.setPixelColor(i, strip.Color(255, 255, 0));
        else
            strip.setPixelColor(i, strip.Color(1, 1, 1));
    }
    strip.show();
    delay(1000);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void lovePrestonMatrix(uint16_t speed)
{
    int is_on[NUM_LEDS] = {0};
    int total_width = WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + WIDTH_P + WIDTH_R + WIDTH_E + WIDTH_S + WIDTH_T + WIDTH_O + WIDTH_N + 14;
    for (int x = 0; x < 32; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            int index = (x % 2 == 0) ? (y + x * 8) : ((7 - y) + x * 8);
            if (x - text_scroll_x >= 0 && x - text_scroll_x < WIDTH_I)
                is_on[index] = letter_I[y][x - text_scroll_x];
            else if (x - text_scroll_x >= WIDTH_I + 1 && x - text_scroll_x < WIDTH_I + WIDTH_L + 1)
                is_on[index] = letter_L[y][x - text_scroll_x - WIDTH_I - 1];
            else if (x - text_scroll_x >= WIDTH_I + WIDTH_L + 2 && x - text_scroll_x < WIDTH_I + WIDTH_L + WIDTH_O + 2)
                is_on[index] = letter_O[y][x - text_scroll_x - WIDTH_I - WIDTH_L - 2];
            else if (x - text_scroll_x >= WIDTH_I + WIDTH_L + WIDTH_O + 3 && x - text_scroll_x < WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + 3)
                is_on[index] = letter_V[y][x - text_scroll_x - WIDTH_I - WIDTH_L - WIDTH_O - 3];
            else if (x - text_scroll_x >= WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + 4 && x - text_scroll_x < WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + 4)
                is_on[index] = letter_E[y][x - text_scroll_x - WIDTH_I - WIDTH_L - WIDTH_O - WIDTH_V - 4];
            else if (x - text_scroll_x >= WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + 5 && x - text_scroll_x < WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + 5)
                is_on[index] = letter_Y[y][x - text_scroll_x - WIDTH_I - WIDTH_L - WIDTH_O - WIDTH_V - WIDTH_E - 5];
            else if (x - text_scroll_x >= WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + 6 && x - text_scroll_x < WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + 6)
                is_on[index] = letter_O[y][x - text_scroll_x - WIDTH_I - WIDTH_L - WIDTH_O - WIDTH_V - WIDTH_E - WIDTH_Y - 6];
            else if (x - text_scroll_x >= WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + 7 && x - text_scroll_x < WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + 7)
                is_on[index] = letter_U[y][x - text_scroll_x - WIDTH_I - WIDTH_L - WIDTH_O - WIDTH_V - WIDTH_E - WIDTH_Y - WIDTH_O - 7];
            else if (x - text_scroll_x >= WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + 8 && x - text_scroll_x < WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + WIDTH_P + 8)
                is_on[index] = letter_P[y][x - text_scroll_x - WIDTH_I - WIDTH_L - WIDTH_O - WIDTH_V - WIDTH_E - WIDTH_Y - WIDTH_O - WIDTH_U - 8];
            else if (x - text_scroll_x >= WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + WIDTH_P + 9 && x - text_scroll_x < WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + WIDTH_P + WIDTH_R + 9)
                is_on[index] = letter_R[y][x - text_scroll_x - WIDTH_I - WIDTH_L - WIDTH_O - WIDTH_V - WIDTH_E - WIDTH_Y - WIDTH_O - WIDTH_U - WIDTH_P - 9];
            else if (x - text_scroll_x >= WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + WIDTH_P + WIDTH_R + 10 && x - text_scroll_x < WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + WIDTH_P + WIDTH_R + WIDTH_E + 10)
                is_on[index] = letter_E[y][x - text_scroll_x - WIDTH_I - WIDTH_L - WIDTH_O - WIDTH_V - WIDTH_E - WIDTH_Y - WIDTH_O - WIDTH_U - WIDTH_P - WIDTH_R - 10];
            else if (x - text_scroll_x >= WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + WIDTH_P + WIDTH_R + WIDTH_E + 11 && x - text_scroll_x < WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + WIDTH_P + WIDTH_R + WIDTH_E + WIDTH_S + 11)
                is_on[index] = letter_S[y][x - text_scroll_x - WIDTH_I - WIDTH_L - WIDTH_O - WIDTH_V - WIDTH_E - WIDTH_Y - WIDTH_O - WIDTH_U - WIDTH_P - WIDTH_R - WIDTH_E - 11];
            else if (x - text_scroll_x >= WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + WIDTH_P + WIDTH_R + WIDTH_E + WIDTH_S + 12 && x - text_scroll_x < WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + WIDTH_P + WIDTH_R + WIDTH_E + WIDTH_S + WIDTH_T + 12)
                is_on[index] = letter_T[y][x - text_scroll_x - WIDTH_I - WIDTH_L - WIDTH_O - WIDTH_V - WIDTH_E - WIDTH_Y - WIDTH_O - WIDTH_U - WIDTH_P - WIDTH_R - WIDTH_E - WIDTH_S - 12];
            else if (x - text_scroll_x >= WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + WIDTH_P + WIDTH_R + WIDTH_E + WIDTH_S + WIDTH_T + 13 && x - text_scroll_x < WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + WIDTH_P + WIDTH_R + WIDTH_E + WIDTH_S + WIDTH_T + WIDTH_O + 13)
                is_on[index] = letter_O[y][x - text_scroll_x - WIDTH_I - WIDTH_L - WIDTH_O - WIDTH_V - WIDTH_E - WIDTH_Y - WIDTH_O - WIDTH_U - WIDTH_P - WIDTH_R - WIDTH_E - WIDTH_S - WIDTH_T - 13];
            else if (x - text_scroll_x >= WIDTH_I + WIDTH_L + WIDTH_O + WIDTH_V + WIDTH_E + WIDTH_Y + WIDTH_O + WIDTH_U + WIDTH_P + WIDTH_R + WIDTH_E + WIDTH_S + WIDTH_T + WIDTH_O + 14 && x - text_scroll_x < total_width)
                is_on[index] = letter_N[y][x - text_scroll_x - WIDTH_I - WIDTH_L - WIDTH_O - WIDTH_V - WIDTH_E - WIDTH_Y - WIDTH_O - WIDTH_U - WIDTH_P - WIDTH_R - WIDTH_E - WIDTH_S - WIDTH_T - WIDTH_O - 14];
        }
    }
    for (int i = 0; i < NUM_LEDS; i++)
    {
        strip.setPixelColor(i, is_on[i] ? strip.Color(255, 215, 0) : strip.Color(0, 255, 42)); // Gold text, green background
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

// New Effects
void marvinMoan()
{
    static int cycle = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int brightness = random(50, 100);
        strip.setPixelColor(i, strip.Color(brightness * 0.5, brightness * 0.5, brightness * 0.5)); // Dull gray
        if (random(20) == 0)
            strip.setPixelColor(i, strip.Color(0, 0, 0)); // Flicker off
    }
    strip.show();
    delay(random(100, 300));
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void fordSparkle()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        strip.setPixelColor(i, strip.Color(10, 10, 10)); // Near-black background
        if (random(20) == 0)
            strip.setPixelColor(i, strip.Color(255, 215, 0)); // Gold sparkles
    }
    strip.show();
    delay(100);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void arthurTea()
{
    static int brightness = 0;
    static int direction = 1;
    if (millis() % 80 == 0)
    {
        brightness += direction * 10;
        if (brightness >= 255 || brightness <= 50)
            direction *= -1;
        for (int i = 0; i < NUM_LEDS; i++)
        {
            strip.setPixelColor(i, strip.Color(brightness, brightness * 0.8, brightness * 0.4)); // Warm tea glow
        }
        strip.show();
    }
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
        return strip.Color(0, 255, 42); // Default HHGTTG green
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