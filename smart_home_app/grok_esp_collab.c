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