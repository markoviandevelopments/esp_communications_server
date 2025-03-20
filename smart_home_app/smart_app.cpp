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

        // Fade transition for new command
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

        if (payload.startsWith("COLORPULSE"))
        {
            int colonIndex = payload.indexOf(':');
            if (colonIndex != -1)
                pulseColor = parseColor(payload.substring(colonIndex + 1));
            currentEffect = DONT_PANIC;
        }
        else if (payload == "BABELFISH")
            currentEffect = BABEL_FISH;
        else if (payload == "PANGLACTIC")
            currentEffect = PAN_GALACTIC;
        else if (payload == "VOGONPOETRY")
            currentEffect = VOGON_POETRY;
        else if (payload == "BEESWARM")
            currentEffect = BEE_SWARM;
        else if (payload == "LOVEPRESTON")
            currentEffect = LOVE_PRESTON;
        else if (payload == "ZAPHODWAVE")
            currentEffect = ZAPHOD_WAVE;
        else if (payload == "TRILLIANSPARK")
            currentEffect = TRILLIAN_SPARK;
        else if (payload == "HEARTOFGOLD")
            currentEffect = HEART_OF_GOLD_PULSE;
        else if (payload == "SLARTIDATA")
            currentEffect = SLARTI_DATA_STREAM;
        else if (payload == "IMPROBABILITY")
            currentEffect = IMPROBABILITY_DRIVE;
        else if (payload.startsWith("CHAOS:"))
        {
            chaosFactor = payload.substring(6).toInt();
            chaosFactor = constrain(chaosFactor, 0, 100);
            Serial.println("Chaos Factor set to: " + String(chaosFactor));
        }
        else if (payload == "RESET")
        {
            strip.clear();
            strip.show();
            currentEffect = IDLE;
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
    if (millis() - lastUpdate >= 80)
    {
        pulseBrightness += pulseDirection * 10;
        if (pulseBrightness >= 255 || pulseBrightness <= 20)
            pulseDirection *= -1;

        // Use pulseColor as the background, scaled by brightness
        uint32_t bgColor = dimColor(pulseColor, pulseBrightness);

        for (int i = 0; i < NUM_LEDS; i++)
            strip.setPixelColor(i, bgColor);

        // "DON'T PANIC" scrolling text with glitches
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
                strip.setPixelColor(i, strip.Color(0, 255, 0)); // Keep text green

        // Towel wave
        if (random(100) < chaosFactor / 20)
        {
            static int towelPos = 0;
            strip.setPixelColor(towelPos, strip.Color(255, 255, 150));
            towelPos = (towelPos + 5) % NUM_LEDS;
        }

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
        strip.clear();

        // Scrolling text: "I LOVE YOU PRESTON" in bright white
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

        // Electric fire background
        for (int i = 0; i < NUM_LEDS; i++)
        {
            if (!is_on[i])
            {
                uint8_t flicker = random(100, 255); // Flickering intensity
                uint8_t r = flicker;                // Red-hot base
                uint8_t g = flicker * 0.5;          // Orange-yellow tint
                uint8_t b = flicker * 0.2;          // Slight blue for electric zap
                strip.setPixelColor(i, strip.Color(r, g, b));
            }
            else
            {
                strip.setPixelColor(i, strip.Color(255, 255, 255)); // White text
            }
        }

        // Sparkles stay
        if (random(100) < chaosFactor / 5)
        {
            int sparkPos = random(NUM_LEDS);
            strip.setPixelColor(sparkPos, strip.Color(255, 255, random(150, 255))); // Bright sparkle
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
    static float wavePos = 0.0;
    const float waveSpeed = 0.1;  // Speed of wave movement
    const float waveWidth = 20.0; // Width of the wave peak

    if (millis() - lastUpdate >= 20) // Update every 20ms for smooth flow
    {
        strip.clear(); // Clear previous frame

        for (int i = 0; i < NUM_LEDS; i++)
        {
            // Calculate wave intensity with a sine function for smoothness
            float distance = fabs(i - wavePos);
            float intensity = sin((distance / waveWidth) * PI) * 255;
            if (intensity < 0)
                intensity = 0; // Clamp to positive values

            // Two-tone color shift for Zaphod’s dual-head vibe
            uint8_t r = (i % 2 == 0) ? intensity : intensity * 0.5;
            uint8_t g = intensity * 0.8;
            uint8_t b = (i % 2 == 0) ? intensity * 0.5 : intensity;
            strip.setPixelColor(i, strip.Color(r, g, b));
        }

        // Random "Zaphod flair" burst
        if (random(100) < chaosFactor / 10)
        {
            int burstPos = random(NUM_LEDS);
            strip.setPixelColor(burstPos, strip.Color(255, 255, 0)); // Bright yellow flash
        }

        strip.show();
        wavePos += waveSpeed;
        if (wavePos >= NUM_LEDS + waveWidth)
            wavePos = -waveWidth; // Loop seamlessly
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
    static float chunkPos = 0.0;
    static float chunkSpeed = 8.0;
    static uint8_t hue = 0;
    const float friction = 0.92;
    static int direction = 1;
    const int chunkSize = 10;

    if (millis() - lastUpdate >= 10)
    {
        strip.clear();

        chunkPos += chunkSpeed * direction;
        chunkSpeed *= friction;

        // Bounce with color change
        if (chunkPos + chunkSize >= NUM_LEDS || chunkPos <= 0)
        {
            direction *= -1;
            chunkSpeed = max(chunkSpeed, 6.0); // Minimum speed after bounce
            hue = (hue + 85) % 255;            // Color shift
        }

        // Draw 10-LED chunk
        int startPos = (int)chunkPos;
        for (int i = max(0, startPos); i < min(NUM_LEDS, startPos + chunkSize); i++)
        {
            int brightness = 255 - ((i - startPos) * 255 / chunkSize); // Fade to tail
            strip.setPixelColor(i, strip.ColorHSV(hue * 257, 255, brightness));
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

void heartOfGoldPulse()
{
    static unsigned long lastUpdate = 0;
    static int flashPhase = 0;

    if (millis() - lastUpdate >= 20)
    {
        strip.clear();

        // Flashy chaos
        for (int i = 0; i < NUM_LEDS; i++)
        {
            if (random(100) < 50 + sin(flashPhase * 0.1) * 40) // Wave-driven probability
                strip.setPixelColor(i, strip.ColorHSV(random(65535), 255, 255));
        }

        strip.show();
        flashPhase++;
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
    static int streamPos = NUM_LEDS + 20; // Start off-screen right
    static int streamLength = 20;
    const int streamSpeed = -5;   // Faster, reversed
    static int beePos = NUM_LEDS; // Bee scooter position

    if (millis() - lastUpdate >= 10) // Faster updates
    {
        strip.clear();

        // Data stream (right to left)
        for (int i = max(0, streamPos); i < min(NUM_LEDS, streamPos + streamLength); i++)
        {
            int brightness = 255 - ((streamPos + streamLength - i) * 255 / streamLength); // Fade from head to tail
            if (brightness > 0)
            {
                uint8_t r = (i % 4 == 0) ? random(50, 100) : 0;
                uint8_t g = brightness * 0.8;
                uint8_t b = brightness;
                strip.setPixelColor(i, strip.Color(r, g, b));
            }
        }

        // Bee scooter (yellow, chasing the stream)
        beePos += streamSpeed / 2; // Slower than stream
        if (beePos < 0)
            beePos = NUM_LEDS;                                 // Reset to right
        strip.setPixelColor(beePos, strip.Color(255, 255, 0)); // Bright yellow bee

        // Fjord sparkle
        if (random(100) < chaosFactor / 5)
        {
            int sparklePos = random(NUM_LEDS);
            strip.setPixelColor(sparklePos, strip.Color(255, 255, 255));
        }

        strip.show();
        streamPos += streamSpeed;
        if (streamPos <= -streamLength)
        {
            streamPos = NUM_LEDS + 20;
            streamLength = random(15, 30);
        }

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
    static uint8_t entropyGrid[NUM_LEDS] = {0};
    static uint16_t hueShift = 0;

    if (millis() - lastUpdate >= 30) // Faster for more energy
    {
        // Entropy sim with more action
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int prob = random(100);
            if (entropyGrid[i] > 0 && prob < 30) // Faster decay
                entropyGrid[i] = max(0, entropyGrid[i] - 30);
            else if (prob < chaosFactor) // Higher flare chance
                entropyGrid[i] = random(150, 255);

            if (entropyGrid[i] > 0)
            {
                uint32_t color = strip.ColorHSV(hueShift + (i * 200), 255, entropyGrid[i]);
                strip.setPixelColor(i, color);
            }
            else
            {
                strip.setPixelColor(i, strip.Color(5, 5, 5)); // Dimmer void
            }
        }

        // Wild improbability bursts
        if (random(100) < chaosFactor / 5)
        {
            int burstStart = random(NUM_LEDS - 20);
            for (int i = burstStart; i < burstStart + 20 && i < NUM_LEDS; i++)
            {
                strip.setPixelColor(i, strip.ColorHSV(random(65535), 255, 255)); // Random hue burst
                entropyGrid[i] = 255;
            }
        }

        strip.show();
        hueShift = (hueShift + random(50, 150)) % 65535; // Wild hue shifts
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