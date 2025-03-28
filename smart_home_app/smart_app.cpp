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

// Define the EffectState enum
enum EffectState
{
    IDLE,
    DONT_PANIC,
    BABEL_FISH,
    PAN_GALACTIC,
    VOGON_POETRY,
    BEE_SWARM,
    LOVE_PRESTON,
    ZAPHOD_WAVE,
    TRILLIAN_SPARK,
    HEART_OF_GOLD_PULSE,
    SLARTI_DATA_STREAM,
    IMPROBABILITY_DRIVE
};

EffectState currentEffect = IDLE;

#define WIDTH_D 5
const uint8_t letter_D[8][WIDTH_D] = {
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 1, 1, 0},
    {1, 1, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0}};

#define WIDTH_O 5
const uint8_t letter_O[8][WIDTH_O] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0}};

#define WIDTH_N 5
const uint8_t letter_N[8][WIDTH_N] = {
    {1, 0, 0, 0, 1},
    {1, 1, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 0, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 0, 0, 0, 0}};

#define WIDTH_T 5
const uint8_t letter_T[8][WIDTH_T] = {
    {1, 1, 1, 1, 1},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}};

#define WIDTH_P 5
const uint8_t letter_P[8][WIDTH_P] = {
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}};

#define WIDTH_A 5
const uint8_t letter_A[8][WIDTH_A] = {
    {0, 0, 1, 0, 0},
    {0, 1, 0, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 0, 0, 0, 0}};

#define WIDTH_I 3
const uint8_t letter_I[8][WIDTH_I] = {
    {1, 1, 1},
    {0, 1, 0},
    {0, 1, 0},
    {0, 1, 0},
    {0, 1, 0},
    {0, 1, 0},
    {1, 1, 1},
    {0, 0, 0}};

#define WIDTH_C 5
const uint8_t letter_C[8][WIDTH_C] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0}};

#define WIDTH_L 5
const uint8_t letter_L[8][WIDTH_L] = {
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0}};

#define WIDTH_V 5
const uint8_t letter_V[8][WIDTH_V] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 0, 1, 0},
    {0, 1, 0, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}};

#define WIDTH_E 5
const uint8_t letter_E[8][WIDTH_E] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0}};

#define WIDTH_Y 5
const uint8_t letter_Y[8][WIDTH_Y] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0}};

#define WIDTH_U 5
const uint8_t letter_U[8][WIDTH_U] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0}};

#define WIDTH_R 5
const uint8_t letter_R[8][WIDTH_R] = {
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0},
    {1, 0, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 0, 1},
    {0, 0, 0, 0, 0}};

#define WIDTH_S 5
const uint8_t letter_S[8][WIDTH_S] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0}};

// Global variables
volatile bool newCommandReceived = false;
unsigned long lastServerCheck = 0;
const int serverCheckInterval = 300;

int text_scroll_x = 0, text_scroll_dir = 1;
uint32_t pulseColor = strip.Color(0, 255, 42);
int pulseBrightness = 0, pulseDirection = 1;
int chaosFactor = 50;     // Global randomness scale (0-100)
int bee_x = 0, bee_y = 0; // For matrixBee

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
    case BABEL_FISH:
        babelFishSwarm();
        break;
    case PAN_GALACTIC:
        panGalacticGargleBlaster();
        break;
    case VOGON_POETRY:
        vogonPoetrySlam();
        break;
    case BEE_SWARM:
        matrixBee();
        break;
    case LOVE_PRESTON:
        lovePrestonMatrix(500);
        break;
    case ZAPHOD_WAVE:
        zaphodCosmicWave();
        break;
    case TRILLIAN_SPARK:
        trillianSpark();
        break;
    case HEART_OF_GOLD_PULSE:
        heartOfGoldPulse();
        break;
    case SLARTI_DATA_STREAM:
        slartiDataStream();
        break;
    case IMPROBABILITY_DRIVE:
        improbabilityDrive();
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

        if (currentEffect != IDLE)
        {
            for (int i = 255; i >= 0; i -= 20)
            {
                for (int j = 0; j < NUM_LEDS; j++)
                {
                    strip.setPixelColor(j, dimColor(strip.getPixelColor(j), i));
                }
                strip.show();
                delay(10);
            }
        }

        int modeNum = payload.toInt();
        bool isNumeric = (payload.length() > 0 && payload.toInt() != 0) || payload == "0";
        if (payload == "RESET" || (isNumeric && modeNum == 0))
        {
            strip.clear();
            strip.show();
            currentEffect = IDLE;
        }
        else if (payload.startsWith("COLORPULSE"))
        {
            int colonIndex = payload.indexOf(':');
            if (colonIndex != -1)
                pulseColor = parseColor(payload.substring(colonIndex + 1));
            currentEffect = DONT_PANIC;
        }
        else if (payload == "BABELFISH" || (isNumeric && modeNum == 2))
            currentEffect = BABEL_FISH;
        else if (payload == "PANGLACTIC" || (isNumeric && modeNum == 3))
            currentEffect = PAN_GALACTIC;
        else if (payload == "VOGONPOETRY" || (isNumeric && modeNum == 4))
            currentEffect = VOGON_POETRY;
        else if (payload == "BEESWARM" || (isNumeric && modeNum == 5))
            currentEffect = BEE_SWARM;
        else if (payload == "LOVEPRESTON" || (isNumeric && modeNum == 6))
            currentEffect = LOVE_PRESTON;
        else if (payload == "ZAPHODWAVE" || (isNumeric && modeNum == 7))
            currentEffect = ZAPHOD_WAVE;
        else if (payload == "TRILLIANSPARK" || (isNumeric && modeNum == 9))
            currentEffect = TRILLIAN_SPARK;
        else if (payload == "HEARTOFGOLD" || (isNumeric && modeNum == 10))
            currentEffect = HEART_OF_GOLD_PULSE;
        else if (payload == "SLARTIDATA" || (isNumeric && modeNum == 11))
            currentEffect = SLARTI_DATA_STREAM;
        else if (payload == "IMPROBABILITY" || (isNumeric && modeNum == 12))
            currentEffect = IMPROBABILITY_DRIVE;
        else if (payload.startsWith("CHAOS:"))
        {
            chaosFactor = payload.substring(6).toInt();
            chaosFactor = constrain(chaosFactor, 0, 100);
            Serial.println("Chaos Factor set to: " + String(chaosFactor));
        }
        else
        {
            setRandomColor();
        }
    }
    else
    {
        Serial.println("Guide Network Error: " + String(httpCode));
    }
    http.end();
}

void dontPanicPulse()
{
    static unsigned long lastUpdate = 0;
    static int textPos = 0;
    if (millis() - lastUpdate >= 60) // Faster pulse
    {
        pulseBrightness += pulseDirection * 20; // Sharper fade
        if (pulseBrightness >= 255 || pulseBrightness <= 40)
            pulseDirection *= -1;

        uint32_t bgColor = dimColor(pulseColor, pulseBrightness);

        for (int i = 0; i < NUM_LEDS; i++)
            strip.setPixelColor(i, bgColor);

        int total_width = 46;
        int is_on[NUM_LEDS] = {0};
        for (int x = 0; x < 32; x++)
        {
            for (int y = 0; y < 8; y++)
            {
                int index = (x % 2 == 0) ? (y + x * 8) : ((7 - y) + x * 8);
                int glitch = (random(100) < chaosFactor / 10) ? random(2) : 0;
                if (x - textPos >= 0 && x - textPos < WIDTH_D)
                    is_on[index] = glitch ? !letter_D[y][x - textPos] : letter_D[y][x - textPos];
                else if (x - textPos >= 5 && x - textPos < 10)
                    is_on[index] = glitch ? !letter_O[y][x - textPos - 5] : letter_O[y][x - textPos - 5];
                else if (x - textPos >= 10 && x - textPos < 15)
                    is_on[index] = glitch ? !letter_N[y][x - textPos - 10] : letter_N[y][x - textPos - 10];
                else if (x - textPos >= 15 && x - textPos < 20)
                    is_on[index] = glitch ? !letter_T[y][x - textPos - 15] : letter_T[y][x - textPos - 15];
                else if (x - textPos >= 23 && x - textPos < 28)
                    is_on[index] = glitch ? !letter_P[y][x - textPos - 23] : letter_P[y][x - textPos - 23];
                else if (x - textPos >= 28 && x - textPos < 33)
                    is_on[index] = glitch ? !letter_A[y][x - textPos - 28] : letter_A[y][x - textPos - 28];
                else if (x - textPos >= 33 && x - textPos < 38)
                    is_on[index] = glitch ? !letter_N[y][x - textPos - 33] : letter_N[y][x - textPos - 33];
                else if (x - textPos >= 38 && x - textPos < 41)
                    is_on[index] = glitch ? !letter_I[y][x - textPos - 38] : letter_I[y][x - textPos - 38];
                else if (x - textPos >= 41 && x - textPos < 46)
                    is_on[index] = glitch ? !letter_C[y][x - textPos - 41] : letter_C[y][x - textPos - 41];
            }
        }
        for (int i = 0; i < NUM_LEDS; i++)
            if (is_on[i])
                strip.setPixelColor(i, strip.Color(0, 255, 0)); // Green text

        strip.show();
        textPos = (textPos + 1) % (total_width + 32);
        lastUpdate = millis();
    }
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
            strip.setPixelColor(i, strip.Color(255, 255, 0));
    }
    if (random(150 - chaosFactor) == 0)
    {
        int start = random(NUM_LEDS - 20);
        for (int i = start; i < start + 20; i++)
        {
            strip.setPixelColor(i, strip.Color(100, 100, 100));
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
    if (random(50 - chaosFactor / 2) == 0)
    {
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

void vogonPoetrySlam()
{
    static int j = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int brightness = (sin((i + j) * 0.1) + 1) * 127;
        strip.setPixelColor(i, strip.Color(0, brightness * 0.8, brightness * 0.4));
        if (random(30) == 0)
            strip.setPixelColor(i, strip.Color(255, 0, 0));
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
    static unsigned long lastUpdate = 0;
    int is_on[NUM_LEDS] = {0};
    int total_width = 82;

    if (millis() - lastUpdate >= speed)
    {
        strip.clear(); // Black background

        // Scrolling text: "I LOVE YOU PRESTON" in red
        for (int x = 0; x < 32; x++)
        {
            for (int y = 0; y < 8; y++)
            {
                int index = (x % 2 == 0) ? (y + x * 8) : ((7 - y) + x * 8);
                if (x - text_scroll_x >= 0 && x - text_scroll_x < WIDTH_I)
                    is_on[index] = letter_I[y][x - text_scroll_x];
                else if (x - text_scroll_x >= 6 && x - text_scroll_x < 11)
                    is_on[index] = letter_L[y][x - text_scroll_x - 6];
                else if (x - text_scroll_x >= 11 && x - text_scroll_x < 16)
                    is_on[index] = letter_O[y][x - text_scroll_x - 11];
                else if (x - text_scroll_x >= 16 && x - text_scroll_x < 21)
                    is_on[index] = letter_V[y][x - text_scroll_x - 16];
                else if (x - text_scroll_x >= 21 && x - text_scroll_x < 26)
                    is_on[index] = letter_E[y][x - text_scroll_x - 21];
                else if (x - text_scroll_x >= 29 && x - text_scroll_x < 34)
                    is_on[index] = letter_Y[y][x - text_scroll_x - 29];
                else if (x - text_scroll_x >= 34 && x - text_scroll_x < 39)
                    is_on[index] = letter_O[y][x - text_scroll_x - 34];
                else if (x - text_scroll_x >= 39 && x - text_scroll_x < 44)
                    is_on[index] = letter_U[y][x - text_scroll_x - 39];
                else if (x - text_scroll_x >= 47 && x - text_scroll_x < 52)
                    is_on[index] = letter_P[y][x - text_scroll_x - 47];
                else if (x - text_scroll_x >= 52 && x - text_scroll_x < 57)
                    is_on[index] = letter_R[y][x - text_scroll_x - 52];
                else if (x - text_scroll_x >= 57 && x - text_scroll_x < 62)
                    is_on[index] = letter_E[y][x - text_scroll_x - 57];
                else if (x - text_scroll_x >= 62 && x - text_scroll_x < 67)
                    is_on[index] = letter_S[y][x - text_scroll_x - 62];
                else if (x - text_scroll_x >= 67 && x - text_scroll_x < 72)
                    is_on[index] = letter_T[y][x - text_scroll_x - 67];
                else if (x - text_scroll_x >= 72 && x - text_scroll_x < 77)
                    is_on[index] = letter_O[y][x - text_scroll_x - 72];
                else if (x - text_scroll_x >= 77 && x - text_scroll_x < 82)
                    is_on[index] = letter_N[y][x - text_scroll_x - 77];
            }
        }

        // Set red text on black background
        for (int i = 0; i < NUM_LEDS; i++)
        {
            if (is_on[i])
                strip.setPixelColor(i, strip.Color(255, 0, 0)); // Red text
        }

        strip.show();
        text_scroll_x += text_scroll_dir;
        if (text_scroll_x >= total_width - 32)
            text_scroll_dir = -1;
        else if (text_scroll_x <= 0)
            text_scroll_dir = 1;
        lastUpdate = millis();
    }

    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void zaphodCosmicWave()
{
    static unsigned long lastUpdate = 0;
    static float wavePos1 = 0.0;  // Primary wave
    static float wavePos2 = 50.0; // Secondary wave (offset)
    const float waveSpeed = 0.15; // Slightly faster for energy
    const float waveWidth = 30.0; // Wider for smoother gradients

    if (millis() - lastUpdate >= 15) // Faster updates for fluidity
    {
        strip.clear();

        for (int i = 0; i < NUM_LEDS; i++)
        {
            // Two overlapping waves for depth
            float dist1 = fabs(i - wavePos1);
            float dist2 = fabs(i - wavePos2);
            float intensity1 = max(0, cos((dist1 / waveWidth) * PI) * 255); // Smooth cosine wave
            float intensity2 = max(0, cos((dist2 / waveWidth) * PI) * 150); // Secondary, dimmer

            // Dual-head Zaphod vibe: split colors
            uint8_t r = intensity1 * (i % 2 == 0) + intensity2 * 0.5;
            uint8_t g = intensity1 * 0.7;
            uint8_t b = intensity2 * (i % 2 == 1) + intensity1 * 0.5;
            strip.setPixelColor(i, strip.Color(r, g, b));

            // Trailing glow effect
            if (dist1 < waveWidth * 1.5 || dist2 < waveWidth * 1.5)
            {
                uint8_t glow = max(0, 50 - (dist1 + dist2) * 2);
                strip.setPixelColor(i, strip.Color(r + glow, g + glow * 0.8, b + glow));
            }
        }

        // Zaphod’s cosmic flair: random bursts
        if (random(100) < chaosFactor / 8)
        {
            int burstPos = random(NUM_LEDS);
            strip.setPixelColor(burstPos, strip.ColorHSV(random(65535), 255, 255)); // Wild color flash
        }

        strip.show();
        wavePos1 += waveSpeed;
        wavePos2 += waveSpeed * 1.2; // Secondary wave moves slightly faster
        if (wavePos1 >= NUM_LEDS + waveWidth)
            wavePos1 = -waveWidth;
        if (wavePos2 >= NUM_LEDS + waveWidth)
            wavePos2 = -waveWidth;

        lastUpdate = millis();
    }

    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void trillianSpark()
{
    static unsigned long lastUpdate = 0;
    static float puckPos = 0.0;          // Position of the puck's leading edge
    static float puckSpeed = 12.0;       // Initial speed (LEDs per frame)
    static uint8_t hue = 0;              // Color hue (0-255)
    static int direction = 1;            // 1 = right, -1 = left
    static int bounceCount = 0;          // Track number of bounces
    static bool paused = false;          // Pause state
    static unsigned long pauseStart = 0; // Time pause began
    const int puckSize = 15;             // Size of the puck
    const float friction = 0.88;         // Slowdown factor per frame
    const int pauseDuration = 1000;      // Pause time in ms

    if (millis() - lastUpdate >= 10) // Update every 10ms for smooth motion
    {
        if (paused)
        {
            // Check if pause is over
            if (millis() - pauseStart >= pauseDuration)
            {
                // Reset for next cycle with slight randomization
                puckPos = 0.0;
                puckSpeed = 12.0 + random(-2, 3); // Initial speed varies slightly (10-14)
                direction = 1;
                hue = random(256); // Random starting color
                bounceCount = 0;
                paused = false;
            }
        }
        else
        {
            strip.clear();

            // Update position and speed
            puckPos += puckSpeed * direction;
            puckSpeed *= friction;

            // Bounce logic
            if (puckPos + puckSize >= NUM_LEDS) // Hit right end
            {
                puckPos = NUM_LEDS - puckSize; // Snap to edge
                direction = -1;
                bounceCount++;
                hue = (hue + 64) % 256;                // Shift color by ~1/4 hue circle
                puckSpeed = max(puckSpeed * 0.8, 2.0); // Retain some speed, min 2
            }
            else if (puckPos <= 0) // Hit left end
            {
                puckPos = 0;
                direction = 1;
                bounceCount++;
                hue = (hue + 64) % 256; // Shift color
                puckSpeed = max(puckSpeed * 0.8, 2.0);
            }

            // Draw the puck with a smooth gradient
            int startPos = (int)puckPos;
            for (int i = max(0, startPos); i < min(NUM_LEDS, startPos + puckSize); i++)
            {
                int brightness = 255 - abs(i - (startPos + puckSize / 2)) * 255 / (puckSize / 2);
                brightness = max(brightness, 0); // Ensure no negative values
                strip.setPixelColor(i, strip.ColorHSV(hue * 257, 255, brightness));
            }

            strip.show();

            // Check if puck has stopped (speed too low) and randomize bounce count
            int maxBounces = 4 + random(0, 3); // 4-6 bounces
            if ((puckSpeed < 0.5 && bounceCount > 0) || bounceCount >= maxBounces)
            {
                paused = true;
                pauseStart = millis();
            }

            lastUpdate = millis();
        }
    }

    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void heartOfGoldPulse()
{
    static unsigned long lastUpdate = 0;
    static struct Particle
    {
        float pos;      // Position on strip
        float speed;    // Speed (positive = right, negative = left)
        uint8_t bright; // Brightness (decays over time)
        bool active;    // Is this particle alive?
    } particles[30];    // Up to 30 particles
    static int particleCount = 0;
    static float explosionCenter = NUM_LEDS / 2; // Center of explosion
    static bool exploding = false;               // State: exploding or collapsing
    static unsigned long phaseStart = 0;         // Time of current phase start

    if (millis() - lastUpdate >= 15) // Smooth updates every 15ms
    {
        strip.clear();

        if (!exploding && particleCount == 0)
        {
            // Start explosion phase
            exploding = true;
            phaseStart = millis();
            explosionCenter = random(NUM_LEDS / 4, NUM_LEDS * 3 / 4); // Random center point
            particleCount = 20 + random(0, 11);                       // 20-30 particles
            for (int i = 0; i < particleCount; i++)
            {
                float angle = random(0, 628) / 100.0; // Random angle in radians (0-2π)
                float speed = random(5, 10);          // Initial speed
                particles[i] = {
                    explosionCenter,           // Start at center
                    cos(angle) * speed,        // Horizontal speed (organic spread)
                    (uint8_t)random(200, 255), // Bright golden spark
                    true};
            }
        }

        // Update particles
        for (int i = 0; i < particleCount; i++)
        {
            if (!particles[i].active)
                continue;

            // Movement logic
            if (exploding)
            {
                // Explosion phase: particles disperse
                particles[i].pos += particles[i].speed;
                particles[i].speed *= 0.92;                                        // Friction slows them down
                particles[i].bright = max(0, particles[i].bright - random(5, 15)); // Fade
            }
            else
            {
                // Collapse phase: gravitational pull back to center
                float distToCenter = explosionCenter - particles[i].pos;
                particles[i].speed += distToCenter * 0.005; // Acceleration toward center
                particles[i].speed *= 0.95;                 // Dampening for smooth stop
                particles[i].pos += particles[i].speed;
                particles[i].bright = max(0, particles[i].bright - random(3, 10)); // Slower fade
            }

            // Draw particle with golden hue
            int pos = (int)particles[i].pos;
            if (pos >= 0 && pos < NUM_LEDS)
            {
                uint8_t r = particles[i].bright;
                uint8_t g = particles[i].bright * 0.8;
                uint8_t b = particles[i].bright * 0.2;
                strip.setPixelColor(pos, strip.Color(r, g, b));
                // Faint trail for organic feel
                if (pos - 1 >= 0)
                    strip.setPixelColor(pos - 1, strip.Color(r / 4, g / 4, b / 4));
                if (pos + 1 < NUM_LEDS)
                    strip.setPixelColor(pos + 1, strip.Color(r / 4, g / 4, b / 4));
            }

            // Deactivate if faded or off-strip
            if (particles[i].bright == 0 || particles[i].pos < 0 || particles[i].pos >= NUM_LEDS)
            {
                particles[i].active = false;
                particleCount--;
                particles[i] = particles[particleCount]; // Swap with last active
                i--;                                     // Re-check this index
            }
        }

        // Switch to collapse phase after explosion slows
        if (exploding && millis() - phaseStart > 1000) // 1-second explosion duration
        {
            exploding = false;
            phaseStart = millis();
        }

        // Reset when all particles converge or fade
        if (!exploding && particleCount > 0)
        {
            bool allConverged = true;
            for (int i = 0; i < particleCount; i++)
            {
                if (abs(particles[i].pos - explosionCenter) > 5 || abs(particles[i].speed) > 0.5)
                {
                    allConverged = false;
                    break;
                }
            }
            if (allConverged || millis() - phaseStart > 2000) // Max 2-second collapse
            {
                particleCount = 0; // Reset for next explosion
            }
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

void slartiDataStream()
{
    static unsigned long lastUpdate = 0;
    static int streamOffset = 0; // Shifts entire stream
    const int streamSpeed = 5;   // Pixels per frame
    const int segmentSize = 10;  // Length of data "bits"

    if (millis() - lastUpdate >= 15)
    {
        strip.clear();

        // Continuous data stream
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int pos = (i + streamOffset) % (segmentSize * 2);                // Cycle through segments
            bool isData = pos < segmentSize;                                 // Binary on/off pattern
            uint8_t brightness = isData ? random(100, 255) : random(20, 50); // Data bright, gaps dim
            uint8_t g = brightness * 0.9;                                    // Greenish-cyan data vibe
            uint8_t b = brightness;
            uint8_t r = brightness * (random(100) < 10 ? 1 : 0); // Occasional red glitch
            strip.setPixelColor(i, strip.Color(r, g, b));
        }

        // Bee scooter chasing the stream
        static int beePos = NUM_LEDS;
        beePos -= streamSpeed / 2;
        if (beePos < 0)
            beePos = NUM_LEDS;
        strip.setPixelColor(beePos, strip.Color(255, 255, 0));

        // Fjord sparkles
        if (random(100) < chaosFactor / 5)
        {
            int sparklePos = random(NUM_LEDS);
            strip.setPixelColor(sparklePos, strip.Color(255, 255, 255));
        }

        strip.show();
        streamOffset += streamSpeed;
        if (streamOffset >= segmentSize * 2)
            streamOffset = 0;

        lastUpdate = millis();
    }

    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void improbabilityDrive()
{
    static unsigned long lastUpdate = 0;
    static float entropyGrid[NUM_LEDS] = {0}; // Floating-point for smoother transitions
    static uint16_t hueShift = 0;

    if (millis() - lastUpdate >= 20) // Smoother frame rate
    {
        // Update entropy simulation
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int prob = random(100);
            if (entropyGrid[i] > 0)
                entropyGrid[i] = max(0, entropyGrid[i] - random(5, 15)); // Slower decay
            if (prob < chaosFactor / 2)
                entropyGrid[i] = min(255, entropyGrid[i] + random(50, 100)); // Gradual buildup

            // Smooth color transition
            uint8_t brightness = (uint8_t)entropyGrid[i];
            strip.setPixelColor(i, strip.ColorHSV(hueShift + (i * 200), 255, brightness));
            if (brightness < 10)
                strip.setPixelColor(i, strip.Color(5, 5, 5)); // Dim void
        }

        // Improbability bursts
        if (random(100) < chaosFactor / 4)
        {
            int burstStart = random(NUM_LEDS - 20);
            for (int i = burstStart; i < burstStart + 20 && i < NUM_LEDS; i++)
            {
                entropyGrid[i] = min(255, entropyGrid[i] + random(100, 200)); // Ramp up smoothly
            }
        }

        strip.show();
        hueShift = (hueShift + random(20, 60)) % 65535; // Gentler hue shifts
        lastUpdate = millis();
    }

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

Arduino: 1.8.19 (Linux), Board: "Generic ESP8266 Module, 80 MHz, Flash, Disabled (new aborts on oom), Disabled, All SSL ciphers (most compatible), 32KB cache + 32KB IRAM (balanced), Use pgm_read macros for IRAM/PROGMEM, dtr (aka nodemcu), 26 MHz, 40MHz, DOUT (compatible), 1MB (FS:64KB OTA:~470KB), 2, nonos-sdk 2.2.1+100 (190703), v2 Lower Memory, Disabled, None, Only Sketch, 115200"

/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void zaphodCosmicWave()':
sketch_feb02b:624:74: error: no matching function for call to 'max(int, double)'
  624 |             float intensity1 = max(0, cos((dist1 / waveWidth) * PI) * 255); // Smooth cosine wave
      |                                                                          ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3491:5: note: candidate: 'template<class _Tp, class _Compare> constexpr _Tp std::max(std::initializer_list<_Tp>, _Compare)'
 3491 |     max(initializer_list<_Tp> __l, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3491:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:624:74: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  624 |             float intensity1 = max(0, cos((dist1 / waveWidth) * PI) * 255); // Smooth cosine wave
      |                                                                          ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3485:5: note: candidate: 'template<class _Tp> constexpr _Tp std::max(std::initializer_list<_Tp>)'
 3485 |     max(initializer_list<_Tp> __l)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3485:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:624:74: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  624 |             float intensity1 = max(0, cos((dist1 / waveWidth) * PI) * 255); // Smooth cosine wave
      |                                                                          ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:300:5: note: candidate: 'template<class _Tp, class _Compare> constexpr const _Tp& std::max(const _Tp&, const _Tp&, _Compare)'
  300 |     max(const _Tp& __a, const _Tp& __b, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:300:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:624:74: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'double')
  624 |             float intensity1 = max(0, cos((dist1 / waveWidth) * PI) * 255); // Smooth cosine wave
      |                                                                          ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:254:5: note: candidate: 'template<class _Tp> constexpr const _Tp& std::max(const _Tp&, const _Tp&)'
  254 |     max(const _Tp& __a, const _Tp& __b)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:254:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:624:74: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'double')
  624 |             float intensity1 = max(0, cos((dist1 / waveWidth) * PI) * 255); // Smooth cosine wave
      |                                                                          ^
sketch_feb02b:625:74: error: no matching function for call to 'max(int, double)'
  625 |             float intensity2 = max(0, cos((dist2 / waveWidth) * PI) * 150); // Secondary, dimmer
      |                                                                          ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3491:5: note: candidate: 'template<class _Tp, class _Compare> constexpr _Tp std::max(std::initializer_list<_Tp>, _Compare)'
 3491 |     max(initializer_list<_Tp> __l, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3491:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:625:74: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  625 |             float intensity2 = max(0, cos((dist2 / waveWidth) * PI) * 150); // Secondary, dimmer
      |                                                                          ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3485:5: note: candidate: 'template<class _Tp> constexpr _Tp std::max(std::initializer_list<_Tp>)'
 3485 |     max(initializer_list<_Tp> __l)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3485:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:625:74: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  625 |             float intensity2 = max(0, cos((dist2 / waveWidth) * PI) * 150); // Secondary, dimmer
      |                                                                          ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:300:5: note: candidate: 'template<class _Tp, class _Compare> constexpr const _Tp& std::max(const _Tp&, const _Tp&, _Compare)'
  300 |     max(const _Tp& __a, const _Tp& __b, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:300:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:625:74: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'double')
  625 |             float intensity2 = max(0, cos((dist2 / waveWidth) * PI) * 150); // Secondary, dimmer
      |                                                                          ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:254:5: note: candidate: 'template<class _Tp> constexpr const _Tp& std::max(const _Tp&, const _Tp&)'
  254 |     max(const _Tp& __a, const _Tp& __b)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:254:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:625:74: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'double')
  625 |             float intensity2 = max(0, cos((dist2 / waveWidth) * PI) * 150); // Secondary, dimmer
      |                                                                          ^
sketch_feb02b:636:63: error: no matching function for call to 'max(int, float)'
  636 |                 uint8_t glow = max(0, 50 - (dist1 + dist2) * 2);
      |                                                               ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3491:5: note: candidate: 'template<class _Tp, class _Compare> constexpr _Tp std::max(std::initializer_list<_Tp>, _Compare)'
 3491 |     max(initializer_list<_Tp> __l, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3491:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:636:63: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  636 |                 uint8_t glow = max(0, 50 - (dist1 + dist2) * 2);
      |                                                               ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3485:5: note: candidate: 'template<class _Tp> constexpr _Tp std::max(std::initializer_list<_Tp>)'
 3485 |     max(initializer_list<_Tp> __l)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3485:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:636:63: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  636 |                 uint8_t glow = max(0, 50 - (dist1 + dist2) * 2);
      |                                                               ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:300:5: note: candidate: 'template<class _Tp, class _Compare> constexpr const _Tp& std::max(const _Tp&, const _Tp&, _Compare)'
  300 |     max(const _Tp& __a, const _Tp& __b, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:300:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:636:63: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'float')
  636 |                 uint8_t glow = max(0, 50 - (dist1 + dist2) * 2);
      |                                                               ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:254:5: note: candidate: 'template<class _Tp> constexpr const _Tp& std::max(const _Tp&, const _Tp&)'
  254 |     max(const _Tp& __a, const _Tp& __b)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:254:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:636:63: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'float')
  636 |                 uint8_t glow = max(0, 50 - (dist1 + dist2) * 2);
      |                                                               ^
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void heartOfGoldPulse()':
sketch_feb02b:802:81: error: no matching function for call to 'max(int, long int)'
  802 |                 particles[i].bright = max(0, particles[i].bright - random(5, 15)); // Fade
      |                                                                                 ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3491:5: note: candidate: 'template<class _Tp, class _Compare> constexpr _Tp std::max(std::initializer_list<_Tp>, _Compare)'
 3491 |     max(initializer_list<_Tp> __l, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3491:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:802:81: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  802 |                 particles[i].bright = max(0, particles[i].bright - random(5, 15)); // Fade
      |                                                                                 ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3485:5: note: candidate: 'template<class _Tp> constexpr _Tp std::max(std::initializer_list<_Tp>)'
 3485 |     max(initializer_list<_Tp> __l)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3485:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:802:81: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  802 |                 particles[i].bright = max(0, particles[i].bright - random(5, 15)); // Fade
      |                                                                                 ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:300:5: note: candidate: 'template<class _Tp, class _Compare> constexpr const _Tp& std::max(const _Tp&, const _Tp&, _Compare)'
  300 |     max(const _Tp& __a, const _Tp& __b, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:300:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:802:81: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'long int')
  802 |                 particles[i].bright = max(0, particles[i].bright - random(5, 15)); // Fade
      |                                                                                 ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:254:5: note: candidate: 'template<class _Tp> constexpr const _Tp& std::max(const _Tp&, const _Tp&)'
  254 |     max(const _Tp& __a, const _Tp& __b)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:254:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:802:81: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'long int')
  802 |                 particles[i].bright = max(0, particles[i].bright - random(5, 15)); // Fade
      |                                                                                 ^
sketch_feb02b:811:81: error: no matching function for call to 'max(int, long int)'
  811 |                 particles[i].bright = max(0, particles[i].bright - random(3, 10)); // Slower fade
      |                                                                                 ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3491:5: note: candidate: 'template<class _Tp, class _Compare> constexpr _Tp std::max(std::initializer_list<_Tp>, _Compare)'
 3491 |     max(initializer_list<_Tp> __l, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3491:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:811:81: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  811 |                 particles[i].bright = max(0, particles[i].bright - random(3, 10)); // Slower fade
      |                                                                                 ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3485:5: note: candidate: 'template<class _Tp> constexpr _Tp std::max(std::initializer_list<_Tp>)'
 3485 |     max(initializer_list<_Tp> __l)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3485:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:811:81: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  811 |                 particles[i].bright = max(0, particles[i].bright - random(3, 10)); // Slower fade
      |                                                                                 ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:300:5: note: candidate: 'template<class _Tp, class _Compare> constexpr const _Tp& std::max(const _Tp&, const _Tp&, _Compare)'
  300 |     max(const _Tp& __a, const _Tp& __b, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:300:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:811:81: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'long int')
  811 |                 particles[i].bright = max(0, particles[i].bright - random(3, 10)); // Slower fade
      |                                                                                 ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:254:5: note: candidate: 'template<class _Tp> constexpr const _Tp& std::max(const _Tp&, const _Tp&)'
  254 |     max(const _Tp& __a, const _Tp& __b)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:254:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:811:81: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'long int')
  811 |                 particles[i].bright = max(0, particles[i].bright - random(3, 10)); // Slower fade
      |                                                                                 ^
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void improbabilityDrive()':
sketch_feb02b:940:71: error: no matching function for call to 'max(int, float)'
  940 |                 entropyGrid[i] = max(0, entropyGrid[i] - random(5, 15)); // Slower decay
      |                                                                       ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3491:5: note: candidate: 'template<class _Tp, class _Compare> constexpr _Tp std::max(std::initializer_list<_Tp>, _Compare)'
 3491 |     max(initializer_list<_Tp> __l, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3491:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:940:71: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  940 |                 entropyGrid[i] = max(0, entropyGrid[i] - random(5, 15)); // Slower decay
      |                                                                       ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3485:5: note: candidate: 'template<class _Tp> constexpr _Tp std::max(std::initializer_list<_Tp>)'
 3485 |     max(initializer_list<_Tp> __l)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3485:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:940:71: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  940 |                 entropyGrid[i] = max(0, entropyGrid[i] - random(5, 15)); // Slower decay
      |                                                                       ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:300:5: note: candidate: 'template<class _Tp, class _Compare> constexpr const _Tp& std::max(const _Tp&, const _Tp&, _Compare)'
  300 |     max(const _Tp& __a, const _Tp& __b, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:300:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:940:71: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'float')
  940 |                 entropyGrid[i] = max(0, entropyGrid[i] - random(5, 15)); // Slower decay
      |                                                                       ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:254:5: note: candidate: 'template<class _Tp> constexpr const _Tp& std::max(const _Tp&, const _Tp&)'
  254 |     max(const _Tp& __a, const _Tp& __b)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:254:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:940:71: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'float')
  940 |                 entropyGrid[i] = max(0, entropyGrid[i] - random(5, 15)); // Slower decay
      |                                                                       ^
sketch_feb02b:942:75: error: no matching function for call to 'min(int, float)'
  942 |                 entropyGrid[i] = min(255, entropyGrid[i] + random(50, 100)); // Gradual buildup
      |                                                                           ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3479:5: note: candidate: 'template<class _Tp, class _Compare> constexpr _Tp std::min(std::initializer_list<_Tp>, _Compare)'
 3479 |     min(initializer_list<_Tp> __l, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3479:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:942:75: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  942 |                 entropyGrid[i] = min(255, entropyGrid[i] + random(50, 100)); // Gradual buildup
      |                                                                           ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3473:5: note: candidate: 'template<class _Tp> constexpr _Tp std::min(std::initializer_list<_Tp>)'
 3473 |     min(initializer_list<_Tp> __l)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3473:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:942:75: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  942 |                 entropyGrid[i] = min(255, entropyGrid[i] + random(50, 100)); // Gradual buildup
      |                                                                           ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:278:5: note: candidate: 'template<class _Tp, class _Compare> constexpr const _Tp& std::min(const _Tp&, const _Tp&, _Compare)'
  278 |     min(const _Tp& __a, const _Tp& __b, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:278:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:942:75: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'float')
  942 |                 entropyGrid[i] = min(255, entropyGrid[i] + random(50, 100)); // Gradual buildup
      |                                                                           ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:230:5: note: candidate: 'template<class _Tp> constexpr const _Tp& std::min(const _Tp&, const _Tp&)'
  230 |     min(const _Tp& __a, const _Tp& __b)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:230:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:942:75: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'float')
  942 |                 entropyGrid[i] = min(255, entropyGrid[i] + random(50, 100)); // Gradual buildup
      |                                                                           ^
sketch_feb02b:957:76: error: no matching function for call to 'min(int, float)'
  957 |                 entropyGrid[i] = min(255, entropyGrid[i] + random(100, 200)); // Ramp up smoothly
      |                                                                            ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3479:5: note: candidate: 'template<class _Tp, class _Compare> constexpr _Tp std::min(std::initializer_list<_Tp>, _Compare)'
 3479 |     min(initializer_list<_Tp> __l, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3479:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:957:76: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  957 |                 entropyGrid[i] = min(255, entropyGrid[i] + random(100, 200)); // Ramp up smoothly
      |                                                                            ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:65,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3473:5: note: candidate: 'template<class _Tp> constexpr _Tp std::min(std::initializer_list<_Tp>)'
 3473 |     min(initializer_list<_Tp> __l)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algo.h:3473:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:957:76: note:   mismatched types 'std::initializer_list<_Tp>' and 'int'
  957 |                 entropyGrid[i] = min(255, entropyGrid[i] + random(100, 200)); // Ramp up smoothly
      |                                                                            ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:278:5: note: candidate: 'template<class _Tp, class _Compare> constexpr const _Tp& std::min(const _Tp&, const _Tp&, _Compare)'
  278 |     min(const _Tp& __a, const _Tp& __b, _Compare __comp)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:278:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:957:76: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'float')
  957 |                 entropyGrid[i] = min(255, entropyGrid[i] + random(100, 200)); // Ramp up smoothly
      |                                                                            ^
In file included from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/array:40,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/tuple:39,
                 from /home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/functional:54,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h:31,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFiSTA.h:28,
                 from /home/zitrone/.arduino15/packages/esp8266/hardware/esp8266/3.1.2/libraries/ESP8266WiFi/src/ESP8266WiFi.h:34,
                 from /home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:1:
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:230:5: note: candidate: 'template<class _Tp> constexpr const _Tp& std::min(const _Tp&, const _Tp&)'
  230 |     min(const _Tp& __a, const _Tp& __b)
      |     ^~~
/home/zitrone/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.1.0-gcc10.3-e5f9fec/xtensa-lx106-elf/include/c++/10.3.0/bits/stl_algobase.h:230:5: note:   template argument deduction/substitution failed:
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino:957:76: note:   deduced conflicting types for parameter 'const _Tp' ('int' and 'float')
  957 |                 entropyGrid[i] = min(255, entropyGrid[i] + random(100, 200)); // Ramp up smoothly
      |                                                                            ^
exit status 1
no matching function for call to 'max(int, double)'


This report would have more information with
"Show verbose output during compilation"
option enabled in File -> Preferences.
