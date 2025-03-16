#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#define NUM_LEDS 300
#define LED_PIN 2
#define MAX_STARS 4

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

const char *ssid = "Brubaker Wifi";
const char *password = "Pre$ton01";
const char *serverUrl = "http://10.1.10.79:4999/device/001";

// Enum EffectState unchanged
EffectState currentEffect = IDLE;

volatile bool newCommandReceived = false;
unsigned long lastServerCheck = 0;
const int serverCheckInterval = 300;

// Matrix text definitions unchanged
// [Letter arrays remain as in your original code]

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
    static int hue = 0; // For color drift
    if (millis() - lastUpdate >= 80)
    {
        pulseBrightness += pulseDirection * 10;
        if (pulseBrightness >= 255 || pulseBrightness <= 20)
            pulseDirection *= -1;

        hue = (hue + 1) % 65535; // Subtle color drift
        uint32_t bgColor = strip.ColorHSV(hue, 255, pulseBrightness);

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
                strip.setPixelColor(i, strip.Color(0, 255, 0));

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

void rainbowGlide(int wait)
{
    static int offset = 0, pulsePos = 0, pulsePos2 = NUM_LEDS / 2;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int hue = ((i + offset) % NUM_LEDS) * (65535 / NUM_LEDS);
        int brightness = 255;
        if (abs(i - pulsePos) < 10)
            brightness = 255 - abs(i - pulsePos) * 20;
        if (abs(i - pulsePos2) < 8)
            brightness = max(brightness, 255 - abs(i - pulsePos2) * 25);
        strip.setPixelColor(i, strip.ColorHSV(hue, 255, max(50, brightness)));
    }
    if (random(80 - chaosFactor / 2) == 0)
    { // Hyperspace jump
        setStripColor(strip.Color(255, 255, 255));
        strip.show();
        delay(50);
        offset = random(65535); // New color palette
    }
    if (random(100) < chaosFactor / 10)
    { // Soundwave
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int wave = (sin((i + offset) * 0.1) + 1) * 50;
            strip.setPixelColor(i, strip.ColorHSV(((i + offset) % NUM_LEDS) * (65535 / NUM_LEDS), 255, wave + 100));
        }
        strip.show();
        delay(100);
    }
    strip.show();
    offset = (offset + 1) % NUM_LEDS;
    pulsePos = (pulsePos + 2) % NUM_LEDS;
    pulsePos2 = (pulsePos2 + 3) % NUM_LEDS;
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

void infiniteImprobabilityDrive()
{
    static int cycle = 0, chaosMode = 0;
    static unsigned long lastShift = 0;
    int delayTime = random(20, 80);

    if (millis() - lastShift >= delayTime)
    {
        cycle++;
        if (random(30 - chaosFactor / 5) == 0)
        {
            chaosMode = random(3);
            strip.clear();
        }

        switch (chaosMode)
        {
        case 0:
            for (int i = 0; i < NUM_LEDS; i++)
            {
                int wave = (sin((i + cycle) * 0.05) + 1) * 127;
                strip.setPixelColor(i, strip.ColorHSV((i * 256 + cycle * 150) % 65535, 255, wave));
            }
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

        if (random(200 - chaosFactor) == 0)
        { // "42" in binary
            strip.clear();
            for (int i = 0; i < NUM_LEDS; i += 6)
            {
                strip.setPixelColor(i, strip.Color(42, 42, 42));     // 1
                strip.setPixelColor(i + 1, strip.Color(0, 0, 0));    // 0
                strip.setPixelColor(i + 2, strip.Color(42, 42, 42)); // 1
                strip.setPixelColor(i + 3, strip.Color(0, 0, 0));    // 0
                strip.setPixelColor(i + 4, strip.Color(42, 42, 42)); // 1
                strip.setPixelColor(i + 5, strip.Color(0, 0, 0));    // 0
            }
            strip.show();
            delay(200);
        }
        else if (random(150) < chaosFactor / 5)
        { // Whale cameo
            int pos = random(NUM_LEDS - 10);
            for (int i = pos; i < pos + 10; i++)
                strip.setPixelColor(i, strip.Color(255, 150, 200));
            strip.show();
            delay(100);
        }
        else
        {
            strip.show();
        }

        lastShift = millis();
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
    static int stanzaBreak = 0, verseBuild = 0;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int brightness = (sin((i + j) * 0.05) + 1) * 127;
        strip.setPixelColor(i, strip.Color(0, brightness * 0.6, brightness * 0.8));
        if (random(50) == 0)
            strip.setPixelColor(i, strip.Color(255, 0, 0));
        if (random(100) < chaosFactor / 10)
            strip.setPixelColor(i, strip.Color(150, 0, 150)); // Audience groans
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
        verseBuild = 0;
    }
    else if (random(100 - chaosFactor / 2) == 0)
    {
        stanzaBreak = 2;
    }
    else if (random(80) < chaosFactor / 5)
    { // Stammer
        setStripColor(strip.Color(255, 0, 0));
        strip.show();
        delay(20);
    }
    verseBuild = min(255, verseBuild + 5);
    for (int i = 0; i < NUM_LEDS; i++)
        strip.setPixelColor(i, dimColor(strip.getPixelColor(i), verseBuild));
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
    struct Star
    {
        int pos;
        int dir;
        uint8_t speed;
        uint8_t brightness;
        int length;
        uint8_t sputter;
    };
    static Star stars[MAX_STARS] = {{42, 1, 2, 255, 20, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};
    static uint8_t improbability = 0;

    float engineHum = (sin(millis() * 0.01) + 1) * 20;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        uint32_t c = strip.getPixelColor(i);
        uint8_t r = ((c >> 16) & 0xFF) * 0.75;
        uint8_t g = ((c >> 8) & 0xFF) * 0.75;
        uint8_t b = (c & 0xFF) * 0.75;
        strip.setPixelColor(i, strip.Color(r, g + engineHum, b));
    }

    if (random(60 - chaosFactor / 2) < 2 || improbability > 0)
    {
        improbability = (improbability > 0) ? improbability - 1 : random(10, 20);
        for (int i = 0; i < NUM_LEDS; i += random(8, 15))
        {
            strip.setPixelColor(i, strip.Color(255, random(180, 230), random(improbability * 15)));
        }
        if (improbability == 10 && random(2) == 0)
        {
            for (int s = 0; s < MAX_STARS; s++)
            {
                if (stars[s].speed == 0)
                {
                    stars[s] = {random(NUM_LEDS), random(2) ? 1 : -1, random(1, 5), 255, random(15, 25), 0};
                    break;
                }
            }
        }
    }

    for (int s = 0; s < MAX_STARS; s++)
    {
        if (stars[s].speed == 0)
            continue;

        for (int i = 0; i < stars[s].length; i++)
        {
            int trailPos = stars[s].pos - (i * stars[s].dir);
            if (trailPos >= 0 && trailPos < NUM_LEDS)
            {
                uint8_t brightness = stars[s].brightness * (stars[s].length - i) / stars[s].length;
                uint8_t gold = random(180, 215);
                if (i == 0)
                {
                    strip.setPixelColor(trailPos, strip.Color(255, gold, brightness / 3));
                }
                else if (stars[s].sputter > 0 && random(3) == 0)
                {
                    continue;
                }
                else
                {
                    uint8_t amber = (gold * (stars[s].length - i)) / stars[s].length;
                    strip.setPixelColor(trailPos, improbability > 0 && random(2) ? strip.Color(0, 0, brightness) : strip.Color(brightness, amber, 0));
                }
            }
        }

        if (random(10) == 0)
        {
            int cracklePos = stars[s].pos + random(-5, 5);
            if (cracklePos >= 0 && cracklePos < NUM_LEDS)
            {
                strip.setPixelColor(cracklePos, strip.Color(255, random(200, 255), random(80)));
            }
        }

        stars[s].pos += (stars[s].dir * stars[s].speed);
        stars[s].brightness = random(200, 255);
        stars[s].sputter = max(0, stars[s].sputter - 1);

        if (stars[s].pos >= NUM_LEDS || stars[s].pos <= 0)
        {
            stars[s].dir = -stars[s].dir;
            stars[s].speed = random(1, 5);
            stars[s].length = random(15, 25);
            stars[s].pos = (stars[s].pos <= 0) ? 0 : NUM_LEDS - 1;
            stars[s].sputter = random(5, 10);
            if (random(4) == 0)
            {
                stars[s].pos = random(NUM_LEDS);
                stars[s].dir = random(2) ? 1 : -1;
                improbability = random(12);
            }
        }
        if (random(20) == 0)
            stars[s].sputter = random(5, 15);
        if (random(100) == 0)
        {
            stars[s].speed = 0;
            if (random(2) == 0)
                stars[s] = {random(NUM_LEDS), random(2) ? 1 : -1, random(1, 5), 255, random(15, 25), 0};
        }
    }

    strip.show();
    delay(random(8, 20));
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void matrixBee()
{
    struct Bee
    {
        int x;
        int y;
        uint32_t color;
    };
    static Bee bees[5] = {{0, 0, strip.Color(255, 255, 0)}, {10, 2, strip.Color(255, 200, 0)}, {20, 4, strip.Color(255, 255, 100)}, {0, 0, 0}, {0, 0, 0}};
    static int trails[NUM_LEDS] = {0};

    for (int i = 0; i < NUM_LEDS; i++)
    {
        trails[i] = max(0, trails[i] - 20);
        strip.setPixelColor(i, strip.Color(0, trails[i] / 2, 0));
    }

    for (int b = 0; b < 5; b++)
    {
        if (bees[b].color == 0)
            continue;
        int rr = random(4);
        if (rr == 0 && bees[b].x < 31)
            bees[b].x++;
        else if (rr == 1 && bees[b].x > 0)
            bees[b].x--;
        else if (rr == 2 && bees[b].y < 7)
            bees[b].y++;
        else if (rr == 3 && bees[b].y > 0)
            bees[b].y--;

        int bee_index = (bees[b].x % 2 == 0) ? (bees[b].y + bees[b].x * 8) : ((7 - bees[b].y) + bees[b].x * 8);
        if (bee_index >= 0 && bee_index < NUM_LEDS)
        {
            strip.setPixelColor(bee_index, bees[b].color);
            trails[bee_index] = 255;
            if (random(100) < chaosFactor)
            { // Buzz flicker
                strip.setPixelColor(bee_index, strip.Color(0, 0, 0));
                strip.show();
                delay(10);
                strip.setPixelColor(bee_index, bees[b].color);
            }
        }
    }

    strip.show();
    delay(200 - chaosFactor);
    if (newCommandReceived)
    {
        newCommandReceived = false;
        currentEffect = IDLE;
    }
}

void lovePrestonMatrix(uint16_t speed)
{
    int is_on[NUM_LEDS] = {0};
    int total_width = 82;
    float heartbeat = (sin(millis() * 0.05) + 1) * 30;
    static int hue = 0;
    hue = (hue + 1) % 65535;

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
    for (int i = 0; i < NUM_LEDS; i++)
    {
        strip.setPixelColor(i, strip.ColorHSV(hue, 255, is_on[i] ? 255 : heartbeat));
        if (random(100) < chaosFactor / 5)
            strip.setPixelColor(i, strip.Color(255, 255, random(150, 255))); // Sparkle
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
    static int gravityCenter = NUM_LEDS / 2;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        trail[i] = max(0, trail[i] - 20);
        int dist = abs(i - gravityCenter);
        int grav = dist < 20 ? (20 - dist) * 5 : 0;
        strip.setPixelColor(i, strip.Color(trail[i] / 2, trail[i] / 2, trail[i] + grav));
    }
    if (random(10) == 0)
    {
        int pos = random(NUM_LEDS);
        trail[pos] = 255;
    }
    if (random(100 - chaosFactor / 2) == 0)
    { // Nebula bloom
        int center = random(NUM_LEDS);
        for (int i = max(0, center - 20); i < min(NUM_LEDS, center + 20); i++)
        {
            int dist = abs(i - center);
            trail[i] = max(trail[i], 255 - dist * 10);
            strip.setPixelColor(i, strip.ColorHSV(40000 - dist * 500, 255, trail[i])); // Purple to blue
        }
    }
    if (random(150) < chaosFactor / 5)
    { // Star birth
        int pos = random(NUM_LEDS);
        for (int i = 0; i < 5; i++)
        {
            strip.setPixelColor(pos, strip.Color(255, 255, 255));
            strip.show();
            delay(20);
            strip.setPixelColor(pos, strip.Color(random(256), random(256), random(256)));
        }
        trail[pos] = 255;
    }
    if (random(200) < chaosFactor)
        gravityCenter = random(NUM_LEDS);
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
    static int phase = 0, shipPos = NUM_LEDS;
    static int beamPos = NUM_LEDS;
    if (phase < 30)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int dist = abs(i - NUM_LEDS / 2);
            int swirl = (sin((i + phase) * 0.2) + 1) * 50;
            if (dist < phase * 2)
                strip.setPixelColor(i, strip.Color(0, 100 + swirl, 200 - swirl));
            else
                strip.setPixelColor(i, strip.Color(0, 0, 0));
            if (random(100) < chaosFactor / 5)
                strip.setPixelColor(i, strip.Color(0, random(150, 255), random(150, 255))); // Panic
        }
    }
    else if (phase < 50)
    {
        beamPos -= 5;
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int dist = abs(i - NUM_LEDS / 2);
            if (dist < 60)
                strip.setPixelColor(i, strip.Color(0, 150, 200));
            if (abs(i - shipPos) < 10)
                strip.setPixelColor(i, strip.Color(255, 255, 0));
            if (abs(i - beamPos) < 5)
                strip.setPixelColor(i, strip.Color(0, 255, 0)); // Vogon beam
        }
        shipPos -= 5;
    }
    else
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int dist = abs(i - NUM_LEDS / 2);
            if (dist < (phase - 50) * 3)
                strip.setPixelColor(i, strip.Color(255, 165 - (phase - 50) * 5, 0));
            else if (random(20 + chaosFactor / 2) == 0 && phase < 70)
                strip.setPixelColor(i, strip.Color(255, random(100, 150), 0));
            else
                strip.setPixelColor(i, strip.Color(0, 0, 0));
        }
        if (phase > 80)
            phase = 0;
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
    static int robotPos[5] = {0, 60, 120, 180, 240}, phase = 0;
    for (int i = 0; i < NUM_LEDS; i++)
        strip.setPixelColor(i, strip.Color(0, 0, 0));

    if (phase < 50)
    {
        for (int r = 0; r < 5; r++)
        {
            robotPos[r] += 2;
            if (robotPos[r] >= NUM_LEDS)
                robotPos[r] = 0;
            for (int i = max(0, robotPos[r] - 5); i < min(NUM_LEDS, robotPos[r] + 5); i++)
            {
                strip.setPixelColor(i, strip.Color(255, 255, 255));
                if (random(100) < chaosFactor / 5)
                    strip.setPixelColor(i, strip.Color(255, 150, 0)); // Damage
            }
            if (random(20) == 0)
            {
                int burst = random(NUM_LEDS);
                for (int j = max(0, burst - 10); j < min(NUM_LEDS, burst + 10); j++)
                    strip.setPixelColor(j, strip.Color(255, 0, 0));
            }
            if (random(50) < chaosFactor / 2)
            { // Laser
                int laserPos = robotPos[r] + random(-20, 20);
                for (int j = max(0, laserPos - 5); j < min(NUM_LEDS, laserPos + 5); j++)
                    strip.setPixelColor(j, strip.Color(0, 0, 255));
            }
        }
    }
    else
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            int dist = abs(i - NUM_LEDS / 2);
            if (dist < (phase - 50) * 5)
                strip.setPixelColor(i, strip.Color(255, 255, 255));
            else if (dist < (phase - 40) * 5)
                strip.setPixelColor(i, strip.Color(200, 200, 200)); // Echo
            else
                strip.setPixelColor(i, strip.Color(0, 0, 0));
        }
        if (phase > 70)
            phase = 0;
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
    float speedMod = (finale > 0) ? 0.5 : (random(100) < chaosFactor / 5 ? random(5, 15) / 10.0 : 1.0);
    for (int i = 0; i < NUM_LEDS; i++)
    {
        int brightness = (sin((i + swirl) * 0.1) + 1) * 127;
        strip.setPixelColor(i, strip.Color(brightness * 0.8, 0, brightness));
    }
    if (random(30 - chaosFactor / 5) == 0)
    {
        int center = random(NUM_LEDS);
        for (int j = max(0, center - 10); j < min(NUM_LEDS, center + 10); j++)
            strip.setPixelColor(j, strip.Color(random(256), random(256), random(256)));
        strip.show();
        delay(50);
    }
    if (random(50) < chaosFactor / 2)
    { // Feast dish
        int pos = random(NUM_LEDS);
        strip.setPixelColor(pos, strip.Color(random(3) == 0 ? 255 : 0, random(3) == 1 ? 255 : 0, random(3) == 2 ? 255 : 0));
    }
    if (finale > 0)
    {
        for (int i = 0; i < NUM_LEDS; i++)
            strip.setPixelColor(i, strip.Color(255, 255, 255));
        strip.show();
        delay(200);
        finale--;
        if (finale == 0)
            swirl = 0;
    }
    else if (random(100 - chaosFactor / 2) == 0)
        finale = 3;
    strip.show();
    swirl += 0.2 * speedMod;
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

Arduino: 1.8.19 (Linux), Board: "Generic ESP8266 Module, 80 MHz, Flash, Disabled (new aborts on oom), Disabled, All SSL ciphers (most compatible), 32KB cache + 32KB IRAM (balanced), Use pgm_read macros for IRAM/PROGMEM, dtr (aka nodemcu), 26 MHz, 40MHz, DOUT (compatible), 1MB (FS:64KB OTA:~470KB), 2, nonos-sdk 2.2.1+100 (190703), v2 Lower Memory, Disabled, None, Only Sketch, 115200"

sketch_feb02b:17:1: error: 'EffectState' does not name a type
   17 | EffectState currentEffect = IDLE;
      | ^~~~~~~~~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void loop()':
sketch_feb02b:54:13: error: 'currentEffect' was not declared in this scope
   54 |     switch (currentEffect)
      |             ^~~~~~~~~~~~~
sketch_feb02b:56:10: error: 'DONT_PANIC' was not declared in this scope
   56 |     case DONT_PANIC:
      |          ^~~~~~~~~~
sketch_feb02b:59:10: error: 'RAINBOW' was not declared in this scope
   59 |     case RAINBOW:
      |          ^~~~~~~
sketch_feb02b:62:10: error: 'BABEL_FISH' was not declared in this scope
   62 |     case BABEL_FISH:
      |          ^~~~~~~~~~
sketch_feb02b:65:10: error: 'PAN_GALACTIC' was not declared in this scope
   65 |     case PAN_GALACTIC:
      |          ^~~~~~~~~~~~
sketch_feb02b:68:10: error: 'IMPROBABILITY' was not declared in this scope
   68 |     case IMPROBABILITY:
      |          ^~~~~~~~~~~~~
sketch_feb02b:71:10: error: 'VOGON_POETRY' was not declared in this scope
   71 |     case VOGON_POETRY:
      |          ^~~~~~~~~~~~
sketch_feb02b:74:10: error: 'GOLD_TRAIL' was not declared in this scope
   74 |     case GOLD_TRAIL:
      |          ^~~~~~~~~~
sketch_feb02b:77:10: error: 'BEE_SWARM' was not declared in this scope
   77 |     case BEE_SWARM:
      |          ^~~~~~~~~
sketch_feb02b:80:10: error: 'LOVE_PRESTON' was not declared in this scope
   80 |     case LOVE_PRESTON:
      |          ^~~~~~~~~~~~
sketch_feb02b:83:10: error: 'COSMIC_DUST' was not declared in this scope
   83 |     case COSMIC_DUST:
      |          ^~~~~~~~~~~
sketch_feb02b:86:10: error: 'EARTH_DEMOLITION' was not declared in this scope
   86 |     case EARTH_DEMOLITION:
      |          ^~~~~~~~~~~~~~~~
sketch_feb02b:89:10: error: 'KRICKET_WARS' was not declared in this scope
   89 |     case KRICKET_WARS:
      |          ^~~~~~~~~~~~
sketch_feb02b:92:10: error: 'MILLIWAYS' was not declared in this scope
   92 |     case MILLIWAYS:
      |          ^~~~~~~~~
sketch_feb02b:95:10: error: 'IDLE' was not declared in this scope
   95 |     case IDLE:
      |          ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void checkServer()':
sketch_feb02b:128:13: error: 'currentEffect' was not declared in this scope
  128 |         if (currentEffect != IDLE)
      |             ^~~~~~~~~~~~~
sketch_feb02b:128:30: error: 'IDLE' was not declared in this scope
  128 |         if (currentEffect != IDLE)
      |                              ^~~~
sketch_feb02b:146:13: error: 'currentEffect' was not declared in this scope
  146 |             currentEffect = DONT_PANIC;
      |             ^~~~~~~~~~~~~
sketch_feb02b:146:29: error: 'DONT_PANIC' was not declared in this scope
  146 |             currentEffect = DONT_PANIC;
      |                             ^~~~~~~~~~
sketch_feb02b:149:13: error: 'currentEffect' was not declared in this scope
  149 |             currentEffect = RAINBOW;
      |             ^~~~~~~~~~~~~
sketch_feb02b:149:29: error: 'RAINBOW' was not declared in this scope
  149 |             currentEffect = RAINBOW;
      |                             ^~~~~~~
sketch_feb02b:151:13: error: 'currentEffect' was not declared in this scope
  151 |             currentEffect = BABEL_FISH;
      |             ^~~~~~~~~~~~~
sketch_feb02b:151:29: error: 'BABEL_FISH' was not declared in this scope
  151 |             currentEffect = BABEL_FISH;
      |                             ^~~~~~~~~~
sketch_feb02b:153:13: error: 'currentEffect' was not declared in this scope
  153 |             currentEffect = PAN_GALACTIC;
      |             ^~~~~~~~~~~~~
sketch_feb02b:153:29: error: 'PAN_GALACTIC' was not declared in this scope
  153 |             currentEffect = PAN_GALACTIC;
      |                             ^~~~~~~~~~~~
sketch_feb02b:155:13: error: 'currentEffect' was not declared in this scope
  155 |             currentEffect = IMPROBABILITY;
      |             ^~~~~~~~~~~~~
sketch_feb02b:155:29: error: 'IMPROBABILITY' was not declared in this scope
  155 |             currentEffect = IMPROBABILITY;
      |                             ^~~~~~~~~~~~~
sketch_feb02b:157:13: error: 'currentEffect' was not declared in this scope
  157 |             currentEffect = VOGON_POETRY;
      |             ^~~~~~~~~~~~~
sketch_feb02b:157:29: error: 'VOGON_POETRY' was not declared in this scope
  157 |             currentEffect = VOGON_POETRY;
      |                             ^~~~~~~~~~~~
sketch_feb02b:159:13: error: 'currentEffect' was not declared in this scope
  159 |             currentEffect = GOLD_TRAIL;
      |             ^~~~~~~~~~~~~
sketch_feb02b:159:29: error: 'GOLD_TRAIL' was not declared in this scope
  159 |             currentEffect = GOLD_TRAIL;
      |                             ^~~~~~~~~~
sketch_feb02b:161:13: error: 'currentEffect' was not declared in this scope
  161 |             currentEffect = BEE_SWARM;
      |             ^~~~~~~~~~~~~
sketch_feb02b:161:29: error: 'BEE_SWARM' was not declared in this scope
  161 |             currentEffect = BEE_SWARM;
      |                             ^~~~~~~~~
sketch_feb02b:163:13: error: 'currentEffect' was not declared in this scope
  163 |             currentEffect = LOVE_PRESTON;
      |             ^~~~~~~~~~~~~
sketch_feb02b:163:29: error: 'LOVE_PRESTON' was not declared in this scope
  163 |             currentEffect = LOVE_PRESTON;
      |                             ^~~~~~~~~~~~
sketch_feb02b:165:13: error: 'currentEffect' was not declared in this scope
  165 |             currentEffect = COSMIC_DUST;
      |             ^~~~~~~~~~~~~
sketch_feb02b:165:29: error: 'COSMIC_DUST' was not declared in this scope
  165 |             currentEffect = COSMIC_DUST;
      |                             ^~~~~~~~~~~
sketch_feb02b:167:13: error: 'currentEffect' was not declared in this scope
  167 |             currentEffect = EARTH_DEMOLITION;
      |             ^~~~~~~~~~~~~
sketch_feb02b:167:29: error: 'EARTH_DEMOLITION' was not declared in this scope
  167 |             currentEffect = EARTH_DEMOLITION;
      |                             ^~~~~~~~~~~~~~~~
sketch_feb02b:169:13: error: 'currentEffect' was not declared in this scope
  169 |             currentEffect = KRICKET_WARS;
      |             ^~~~~~~~~~~~~
sketch_feb02b:169:29: error: 'KRICKET_WARS' was not declared in this scope
  169 |             currentEffect = KRICKET_WARS;
      |                             ^~~~~~~~~~~~
sketch_feb02b:171:13: error: 'currentEffect' was not declared in this scope
  171 |             currentEffect = MILLIWAYS;
      |             ^~~~~~~~~~~~~
sketch_feb02b:171:29: error: 'MILLIWAYS' was not declared in this scope
  171 |             currentEffect = MILLIWAYS;
      |                             ^~~~~~~~~
sketch_feb02b:182:13: error: 'currentEffect' was not declared in this scope
  182 |             currentEffect = IDLE;
      |             ^~~~~~~~~~~~~
sketch_feb02b:182:29: error: 'IDLE' was not declared in this scope
  182 |             currentEffect = IDLE;
      |                             ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void dontPanicPulse()':
sketch_feb02b:222:55: error: 'WIDTH_D' was not declared in this scope
  222 |                 if (x - textPos >= 0 && x - textPos < WIDTH_D)
      |                                                       ^~~~~~~
sketch_feb02b:223:46: error: 'letter_D' was not declared in this scope
  223 |                     is_on[index] = glitch ? !letter_D[y][x - textPos] : letter_D[y][x - textPos];
      |                                              ^~~~~~~~
sketch_feb02b:225:46: error: 'letter_O' was not declared in this scope
  225 |                     is_on[index] = glitch ? !letter_O[y][x - textPos - 5] : letter_O[y][x - textPos - 5];
      |                                              ^~~~~~~~
sketch_feb02b:227:46: error: 'letter_N' was not declared in this scope
  227 |                     is_on[index] = glitch ? !letter_N[y][x - textPos - 10] : letter_N[y][x - textPos - 10];
      |                                              ^~~~~~~~
sketch_feb02b:229:46: error: 'letter_T' was not declared in this scope
  229 |                     is_on[index] = glitch ? !letter_T[y][x - textPos - 15] : letter_T[y][x - textPos - 15];
      |                                              ^~~~~~~~
sketch_feb02b:231:46: error: 'letter_P' was not declared in this scope
  231 |                     is_on[index] = glitch ? !letter_P[y][x - textPos - 23] : letter_P[y][x - textPos - 23];
      |                                              ^~~~~~~~
sketch_feb02b:233:46: error: 'letter_A' was not declared in this scope
  233 |                     is_on[index] = glitch ? !letter_A[y][x - textPos - 28] : letter_A[y][x - textPos - 28];
      |                                              ^~~~~~~~
sketch_feb02b:235:46: error: 'letter_N' was not declared in this scope
  235 |                     is_on[index] = glitch ? !letter_N[y][x - textPos - 33] : letter_N[y][x - textPos - 33];
      |                                              ^~~~~~~~
sketch_feb02b:237:46: error: 'letter_I' was not declared in this scope
  237 |                     is_on[index] = glitch ? !letter_I[y][x - textPos - 38] : letter_I[y][x - textPos - 38];
      |                                              ^~~~~~~~
sketch_feb02b:239:46: error: 'letter_C' was not declared in this scope
  239 |                     is_on[index] = glitch ? !letter_C[y][x - textPos - 41] : letter_C[y][x - textPos - 41];
      |                                              ^~~~~~~~
sketch_feb02b:261:9: error: 'currentEffect' was not declared in this scope
  261 |         currentEffect = IDLE;
      |         ^~~~~~~~~~~~~
sketch_feb02b:261:25: error: 'IDLE' was not declared in this scope
  261 |         currentEffect = IDLE;
      |                         ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void rainbowGlide(int)':
sketch_feb02b:303:9: error: 'currentEffect' was not declared in this scope
  303 |         currentEffect = IDLE;
      |         ^~~~~~~~~~~~~
sketch_feb02b:303:25: error: 'IDLE' was not declared in this scope
  303 |         currentEffect = IDLE;
      |                         ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void babelFishSwarm()':
sketch_feb02b:333:9: error: 'currentEffect' was not declared in this scope
  333 |         currentEffect = IDLE;
      |         ^~~~~~~~~~~~~
sketch_feb02b:333:25: error: 'IDLE' was not declared in this scope
  333 |         currentEffect = IDLE;
      |                         ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void panGalacticGargleBlaster()':
sketch_feb02b:366:9: error: 'currentEffect' was not declared in this scope
  366 |         currentEffect = IDLE;
      |         ^~~~~~~~~~~~~
sketch_feb02b:366:25: error: 'IDLE' was not declared in this scope
  366 |         currentEffect = IDLE;
      |                         ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void infiniteImprobabilityDrive()':
sketch_feb02b:443:9: error: 'currentEffect' was not declared in this scope
  443 |         currentEffect = IDLE;
      |         ^~~~~~~~~~~~~
sketch_feb02b:443:25: error: 'IDLE' was not declared in this scope
  443 |         currentEffect = IDLE;
      |                         ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void vogonPoetrySlam()':
sketch_feb02b:494:9: error: 'currentEffect' was not declared in this scope
  494 |         currentEffect = IDLE;
      |         ^~~~~~~~~~~~~
sketch_feb02b:494:25: error: 'IDLE' was not declared in this scope
  494 |         currentEffect = IDLE;
      |                         ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void heartOfGoldTrail()':
sketch_feb02b:612:9: error: 'currentEffect' was not declared in this scope
  612 |         currentEffect = IDLE;
      |         ^~~~~~~~~~~~~
sketch_feb02b:612:25: error: 'IDLE' was not declared in this scope
  612 |         currentEffect = IDLE;
      |                         ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void matrixBee()':
sketch_feb02b:667:9: error: 'currentEffect' was not declared in this scope
  667 |         currentEffect = IDLE;
      |         ^~~~~~~~~~~~~
sketch_feb02b:667:25: error: 'IDLE' was not declared in this scope
  667 |         currentEffect = IDLE;
      |                         ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void lovePrestonMatrix(uint16_t)':
sketch_feb02b:684:63: error: 'WIDTH_I' was not declared in this scope
  684 |             if (x - text_scroll_x >= 0 && x - text_scroll_x < WIDTH_I)
      |                                                               ^~~~~~~
sketch_feb02b:685:32: error: 'letter_I' was not declared in this scope
  685 |                 is_on[index] = letter_I[y][x - text_scroll_x];
      |                                ^~~~~~~~
sketch_feb02b:687:32: error: 'letter_L' was not declared in this scope
  687 |                 is_on[index] = letter_L[y][x - text_scroll_x - 6];
      |                                ^~~~~~~~
sketch_feb02b:689:32: error: 'letter_O' was not declared in this scope
  689 |                 is_on[index] = letter_O[y][x - text_scroll_x - 11];
      |                                ^~~~~~~~
sketch_feb02b:691:32: error: 'letter_V' was not declared in this scope
  691 |                 is_on[index] = letter_V[y][x - text_scroll_x - 16];
      |                                ^~~~~~~~
sketch_feb02b:693:32: error: 'letter_E' was not declared in this scope
  693 |                 is_on[index] = letter_E[y][x - text_scroll_x - 21];
      |                                ^~~~~~~~
sketch_feb02b:695:32: error: 'letter_Y' was not declared in this scope
  695 |                 is_on[index] = letter_Y[y][x - text_scroll_x - 29];
      |                                ^~~~~~~~
sketch_feb02b:697:32: error: 'letter_O' was not declared in this scope
  697 |                 is_on[index] = letter_O[y][x - text_scroll_x - 34];
      |                                ^~~~~~~~
sketch_feb02b:699:32: error: 'letter_U' was not declared in this scope
  699 |                 is_on[index] = letter_U[y][x - text_scroll_x - 39];
      |                                ^~~~~~~~
sketch_feb02b:701:32: error: 'letter_P' was not declared in this scope
  701 |                 is_on[index] = letter_P[y][x - text_scroll_x - 47];
      |                                ^~~~~~~~
sketch_feb02b:703:32: error: 'letter_R' was not declared in this scope
  703 |                 is_on[index] = letter_R[y][x - text_scroll_x - 52];
      |                                ^~~~~~~~
sketch_feb02b:705:32: error: 'letter_E' was not declared in this scope
  705 |                 is_on[index] = letter_E[y][x - text_scroll_x - 57];
      |                                ^~~~~~~~
sketch_feb02b:707:32: error: 'letter_S' was not declared in this scope
  707 |                 is_on[index] = letter_S[y][x - text_scroll_x - 62];
      |                                ^~~~~~~~
sketch_feb02b:709:32: error: 'letter_T' was not declared in this scope
  709 |                 is_on[index] = letter_T[y][x - text_scroll_x - 67];
      |                                ^~~~~~~~
sketch_feb02b:711:32: error: 'letter_O' was not declared in this scope
  711 |                 is_on[index] = letter_O[y][x - text_scroll_x - 72];
      |                                ^~~~~~~~
sketch_feb02b:713:32: error: 'letter_N' was not declared in this scope
  713 |                 is_on[index] = letter_N[y][x - text_scroll_x - 77];
      |                                ^~~~~~~~
sketch_feb02b:732:9: error: 'currentEffect' was not declared in this scope
  732 |         currentEffect = IDLE;
      |         ^~~~~~~~~~~~~
sketch_feb02b:732:25: error: 'IDLE' was not declared in this scope
  732 |         currentEffect = IDLE;
      |                         ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void cosmicDust()':
sketch_feb02b:781:9: error: 'currentEffect' was not declared in this scope
  781 |         currentEffect = IDLE;
      |         ^~~~~~~~~~~~~
sketch_feb02b:781:25: error: 'IDLE' was not declared in this scope
  781 |         currentEffect = IDLE;
      |                         ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void earthDemolition()':
sketch_feb02b:839:9: error: 'currentEffect' was not declared in this scope
  839 |         currentEffect = IDLE;
      |         ^~~~~~~~~~~~~
sketch_feb02b:839:25: error: 'IDLE' was not declared in this scope
  839 |         currentEffect = IDLE;
      |                         ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void kricketWars()':
sketch_feb02b:897:9: error: 'currentEffect' was not declared in this scope
  897 |         currentEffect = IDLE;
      |         ^~~~~~~~~~~~~
sketch_feb02b:897:25: error: 'IDLE' was not declared in this scope
  897 |         currentEffect = IDLE;
      |                         ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void milliways()':
sketch_feb02b:942:9: error: 'currentEffect' was not declared in this scope
  942 |         currentEffect = IDLE;
      |         ^~~~~~~~~~~~~
sketch_feb02b:942:25: error: 'IDLE' was not declared in this scope
  942 |         currentEffect = IDLE;
      |                         ^~~~
/home/zitrone/Arduino/sketch_feb02b/sketch_feb02b.ino: In function 'void setRandomColor()':
sketch_feb02b:966:5: error: 'currentEffect' was not declared in this scope
  966 |     currentEffect = IDLE;
      |     ^~~~~~~~~~~~~
sketch_feb02b:966:21: error: 'IDLE' was not declared in this scope
  966 |     currentEffect = IDLE;
      |                     ^~~~
exit status 1
'EffectState' does not name a type


This report would have more information with
"Show verbose output during compilation"
option enabled in File -> Preferences.
