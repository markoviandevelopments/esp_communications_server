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
    static float wavePos1 = 0.0, wavePos2 = NUM_LEDS / 2.0;
    static float waveSpeed1 = 1.5, waveSpeed2 = -1.8; // Faster, groovier speeds
    static uint16_t baseHue = 0;
    const float waveWidth = 40.0; // Wider waves for impact

    if (millis() - lastUpdate >= 12) // Tight timing for groove
    {
        strip.clear();

        // Two groovy waves (Zaphod’s dual vibe)
        for (int i = 0; i < NUM_LEDS; i++)
        {
            float dist1 = fabs(i - wavePos1);
            float dist2 = fabs(i - wavePos2);
            float groove1 = sin(dist1 / waveWidth * PI + millis() * 0.005) * 255; // Pulsing rhythm
            float groove2 = sin(dist2 / waveWidth * PI + millis() * 0.007) * 255; // Off-beat sync
            float intensity = max(groove1, groove2) + random(-20, 20);            // Overlap with jitter
            if (intensity > 50)
            {
                uint16_t hue = (baseHue + (i * 300) + millis() / 10) % 65535; // Funky, shifting colors
                strip.setPixelColor(i, strip.ColorHSV(hue, 255, min(255, (int)intensity)));
            }
        }

        // Zaphod’s wild interruptions
        if (random(100) < chaosFactor / 2)
        {
            int zapPos = random(NUM_LEDS);
            for (int i = max(0, zapPos - 15); i < min(NUM_LEDS, zapPos + 15); i++)
                strip.setPixelColor(i, strip.ColorHSV(random(32768, 49152), 255, 255)); // Purple-blue zap
        }

        strip.show();
        wavePos1 += waveSpeed1 + sin(millis() * 0.002) * 0.5; // Wavy speed modulation
        wavePos2 += waveSpeed2 + cos(millis() * 0.003) * 0.5;
        baseHue = (baseHue + random(100, 300)) % 65535; // Hue boogie
        if (wavePos1 >= NUM_LEDS + waveWidth)
            wavePos1 = -waveWidth;
        if (wavePos2 <= -waveWidth)
            wavePos2 = NUM_LEDS + waveWidth;
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
    static float puckPos = 0.0;                         // Starting at the left
    static float puckSpeed = 30.0;                      // Increased speed (3x original 10.0)
    static uint32_t puckColor = strip.Color(255, 0, 0); // Start with red, changes on bounce
    static int direction = 1;                           // 1 = right, -1 = left
    static int bounceCount = 0;                         // Track bounces for reset
    const int puckSize = 15;                            // Smaller mass (half of original 30)
    const float friction = 0.995;                       // Same slight friction
    const int trailLength = 20;                         // Keep trail length for smooth gradient

    if (millis() - lastUpdate >= 10) // Keep 10ms update for smoothness
    {
        strip.clear();

        // Update position and apply friction
        puckPos += puckSpeed * direction;
        puckSpeed *= friction;

        // Bounce logic
        if (puckPos + puckSize >= NUM_LEDS) // Hit right edge
        {
            puckPos = NUM_LEDS - puckSize;
            direction = -1;
            bounceCount++;
            puckSpeed = max(puckSpeed, 24.0);                               // Minimum speed (3x original 8.0)
            puckColor = strip.Color(random(256), random(256), random(256)); // New random color
        }
        else if (puckPos <= 0) // Hit left edge
        {
            puckPos = 0;
            direction = 1;
            bounceCount++;
            puckSpeed = max(puckSpeed, 24.0);
            puckColor = strip.Color(random(256), random(256), random(256)); // New random color
        }

        // Draw puck with solid color and intensity-based trail
        int startPos = (int)puckPos;
        for (int i = max(0, startPos - trailLength); i < min(NUM_LEDS, startPos + puckSize + trailLength); i++)
        {
            int distFromCenter = abs(i - (startPos + puckSize / 2));
            int brightness = 255 - (distFromCenter * 255 / (puckSize / 2 + trailLength));
            if (brightness > 0)
            {
                // Apply brightness to the solid puckColor
                uint8_t r = ((puckColor >> 16) & 0xFF) * brightness / 255;
                uint8_t g = ((puckColor >> 8) & 0xFF) * brightness / 255;
                uint8_t b = (puckColor & 0xFF) * brightness / 255;
                strip.setPixelColor(i, strip.Color(r, g, b));
            }
        }

        // Add a chaotic spark with the same color
        if (random(100) < chaosFactor / 3)
        {
            int sparkPos = random(NUM_LEDS);
            strip.setPixelColor(sparkPos, puckColor); // Use puckColor for consistency
        }

        strip.show();
        lastUpdate = millis();

        // Reset if it slows too much or after many bounces
        if (puckSpeed < 6.0 || bounceCount > 15) // Adjusted min speed (3x original 2.0)
        {
            puckPos = NUM_LEDS / 2;         // Restart from center
            puckSpeed = random(24, 36);     // Randomize speed (3x original 8-12)
            direction = random(2) ? 1 : -1; // Random direction
            bounceCount = 0;
            puckColor = strip.Color(random(256), random(256), random(256)); // New random color
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
    static float streamPos[8];    // More streams for density
    static float streamSpeeds[8]; // Individual speeds
    static bool initialized = false;

    if (!initialized) // Initialize stream positions and speeds
    {
        for (int i = 0; i < 8; i++)
        {
            streamPos[i] = random(-50, NUM_LEDS + 50); // Start off-screen
            streamSpeeds[i] = random(80, 200) / 100.0; // 0.8 to 2.0 pixels per frame
        }
        initialized = true;
    }

    if (millis() - lastUpdate >= 15) // Faster update for fluidity
    {
        strip.clear();

        // Flowing streams with Gaussian fade
        for (int s = 0; s < 8; s++)
        {
            streamPos[s] += streamSpeeds[s]; // Move each stream
            if (streamPos[s] >= NUM_LEDS + 50)
                streamPos[s] = -50; // Restart off-screen

            for (int i = max(0, (int)streamPos[s] - 40); i < min(NUM_LEDS, (int)streamPos[s] + 40); i++)
            {
                float dist = fabs(i - streamPos[s]);
                int brightness = 255 * exp(-dist * dist / 200.0); // Gaussian fade (σ^2 = 100)
                if (brightness > 0)
                {
                    uint16_t hue = (millis() / 50 + s * 8192) % 65535; // Subtle hue shift per stream
                    strip.setPixelColor(i, strip.ColorHSV(hue, 255, brightness));
                }
            }
        }

        // Random glitches for data vibe
        if (random(100) < chaosFactor / 2)
        {
            int glitchPos = random(NUM_LEDS);
            strip.setPixelColor(glitchPos, strip.Color(255, 255, 0)); // Yellow glitch
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

void improbabilityDrive()
{
    static unsigned long lastUpdate = 0;
    static float swirlPos = 0.0;
    static uint16_t globalHue = 0;
    static int beatPhase = 0;

    if (millis() - lastUpdate >= 20) // Groovy beat timing
    {
        strip.clear();

        // Swirling improbability with a funky beat
        for (int i = 0; i < NUM_LEDS; i++)
        {
            float swirl = sin((i - swirlPos) * 0.05 + millis() * 0.01) * 255; // Fast swirl
            float beat = sin(beatPhase * 0.1) * 100 + 155;                    // Pulsing brightness
            if (swirl > 50 || random(100) < chaosFactor / 3)
            {
                uint16_t hue = (globalHue + (i * 500) + millis() / 5) % 65535; // Wild hue groove
                strip.setPixelColor(i, strip.ColorHSV(hue, 255, min(255, (int)(swirl * beat / 255))));
            }
        }

        // Improbable bursts of chaos
        if (random(100) < chaosFactor / 2)
        {
            int burstPos = random(NUM_LEDS - 30);
            for (int i = burstPos; i < burstPos + 30; i++)
            {
                uint16_t burstHue = (globalHue + random(-16384, 16384)) % 65535;
                strip.setPixelColor(i, strip.ColorHSV(burstHue, 255, 255));
            }
        }

        strip.show();
        swirlPos += 2.0 + sin(millis() * 0.003) * 1.0;      // Swirling with a funky wobble
        globalHue = (globalHue + random(200, 600)) % 65535; // Hue dance
        beatPhase++;
        if (swirlPos >= NUM_LEDS)
            swirlPos -= NUM_LEDS; // Loop the swirl
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