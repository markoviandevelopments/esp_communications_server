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

enum EffectState
{
    IDLE,
    DONT_PANIC,
    RAINBOW,
    BABEL_FISH,
    PAN_GALACTIC,
    IMPROBABILITY,
    VOGON_POETRY,
    GOLD_TRAIL,
    BEE_SWARM,
    LOVE_PRESTON,
    COSMIC_DUST,
    EARTH_DEMOLITION,
    KRICKET_WARS,
    MILLIWAYS
};
EffectState currentEffect = IDLE;

volatile bool newCommandReceived = false;
unsigned long lastServerCheck = 0;
const int serverCheckInterval = 300;

// Matrix text definitions (unchanged, just ensuring C is included)
#define HEIGHT 8
#define WIDTH_L 5
#define WIDTH_I 3
#define WIDTH_F 5
#define WIDTH_E 5
#define WIDTH_T 5
#define WIDTH_H 5
#define WIDTH_U 5
#define WIDTH_N 5
#define WIDTH_V 5
#define WIDTH_R 5
#define WIDTH_S 5
#define WIDTH_A 5
#define WIDTH_D 5
#define WIDTH_Y 5
#define WIDTH_O 5
#define WIDTH_P 5
#define WIDTH_G 5
#define WIDTH_C 5
int letter_C[HEIGHT][WIDTH_C] = {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}, {0, 0, 0, 0, 0}};
int letter_G[HEIGHT][WIDTH_G] = {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}, {0, 0, 0, 0, 0}};
int letter_L[HEIGHT][WIDTH_L] = {{1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}, {0, 0, 0, 0, 0}};
int letter_I[HEIGHT][WIDTH_I] = {{0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 0, 0}};
int letter_F[HEIGHT][WIDTH_F] = {{1, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
int letter_E[HEIGHT][WIDTH_E] = {{1, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}, {0, 0, 0, 0, 0}};
int letter_T[HEIGHT][WIDTH_T] = {{1, 1, 1, 1, 1}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, 0, 0}};
int letter_H[HEIGHT][WIDTH_H] = {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 0, 0, 0, 0}};
int letter_U[HEIGHT][WIDTH_U] = {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}, {0, 0, 0, 0, 0}};
int letter_N[HEIGHT][WIDTH_N] = {{1, 0, 0, 0, 1}, {1, 1, 0, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 0, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 0, 0, 0, 0}};
int letter_V[HEIGHT][WIDTH_V] = {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, 0, 0}};
int letter_R[HEIGHT][WIDTH_R] = {{1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 0}, {1, 0, 0, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 0, 0, 0, 0}};
int letter_S[HEIGHT][WIDTH_S] = {{0, 1, 1, 1, 1}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {0, 1, 1, 1, 0}, {0, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {1, 1, 1, 1, 0}, {0, 0, 0, 0, 0}};
int letter_A[HEIGHT][WIDTH_A] = {{0, 0, 1, 0, 0}, {0, 1, 0, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 0, 0, 0, 0}};
int letter_D[HEIGHT][WIDTH_D] = {{1, 1, 1, 0, 0}, {1, 0, 0, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 1, 0}, {1, 1, 1, 0, 0}, {0, 0, 0, 0, 0}};
int letter_Y[HEIGHT][WIDTH_Y] = {{1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, 0, 0}};
int letter_O[HEIGHT][WIDTH_O] = {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}, {0, 0, 0, 0, 0}};
int letter_P[HEIGHT][WIDTH_P] = {{1, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {1, 1, 1, 1, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};

int text_scroll_x = 0, text_scroll_dir = 1;
uint32_t pulseColor = strip.Color(0, 255, 42);
int pulseBrightness = 0, pulseDirection = 1;

void setup()
{
    Serial.begin(115200);
    strip.begin();
    strip.show();
    connectToWiFi();
    randomSeed(ESP.getCycleCount());
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi lost, reconnecting...");
        connectToWiFi();
    }
    if (millis() - lastServerCheck >= serverCheckInterval)
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
        rainbowGlide(50);
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
    case BEE_SWARM:
        matrixBee();
        break;
    case LOVE_PRESTON:
        lovePrestonMatrix(500);
        break;
    case COSMIC_DUST:
        cosmicDust();
        break;
    case EARTH_DEMOLITION:
        earthDemolition();
        break;
    case KRICKET_WARS:
        kricketWars();
        break;
    case MILLIWAYS:
        milliways();
        break;
    case IDLE:
        delay(10);
        break;
    }
}

void connectToWiFi()
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to the Guide Network! IP: " + WiFi.localIP().toString());
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
        else if (payload == "BEESWARM")
            currentEffect = BEE_SWARM;
        else if (payload == "LOVEPRESTON")
            currentEffect = LOVE_PRESTON;
        else if (payload == "COSMICDUST")
            currentEffect = COSMIC_DUST;
        else if (payload == "EARTH")
            currentEffect = EARTH_DEMOLITION;
        else if (payload == "KRICKET")
            currentEffect = KRICKET_WARS;
        else if (payload == "MILLIWAYS")
            currentEffect = MILLIWAYS;
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

void rainbowGlide(int wait)
{
    static int offset = 0, pulsePos = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int hue = ((i + offset) % NUM_LEDS) * (65535 / NUM_LEDS);
        int brightness = 255;
        if (abs(i - pulsePos) < 10)
            brightness = 255 - abs(i - pulsePos) * 20;
        strip.setPixelColor(i, strip.ColorHSV(hue, 255, max(50, brightness)));
    }
    if (random(80) == 0) // Hyperspace ripple
    {
        int rippleCenter = random(NUM_LEDS);
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int dist = abs(i - rippleCenter);
            if (dist < 15)
                strip.setPixelColor(i, strip.Color(255, 255, 255));
        }
        strip.show();
        delay(50);
    }
    strip.show();
    offset = (offset + 1) % NUM_LEDS;
    pulsePos = (pulsePos + 2) % NUM_LEDS;
    delay(wait);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
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

void babelFishSwarm()
{
    static int j = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int hue = (i + j) % 170 * (65535 / 170);
        strip.setPixelColor(i, strip.ColorHSV(hue, 255, 255));
        if (random(20) == 0)
            strip.setPixelColor(i, strip.Color(255, 255, 0));
    }
    if (random(150) == 0)
    { // Translation glitch
        int start = random(NUM_LEDS - 20);
        for (int i = start; i < start + 20; i++)
        {
            strip.setPixelColor(i, strip.Color(100, 100, 100)); // Gray glitch
        }
        strip.show();
        delay(100);
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
            strip.setPixelColor(i, strip.Color(255, 255, 255));
        else
            strip.setPixelColor(i, strip.Color(r, g, b));
    }
    if (random(50) == 0)
    { // Fizz effect
        for (int i = 0; i < 5; i++)
        {
            int fizzPos = random(NUM_LEDS);
            strip.setPixelColor(fizzPos, strip.Color(255, 255, 255));
            strip.show();
            delay(20);
            strip.setPixelColor(fizzPos, strip.Color(0, 0, 0));
        }
    }
    strip.show();
    delay(random(30, 100));
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void infiniteImprobabilityDrive()
{
    static int cycle = 0, chaosMode = 0;
    static unsigned long lastShift = 0;
    int delayTime = random(20, 80);

    if (millis() - lastShift >= delayTime)
    {
        cycle++;
        if (random(30) == 0)
        {
            chaosMode = random(3);
            strip.clear();
        }

        switch (chaosMode)
        {
        case 0:
            for (int i = 0; i < NUM_LEDS; i++)
                strip.setPixelColor(i, strip.ColorHSV((i * 256 + cycle * 150) % 65535, 255, 255));
            break;
        case 1:
            for (int i = 0; i < NUM_LEDS; i++)
                if (random(5) == 0)
                    strip.setPixelColor(i, strip.Color(random(256), random(256), random(256)));
            break;
        case 2:
            for (int i = 0; i < NUM_LEDS; i++)
                strip.setPixelColor(i, random(2) ? strip.Color(0, 0, 0) : strip.ColorHSV(random(65535), 255, 255));
            if (random(50) == 0)
            {
                setStripColor(strip.Color(255, 255, 255));
                delay(50);
            }
            break;
        }

        if (random(200) == 0)
        { // "42" event
            strip.clear();
            for (int i = 0; i < 42; i++)
            {
                strip.setPixelColor(random(NUM_LEDS), strip.Color(42, 42, 42));
            }
            strip.show();
            delay(200);
        }
        else
        {
            strip.show();
        }

        lastShift = millis();

        if (random(100) == 0)
        {
            for (int i = 0; i < NUM_LEDS / 2; i++)
            {
                strip.setPixelColor(i, strip.Color(255, 0, 0));
                strip.setPixelColor(NUM_LEDS - 1 - i, strip.Color(0, 255, 0));
            }
            strip.show();
            delay(100);
        }
    }

    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void vogonPoetrySlam()
{
    static float j = 0;
    static int stanzaBreak = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int brightness = (sin((i + j) * 0.05) + 1) * 127;
        strip.setPixelColor(i, strip.Color(0, brightness * 0.6, brightness * 0.8));
        if (random(50) == 0)
            strip.setPixelColor(i, strip.Color(255, 0, 0));
    }
    if (stanzaBreak > 0)
    {
        for (int i = 0; i < NUM_LEDS; i++)
            strip.setPixelColor(i, dimColor(strip.getPixelColor(i), 50));
        strip.show();
        delay(200);
        if (stanzaBreak == 1)
        {
            setStripColor(strip.Color(255, 0, 0));
            strip.show();
            delay(100);
        }
        stanzaBreak--;
    }
    else if (random(100) == 0)
        stanzaBreak = 2;
    strip.show();
    j += 0.05;
    delay(100);
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
        strip.setPixelColor(i, strip.Color(((c >> 16) & 0xFF) * 0.85, ((c >> 8) & 0xFF) * 0.85, (c & 0xFF) * 0.85));
        if (i == pos)
            strip.setPixelColor(i, strip.Color(255, 215, 0));
        else if (abs(i - pos) < 15 && i < pos)
            if (random(5) == 0)
                strip.setPixelColor(i, strip.Color(255, random(150, 215), 0));
    }
    strip.show();
    pos += dir;
    if (random(50) == 0)
    {
        int newPos = random(NUM_LEDS);
        for (int i = 0; i < NUM_LEDS; i++)
        { // Warp shimmer
            int dist = abs(i - newPos);
            if (dist < 20)
                strip.setPixelColor(i, strip.Color(255, 215 - dist * 10, 0));
        }
        strip.show();
        delay(50);
        pos = newPos;
    }
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
    static int textPos = 0;
    if (millis() - lastUpdate >= 80)
    {
        pulseBrightness += pulseDirection * 10;
        if (pulseBrightness >= 255 || pulseBrightness <= 20)
            pulseDirection *= -1;

        // Background pulse
        for (int i = 0; i < NUM_LEDS; i++)
            strip.setPixelColor(i, dimColor(pulseColor, pulseBrightness));

        // Overlay "DON'T PANIC" scrolling text
        int total_width = 46; // D(5) + O(5) + N(5) + T(5) + 3 + P(5) + A(5) + N(5) + I(3) + C(5)
        int is_on[NUM_LEDS] = {0};
        for (int x = 0; x < 32; x++)
        {
            for (int y = 0; y < 8; y++)
            {
                int index = (x % 2 == 0) ? (y + x * 8) : ((7 - y) + x * 8);
                if (x - textPos >= 0 && x - textPos < WIDTH_D) // D: 0-4
                    is_on[index] = letter_D[y][x - textPos];
                else if (x - textPos >= 5 && x - textPos < 5 + WIDTH_O) // O: 5-9
                    is_on[index] = letter_O[y][x - textPos - 5];
                else if (x - textPos >= 10 && x - textPos < 10 + WIDTH_N) // N: 10-14
                    is_on[index] = letter_N[y][x - textPos - 10];
                else if (x - textPos >= 15 && x - textPos < 15 + WIDTH_T) // T: 15-19
                    is_on[index] = letter_T[y][x - textPos - 15];
                else if (x - textPos >= 23 && x - textPos < 23 + WIDTH_P) // P: 23-27
                    is_on[index] = letter_P[y][x - textPos - 23];
                else if (x - textPos >= 28 && x - textPos < 28 + WIDTH_A) // A: 28-32
                    is_on[index] = letter_A[y][x - textPos - 28];
                else if (x - textPos >= 33 && x - textPos < 33 + WIDTH_N) // N: 33-37
                    is_on[index] = letter_N[y][x - textPos - 33];
                else if (x - textPos >= 38 && x - textPos < 38 + WIDTH_I) // I: 38-40
                    is_on[index] = letter_I[y][x - textPos - 38];
                else if (x - textPos >= 41 && x - textPos < 41 + WIDTH_C) // C: 41-45
                    is_on[index] = letter_C[y][x - textPos - 41];
            }
        }
        for (int i = 0; i < NUM_LEDS; i++)
            if (is_on[i])
                strip.setPixelColor(i, strip.Color(0, 255, 0));

        strip.show();
        textPos = (textPos + 1) % (total_width + 32); // Loop after full scroll
        lastUpdate = millis();
    }
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
            strip.setPixelColor(i, strip.Color(2, 2, 2));
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
    int total_width = 82; // I (3) + 3 + LOVE (20) + 3 + YOU (15) + 3 + PRESTON (35) = 82
    for (int x = 0; x < 32; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            int index = (x % 2 == 0) ? (y + x * 8) : ((7 - y) + x * 8);
            if (x - text_scroll_x >= 0 && x - text_scroll_x < WIDTH_I)
                is_on[index] = letter_I[y][x - text_scroll_x];
            else if (x - text_scroll_x >= 6 && x - text_scroll_x < 6 + WIDTH_L)
                is_on[index] = letter_L[y][x - text_scroll_x - 6];
            else if (x - text_scroll_x >= 11 && x - text_scroll_x < 11 + WIDTH_O)
                is_on[index] = letter_O[y][x - text_scroll_x - 11];
            else if (x - text_scroll_x >= 16 && x - text_scroll_x < 16 + WIDTH_V)
                is_on[index] = letter_V[y][x - text_scroll_x - 16];
            else if (x - text_scroll_x >= 21 && x - text_scroll_x < 21 + WIDTH_E)
                is_on[index] = letter_E[y][x - text_scroll_x - 21];
            else if (x - text_scroll_x >= 29 && x - text_scroll_x < 29 + WIDTH_Y)
                is_on[index] = letter_Y[y][x - text_scroll_x - 29];
            else if (x - text_scroll_x >= 34 && x - text_scroll_x < 34 + WIDTH_O)
                is_on[index] = letter_O[y][x - text_scroll_x - 34];
            else if (x - text_scroll_x >= 39 && x - text_scroll_x < 39 + WIDTH_U)
                is_on[index] = letter_U[y][x - text_scroll_x - 39];
            else if (x - text_scroll_x >= 47 && x - text_scroll_x < 47 + WIDTH_P)
                is_on[index] = letter_P[y][x - text_scroll_x - 47];
            else if (x - text_scroll_x >= 52 && x - text_scroll_x < 52 + WIDTH_R)
                is_on[index] = letter_R[y][x - text_scroll_x - 52];
            else if (x - text_scroll_x >= 57 && x - text_scroll_x < 57 + WIDTH_E)
                is_on[index] = letter_E[y][x - text_scroll_x - 57];
            else if (x - text_scroll_x >= 62 && x - text_scroll_x < 62 + WIDTH_S)
                is_on[index] = letter_S[y][x - text_scroll_x - 62];
            else if (x - text_scroll_x >= 67 && x - text_scroll_x < 67 + WIDTH_T)
                is_on[index] = letter_T[y][x - text_scroll_x - 67];
            else if (x - text_scroll_x >= 72 && x - text_scroll_x < 72 + WIDTH_O)
                is_on[index] = letter_O[y][x - text_scroll_x - 72];
            else if (x - text_scroll_x >= 77 && x - text_scroll_x < 77 + WIDTH_N)
                is_on[index] = letter_N[y][x - text_scroll_x - 77];
        }
    }
    for (int i = 0; i < NUM_LEDS; i++)
    {
        strip.setPixelColor(i, is_on[i] ? strip.Color(191, 0, 48) : strip.Color(0, 0, 0));
    }
    strip.show();
    text_scroll_x += text_scroll_dir;
    if (text_scroll_x >= total_width - 32)
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

void cosmicDust()
{
    static int trail[NUM_LEDS] = {0};
    for (int i = 0; i < NUM_LEDS; i++)
    {
        trail[i] = max(0, trail[i] - 20);
        strip.setPixelColor(i, strip.Color(trail[i] / 2, trail[i] / 2, trail[i]));
    }
    if (random(10) == 0)
    {
        int pos = random(NUM_LEDS);
        trail[pos] = 255;
    }
    if (random(100) == 0)
    { // Nebula bloom
        int center = random(NUM_LEDS);
        for (int i = max(0, center - 20); i < min(NUM_LEDS, center + 20); i++)
        {
            int dist = abs(i - center);
            trail[i] = max(trail[i], 255 - dist * 10);
        }
    }
    strip.show();
    delay(60);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void earthDemolition()
{
    static int phase = 0;
    static int shipPos = NUM_LEDS;
    if (phase < 30) // Earth swirls
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int dist = abs(i - NUM_LEDS / 2);
            int swirl = (sin((i + phase) * 0.2) + 1) * 50;
            if (dist < phase * 2)
                strip.setPixelColor(i, strip.Color(0, 100 + swirl, 200 - swirl));
            else
                strip.setPixelColor(i, strip.Color(0, 0, 0));
        }
    }
    else if (phase < 50) // Vogon ship approaches
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int dist = abs(i - NUM_LEDS / 2);
            if (dist < 60)
                strip.setPixelColor(i, strip.Color(0, 150, 200));
            if (abs(i - shipPos) < 10)
                strip.setPixelColor(i, strip.Color(255, 255, 0)); // Yellow ship
        }
        shipPos -= 5;
    }
    else // Explosion and debris
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int dist = abs(i - NUM_LEDS / 2);
            if (dist < (phase - 50) * 3)
                strip.setPixelColor(i, strip.Color(255, 165 - (phase - 50) * 5, 0));
            else if (random(20) == 0 && phase < 70)
                strip.setPixelColor(i, strip.Color(255, 100, 0)); // Debris
            else
                strip.setPixelColor(i, strip.Color(0, 0, 0));
        }
        if (phase > 80)
            phase = 0; // Reset
    }
    strip.show();
    phase++;
    delay(40);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void kricketWars()
{
    static int robotPos[5] = {0, 60, 120, 180, 240};
    static int phase = 0;
    for (int i = 0; i < NUM_LEDS; i++)
        strip.setPixelColor(i, strip.Color(0, 0, 0));

    if (phase < 50) // Robots march and clash
    {
        for (int r = 0; r < 5; r++)
        {
            robotPos[r] += 2;
            if (robotPos[r] >= NUM_LEDS)
                robotPos[r] = 0;
            for (int i = max(0, robotPos[r] - 5); i < min(NUM_LEDS, robotPos[r] + 5); i++)
                strip.setPixelColor(i, strip.Color(255, 255, 255)); // White robots
            if (random(20) == 0)                                    // Red defensive bursts
            {
                int burst = random(NUM_LEDS);
                for (int j = max(0, burst - 10); j < min(NUM_LEDS, burst + 10); j++)
                    strip.setPixelColor(j, strip.Color(255, 0, 0));
            }
        }
    }
    else // Supernova
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int dist = abs(i - NUM_LEDS / 2);
            if (dist < (phase - 50) * 5)
                strip.setPixelColor(i, strip.Color(255, 255, 255));
            else
                strip.setPixelColor(i, strip.Color(0, 0, 0));
        }
        if (phase > 70)
            phase = 0; // Reset
    }
    strip.show();
    phase++;
    delay(50);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void milliways()
{
    static float swirl = 0;
    static int finale = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int brightness = (sin((i + swirl) * 0.1) + 1) * 127;
        strip.setPixelColor(i, strip.Color(brightness * 0.8, 0, brightness)); // Purple-gold swirl
    }
    if (random(30) == 0) // Firework bursts
    {
        int center = random(NUM_LEDS);
        for (int j = max(0, center - 10); j < min(NUM_LEDS, center + 10); j++)
            strip.setPixelColor(j, strip.Color(random(256), random(256), random(256)));
        strip.show();
        delay(50);
    }
    if (finale > 0)
    {
        for (int i = 0; i < NUM_LEDS; i++)
            strip.setPixelColor(i, strip.Color(255, 255, 255));
        strip.show();
        delay(200);
        finale--;
        if (finale == 0)
            swirl = 0; // Reset
    }
    else if (random(100) == 0)
        finale = 3; // Trigger finale
    strip.show();
    swirl += 0.2;
    delay(40);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

uint32_t parseColor(String hexStr)
{
    hexStr.trim();
    if (hexStr.startsWith("#"))
        hexStr = hexStr.substring(1);
    if (hexStr.length() != 6)
        return strip.Color(0, 255, 42);
    long number = strtol(hexStr.c_str(), NULL, 16);
    return strip.Color((number >> 16) & 0xFF, ((number >> 8) & 0xFF) * 0.9, (number & 0xFF) * 0.8);
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