#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino_JSON.h>

#define NUM_LEDS 300
#define LED_PIN 2 // GPIO2

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

const char *ssid = "Brubaker Wifi";
const char *password = "Pre$ton01";
const char *ws_host = "10.1.10.79";
const int ws_port = 5048;
const char *ws_path = "/";

WebSocketsClient webSocket;

// Animation state
JSONVar animationFrames;
int frameCount = 0;
int currentFrame = 0;
unsigned long frameDelay = 0; // ms
unsigned long lastFrameTime = 0;
bool isAnimating = false;

// Visual feedback state
unsigned long lastFeedbackTime = 0;
int feedbackPhase = 0;

void setAllLEDs(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();
}

void applyPattern(JSONVar pattern) { // Pass by value
    if (JSON.typeof(pattern) != "array" || pattern.length() != 10) {
        setAllLEDs(255, 0, 0); // Red flash for error
        delay(200);
        setAllLEDs(0, 0, 0);
        delay(200);
        setAllLEDs(255, 0, 0);
        delay(200);
        setAllLEDs(0, 0, 0);
        return;
    }
    for (int i = 0; i < 10; i++) {
        if (JSON.typeof(pattern[i]) != "array" || pattern[i].length() != 3) {
            setAllLEDs(255, 0, 0); // Red flash for error
            delay(200);
            setAllLEDs(0, 0, 0);
            delay(200);
            setAllLEDs(255, 0, 0);
            delay(200);
            setAllLEDs(0, 0, 0);
            return;
        }
        int r = (int)pattern[i][0];
        int g = (int)pattern[i][1];
        int b = (int)pattern[i][2];
        r = constrain(r, 0, 255);
        g = constrain(g, 0, 255);
        b = constrain(b, 0, 255);
        for (int j = 0; j < 30; j++) {
            strip.setPixelColor(i * 30 + j, strip.Color(r, g, b));
        }
    }
    strip.show();
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
    case WStype_DISCONNECTED:
        isAnimating = false;
        frameCount = 0; // Reset animation state
        setAllLEDs(0, 0, 0); // Clear LEDs
        break;
    case WStype_CONNECTED:
        setAllLEDs(10, 10, 10); // Dim white when connected
        break;
    case WStype_TEXT: {
        JSONVar doc = JSON.parse((char *)payload);
        if (JSON.typeof(doc) == "undefined") {
            applyPattern(JSONVar()); // Trigger error feedback
            return;
        }

        // Static pattern
        if (doc.hasOwnProperty("pattern")) {
            isAnimating = false;
            JSONVar pattern = doc["pattern"];
            applyPattern(pattern);
        }
        // Animated pattern
        else if (doc.hasOwnProperty("frames") && doc.hasOwnProperty("frame_rate")) {
            animationFrames = doc["frames"];
            frameCount = animationFrames.length();
            double frame_rate = (double)doc["frame_rate"];
            if (JSON.typeof(animationFrames) != "array" || frameCount < 1) {
                applyPattern(JSONVar()); // Trigger error feedback
                return;
            }
            frameDelay = (unsigned long)(frame_rate * 1000);
            currentFrame = 0;
            lastFrameTime = millis();
            isAnimating = true;
        }
        else {
            applyPattern(JSONVar()); // Trigger error feedback
        }
        break;
    }
    case WStype_ERROR:
        applyPattern(JSONVar()); // Trigger error feedback
        break;
    default:
        break;
    }
}

void setup() {
    Serial.begin(115200);
    strip.begin();
    setAllLEDs(0, 0, 0); // Start dark

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        setAllLEDs(20, 10, 0); // Orange flash while connecting
        delay(250);
        setAllLEDs(0, 0, 0);
        delay(250);
    }
    setAllLEDs(10, 10, 10); // Dim white when WiFi connected

    webSocket.begin(ws_host, ws_port, ws_path);
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
}

void loop() {
    webSocket.loop();

    if (isAnimating) {
        unsigned long currentTime = millis();
        if (currentTime - lastFrameTime >= frameDelay) {
            applyPattern(animationFrames[currentFrame]);
            currentFrame = (currentFrame + 1) % frameCount;
            lastFrameTime = currentTime;
        }
    } else {
        // Visual feedback when idle
        unsigned long currentTime = millis();
        if (currentTime - lastFeedbackTime >= 1000) {
            if (!webSocket.isConnected()) {
                // Orange pulse when disconnected
                if (feedbackPhase == 0) {
                    setAllLEDs(20, 10, 0);
                    feedbackPhase = 1;
                } else {
                    setAllLEDs(0, 0, 0);
                    feedbackPhase = 0;
                }
            } else {
                setAllLEDs(10, 10, 10); // Dim white when connected, no pattern
            }
            lastFeedbackTime = currentTime;
        }
    }
}