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

enum EffectState { IDLE, DONT_PANIC, RAINBOW, BABEL_FISH, PAN_GALACTIC, IMPROBABILITY, VOGON_POETRY, GOLD_TRAIL, FIGHT_KAMPF, HEART_WAVE, BEE_MATRIX, TEXT_MATRIX };
EffectState currentEffect = IDLE;

volatile bool newCommandReceived = false;
unsigned long lastServerCheck = 0;
const int serverCheckInterval = 300;

int fight_kampf[NUM_LEDS];
int bee_x = 16, bee_y = 4;
uint32_t pulseColor = strip.Color(0, 0, 0);
int pulseBrightness = 0, pulseDirection = 1;

#define HEIGHT 8
#define WIDTH_P 4
#define WIDTH_PLUS 5
#define WIDTH_W 7

int letter_P[HEIGHT][WIDTH_P] = {{1,1,1,1},{1,0,0,1},{1,0,0,1},{1,1,1,1},{1,0,0,0},{1,0,0,0},{1,0,0,0},{1,0,0,0}};
int letter_PLUS[HEIGHT][WIDTH_PLUS] = {{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{1,1,1,1,1},{0,0,1,0,0},{0,0,1,0,0},{0,0,1,0,0},{0,0,0,0,0}};
int letter_W[HEIGHT][WIDTH_W] = {{1,0,0,0,0,0,1},{1,0,0,0,0,0,1},{1,0,0,1,0,0,1},{1,0,0,1,0,0,1},{1,0,1,0,1,0,1},{1,0,1,0,1,0,1},{1,1,0,0,0,1,1},{1,0,0,0,0,0,1}};

int text_sroll_x_dir = 1, text_sroll_x = 0;

void setup() {
    Serial.begin(115200);
    strip.begin();
    strip.show();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected! IP: " + WiFi.localIP().toString());

    randomSeed(ESP.getCycleCount());
    for (int i = 0; i < NUM_LEDS; i++) fight_kampf[i] = random(0, 2);
}

void loop() {
    if (WiFi.status() == WL_CONNECTED && millis() - lastServerCheck >= serverCheckInterval) {
        checkServer();
        lastServerCheck = millis();
    }

    switch (currentEffect) {
        case DONT_PANIC: dontPanicPulse(); break;
        case RAINBOW: rainbowEffect(50); break;
        case BABEL_FISH: babelFishSwarm(); break;
        case PAN_GALACTIC: panGalacticGargleBlaster(); break;
        case IMPROBABILITY: infiniteImprobabilityDrive(); break;
        case VOGON_POETRY: vogonPoetrySlam(); break;
        case GOLD_TRAIL: heartOfGoldTrail(); break;
        case FIGHT_KAMPF: fight_kampfen(); break;
        case HEART_WAVE: beatingHeartWave(); break;
        case BEE_MATRIX: matrix_bee(); break;
        case TEXT_MATRIX: matrix_text(1000); break;
        case IDLE: delay(10); break;
    }
}

void checkServer() {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, serverUrl);
    http.setTimeout(1000);

    int httpCode = http.GET();
    if (httpCode > 0) {
        String payload = http.getString();
        payload.trim();
        Serial.println("Server command: " + payload);
        newCommandReceived = true;

        if (payload.startsWith("COLORPULSE")) {
            int colonIndex = payload.indexOf(':');
            if (colonIndex != -1) pulseColor = parseColor(payload.substring(colonIndex + 1));
            currentEffect = DONT_PANIC;
        } else if (payload == "FAST") currentEffect = RAINBOW;
        else if (payload == "SLOW") currentEffect = BABEL_FISH;
        else if (payload == "PANGLACTIC") currentEffect = PAN_GALACTIC;
        else if (payload == "IMPROBABILITY") currentEffect = IMPROBABILITY;
        else if (payload == "VOGONPOETRY") currentEffect = VOGON_POETRY;
        else if (payload == "GOLDTRAIL") currentEffect = GOLD_TRAIL;
        else if (payload == "FIGHTKAMPF") currentEffect = FIGHT_KAMPF;
        else if (payload == "HEARTWAVE") currentEffect = HEART_WAVE;
        else if (payload == "BEEMATRIX") currentEffect = BEE_MATRIX;
        else if (payload == "TEXTMATRIX") currentEffect = TEXT_MATRIX;
        else if (payload == "TEXTMATRIXFAST") { currentEffect = TEXT_MATRIX; matrix_text(100); }
        else if (payload == "TEXTMATRIXBLUE") { currentEffect = TEXT_MATRIX; matrix_text(1000, 1); }
        else if (payload == "RESET") { strip.clear(); strip.show(); currentEffect = IDLE; }
        else setRandomColor();
    } else {
        Serial.println("HTTP error: " + String(httpCode));
    }
    http.end();
}

void rainbowEffect(int wait) {
    static int j = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.ColorHSV(((i + j) & 255) * 256, 255, 255));
    }
    strip.show();
    j = (j + 1) % 256;
    delay(wait);
    if (newCommandReceived) { newCommandReceived = false; currentEffect = IDLE; }
}

void babelFishSwarm() {
    static int j = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
        int hue = (i + j) % 170 * (65535 / 170);
        strip.setPixelColor(i, strip.ColorHSV(hue, 255, 255));
        if (random(20) == 0) strip.setPixelColor(i, strip.Color(0, 255, 255)); // Fish darting
    }
    strip.show();
    j = (j + 1) % 170;
    delay(50);
    if (newCommandReceived) { newCommandReceived = false; currentEffect = IDLE; }
}

void panGalacticGargleBlaster() {
    static int cycle = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
        int r = random(2) ? 0 : 255 * (i % 3 == 0);
        int g = random(2) ? 255 * (i % 3 == 1) : 0;
        int b = random(2) ? 0 : 255 * (i % 3 == 2);
        if (random(20) == 0) strip.setPixelColor(i, strip.Color(255, 255, 255)); // Explosions
        else strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();
    delay(random(30, 100));
    if (cycle++ % 10 == 0) { strip.clear(); strip.show(); delay(50); } // Blackout
    if (newCommandReceived) { newCommandReceived = false; currentEffect = IDLE; }
}

void infiniteImprobabilityDrive() {
    static int cycle = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
        if (cycle % 20 < 5) { // Probability wave
            strip.setPixelColor(i, strip.ColorHSV((i * 256 + cycle * 100) % 65535, 255, 255));
        } else { // Chaos
            strip.setPixelColor(i, strip.Color(random(256), random(256), random(256)));
        }
    }
    strip.show();
    cycle++;
    delay(40);
    if (newCommandReceived) { newCommandReceived = false; currentEffect = IDLE; }
}

void vogonPoetrySlam() {
    static int j = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
        int brightness = (sin((i + j) * 0.1) + 1) * 127;
        strip.setPixelColor(i, strip.Color(0, brightness * 0.8, brightness * 0.4));
        if (random(50) == 0) strip.setPixelColor(i, strip.Color(255, 0, 0)); // Jarring flash
    }
    strip.show();
    j++;
    delay(80);
    if (newCommandReceived) { newCommandReceived = false; currentEffect = IDLE; }
}

void heartOfGoldTrail() {
    static int pos = 0, dir = 1;
    for (int i = 0; i < NUM_LEDS; i++) {
        uint32_t c = strip.getPixelColor(i);
        strip.setPixelColor(i, strip.Color(((c >> 16) & 0xFF) * 0.9, ((c >> 8) & 0xFF) * 0.9, (c & 0xFF) * 0.9));
        if (i == pos) strip.setPixelColor(i, strip.Color(255, 215, 0)); // Golden comet
        else if (abs(i - pos) < 10 && random(10) == 0) strip.setPixelColor(i, strip.Color(0, 0, random(100, 255))); // Starry twinkle
    }
    strip.show();
    pos += dir;
    if (pos >= NUM_LEDS || pos <= 0) dir = -dir;
    delay(20);
    if (newCommandReceived) { newCommandReceived = false; currentEffect = IDLE; }
}

void dontPanicPulse() {
    static unsigned long lastUpdate = 0;
    static int dotCount = 0;
    if (millis() - lastUpdate >= 80) {
        pulseBrightness += pulseDirection * 1;
        if (pulseBrightness >= 255 || pulseBrightness <= 20) pulseDirection *= -1;
        for (int i = 0; i < NUM_LEDS; i++) {
            strip.setPixelColor(i, dimColor(strip.Color(0, 100, 150), pulseBrightness));
            if (random(50) == 0 && dotCount < 4) { strip.setPixelColor(i, strip.Color(255, 255, 0)); dotCount++; }
        }
        strip.show();
        lastUpdate = millis();
        dotCount = 0;
    }
    if (newCommandReceived) { newCommandReceived = false; currentEffect = IDLE; }
}

void fight_kampfen() {
    for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t r = fight_kampf[i] ? 0 : 255;
        uint8_t b = fight_kampf[i] ? 255 : 0;
        strip.setPixelColor(i, strip.Color(r, 42, b));
        if (random(100) == 0 && i < NUM_LEDS - 1) fight_kampf[i] = fight_kampf[i + 1];
        if (random(100) == 0 && i > 0) fight_kampf[i] = fight_kampf[i - 1];
    }
    strip.show();
    delay(50);
    if (newCommandReceived) { newCommandReceived = false; currentEffect = IDLE; }
}

int get_index(int x_in, int y_in) {
    return (x_in % 2 == 0) ? (y_in + x_in * 8) : ((7 - y_in) + x_in * 8);
}

void matrix_bee() {
    int rr = random(4);
    if (rr == 0 && bee_x < 31) bee_x++;
    else if (rr == 1 && bee_x > 0) bee_x--;
    else if (rr == 2 && bee_y < 7) bee_y++;
    else if (rr == 3 && bee_y > 0) bee_y--;
    int bee_index = (bee_x % 2 == 0) ? (bee_y + bee_x * 8) : ((7 - bee_y) + bee_x * 8);
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(i == bee_index ? 255 : 0, i == bee_index ? 255 : 0, i == bee_index ? 0 : 255));
    }
    strip.show();
    delay(1000);
    if (newCommandReceived) { newCommandReceived = false; currentEffect = IDLE; }
}

void matrix_text(uint16_t speed, int is_blue_background = 0) {
    int is_on[NUM_LEDS] = {0};
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 8; y++) {
            int index = get_index(x, y);
            if (x - text_sroll_x >= 0 && x - text_sroll_x < WIDTH_P) is_on[index] = letter_P[y][x - text_sroll_x];
            else if (x - text_sroll_x >= WIDTH_P + 1 && x - text_sroll_x < WIDTH_P + WIDTH_PLUS + 1) is_on[index] = letter_PLUS[y][x - text_sroll_x - WIDTH_P - 1];
            else if (x - text_sroll_x >= WIDTH_P + WIDTH_PLUS + 2 && x - text_sroll_x < WIDTH_P + WIDTH_PLUS + WIDTH_W + 2) is_on[index] = letter_W[y][x - text_sroll_x - WIDTH_P - WIDTH_PLUS - 2];
        }
    }
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(is_on[i] ? 255 : 0, is_on[i] ? 255 : 0, is_on[i] ? 0 : (is_blue_background ? 255 : 0)));
    }
    strip.show();
    text_sroll_x += text_sroll_x_dir;
    if (text_sroll_x > 31 - WIDTH_P - WIDTH_PLUS - WIDTH_W - 2) text_sroll_x_dir = -1;
    else if (text_sroll_x <= 0) text_sroll_x_dir = 1;
    delay(speed);
    if (newCommandReceived) { newCommandReceived = false; currentEffect = IDLE; }
}

void beatingHeartWave() {
    static int beat = 0;
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 50) {
        strip.clear();
        float throb = 8.0 * (1.0 + sin((millis() % 60) / 60.0 * PI)) * (beat % 2 ? 1.2 : 0.8);
        for (int pos = 0; pos < NUM_LEDS; pos++) {
            float intensity = 0.8 * constrain(1.0 - abs(pos - NUM_LEDS / 2) / throb, 0, 1);
            intensity = pow(intensity, 1.5);
            strip.setPixelColor(pos, random(100) > 95 ? strip.Color(255, 210, 50) : strip.Color(180 * intensity, 10 * intensity, 20 * intensity));
        }
        strip.show();
        lastUpdate = millis();
        beat = (beat + 1) % 3;
    }
    if (newCommandReceived || beat == 0) { newCommandReceived = false; currentEffect = IDLE; }
}

uint32_t parseColor(String hexStr) {
    hexStr.trim();
    if (hexStr.startsWith("#")) hexStr = hexStr.substring(1);
    if (hexStr.length() != 6) return strip.Color(255, 255, 255);
    long number = strtol(hexStr.c_str(), NULL, 16);
    return strip.Color((number >> 16) & 0xFF, (number >> 8) & 0xFF, number & 0xFF);
}

uint32_t dimColor(uint32_t color, uint8_t brightness) {
    float scale = brightness / 255.0;
    return strip.Color(((color >> 16) & 0xFF) * scale, ((color >> 8) & 0xFF) * scale, (color & 0xFF) * scale);
}

void setStripColor(uint32_t color) {
    for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, color);
    strip.show();
}

void setRandomColor() {
    setStripColor(strip.Color(random(256), random(256), random(256)));
    currentEffect = IDLE;
}
