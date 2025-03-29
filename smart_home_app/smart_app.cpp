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
    HEART_OF_GOLD_EXPLOSION,
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
int chaosFactor = 50;
int bee_x = 0, bee_y = 0;

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
    case HEART_OF_GOLD_EXPLOSION:
        heartOfGoldExplosion();
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
        else if (payload == "HEARTOFGOLDEXPLOSION" || (isNumeric && modeNum == 10)) // Updated command
            currentEffect = HEART_OF_GOLD_EXPLOSION;
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
            float dist1 = fabs(i - wavePos1);
            float dist2 = fabs(i - wavePos2);
            float intensity1 = max(0.0f, cos((dist1 / waveWidth) * M_PI) * 255.0f); // Smooth cosine wave
            float intensity2 = max(0.0f, cos((dist2 / waveWidth) * M_PI) * 150.0f); // Secondary, dimmer

            uint8_t r = (uint8_t)(intensity1 * (i % 2 == 0) + intensity2 * 0.5f);
            uint8_t g = (uint8_t)(intensity1 * 0.7f);
            uint8_t b = (uint8_t)(intensity2 * (i % 2 == 1) + intensity1 * 0.5f);
            strip.setPixelColor(i, strip.Color(r, g, b));

            if (dist1 < waveWidth * 1.5 || dist2 < waveWidth * 1.5)
            {
                uint8_t glow = (uint8_t)max(0.0f, 50.0f - (dist1 + dist2) * 2.0f);
                strip.setPixelColor(i, strip.Color(r + glow, g + glow * 0.8f, b + glow));
            }
        }

        if (random(100) < chaosFactor / 8)
        {
            int burstPos = random(NUM_LEDS);
            strip.setPixelColor(burstPos, strip.ColorHSV(random(65535), 255, 255));
        }

        strip.show();
        wavePos1 += waveSpeed;
        wavePos2 += waveSpeed * 1.2;
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

void heartOfGoldExplosion()
{
    static unsigned long lastUpdate = 0;
    static struct Particle
    {
        float pos;
        float speed;
        uint8_t bright;
        bool active;
    } particles[30];
    static int particleCount = 0;
    static float explosionCenter = NUM_LEDS / 2;
    static bool exploding = false;
    static unsigned long phaseStart = 0;

    if (millis() - lastUpdate >= 15)
    {
        strip.clear();

        if (!exploding && particleCount == 0)
        {
            exploding = true;
            phaseStart = millis();
            explosionCenter = random(NUM_LEDS / 4, NUM_LEDS * 3 / 4);
            particleCount = 20 + random(0, 11);
            for (int i = 0; i < particleCount; i++)
            {
                float angle = random(0, 628) / 100.0f;
                float speed = random(5, 10);
                particles[i] = {explosionCenter, cos(angle) * speed, (uint8_t)random(200, 255), true};
            }
        }

        for (int i = 0; i < particleCount; i++)
        {
            if (!particles[i].active)
                continue;

            if (exploding)
            {
                particles[i].pos += particles[i].speed;
                particles[i].speed *= 0.92f;
                particles[i].bright = (uint8_t)max(0, (int)particles[i].bright - random(5, 15));
            }
            else
            {
                float distToCenter = explosionCenter - particles[i].pos;
                particles[i].speed += distToCenter * 0.005f;
                particles[i].speed *= 0.95f;
                particles[i].pos += particles[i].speed;
                particles[i].bright = (uint8_t)max(0, (int)particles[i].bright - random(3, 10));
            }

            int pos = (int)particles[i].pos;
            if (pos >= 0 && pos < NUM_LEDS)
            {
                uint8_t r = particles[i].bright;
                uint8_t g = (uint8_t)(particles[i].bright * 0.8f);
                uint8_t b = (uint8_t)(particles[i].bright * 0.2f);
                strip.setPixelColor(pos, strip.Color(r, g, b));
                if (pos - 1 >= 0)
                    strip.setPixelColor(pos - 1, strip.Color(r / 4, g / 4, b / 4));
                if (pos + 1 < NUM_LEDS)
                    strip.setPixelColor(pos + 1, strip.Color(r / 4, g / 4, b / 4));
            }

            if (particles[i].bright == 0 || particles[i].pos < 0 || particles[i].pos >= NUM_LEDS)
            {
                particles[i].active = false;
                particleCount--;
                particles[i] = particles[particleCount];
                i--;
            }
        }

        if (exploding && millis() - phaseStart > 1000)
        {
            exploding = false;
            phaseStart = millis();
        }

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
            if (allConverged || millis() - phaseStart > 2000)
            {
                particleCount = 0;
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
    static int streamOffset = 0;
    const int streamSpeed = 5;
    const int segmentSize = 10;

    if (millis() - lastUpdate >= 15)
    {
        strip.clear();

        for (int i = 0; i < NUM_LEDS; i++)
        {
            int pos = (i + streamOffset) % (segmentSize * 2);
            bool isData = pos < segmentSize;
            uint8_t brightness = isData ? random(100, 255) : random(20, 50);
            uint8_t g = (uint8_t)(brightness * 0.9f);
            uint8_t b = brightness;
            uint8_t r = (random(100) < 10) ? brightness : 0;
            strip.setPixelColor(i, strip.Color(r, g, b));
        }

        static int beePos = NUM_LEDS;
        beePos -= streamSpeed / 2;
        if (beePos < 0)
            beePos = NUM_LEDS;
        strip.setPixelColor(beePos, strip.Color(255, 255, 0));

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
    static float entropyGrid[NUM_LEDS] = {0};
    static uint16_t hueShift = 0;

    if (millis() - lastUpdate >= 20)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int prob = random(100);
            if (entropyGrid[i] > 0)
                entropyGrid[i] = max(0.0f, entropyGrid[i] - (float)random(5, 15));
            if (prob < chaosFactor / 2)
                entropyGrid[i] = min(255.0f, entropyGrid[i] + (float)random(50, 100));

            uint8_t brightness = (uint8_t)entropyGrid[i];
            strip.setPixelColor(i, strip.ColorHSV(hueShift + (i * 200), 255, brightness));
            if (brightness < 10)
                strip.setPixelColor(i, strip.Color(5, 5, 5));
        }

        if (random(100) < chaosFactor / 4)
        {
            int burstStart = random(NUM_LEDS - 20);
            for (int i = burstStart; i < burstStart + 20 && i < NUM_LEDS; i++)
            {
                entropyGrid[i] = min(255.0f, entropyGrid[i] + (float)random(100, 200));
            }
        }

        strip.show();
        hueShift = (hueShift + random(20, 60)) % 65535;
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
