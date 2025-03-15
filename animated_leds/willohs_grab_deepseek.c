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

void applyPattern(JSONVar &pattern)
{
    if (JSON.typeof(pattern) != "array" || pattern.length() != 10)
    {
        Serial.printf("[WebSocket] Invalid pattern: type=%s, length=%d\n", JSON.typeof(pattern).c_str(), pattern.length());
        return;
    }
    for (int i = 0; i < 10; i++)
    {
        if (JSON.typeof(pattern[i]) != "array" || pattern[i].length() != 3)
        {
            Serial.printf("[WebSocket] Invalid RGB at %d\n", i);
            return;
        }
        int r = (int)pattern[i][0];
        int g = (int)pattern[i][1];
        int b = (int)pattern[i][2];
        r = constrain(r, 0, 255);
        g = constrain(g, 0, 255);
        b = constrain(b, 0, 255);
        for (int j = 0; j < 30; j++)
        {
            strip.setPixelColor(i * 30 + j, strip.Color(r, g, b));
        }
    }
    strip.show();
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.println("[WebSocket] Disconnected");
        isAnimating = false; // Stop animation on disconnect
        break;
    case WStype_CONNECTED:
        Serial.printf("[WebSocket] Connected to ws://%s:%d%s\n", ws_host, ws_port, ws_path);
        break;
    case WStype_TEXT:
    {
        Serial.printf("[WebSocket] Received: %s\n", payload);
        JSONVar doc = JSON.parse((char *)payload);
        if (JSON.typeof(doc) == "undefined")
        {
            Serial.println("[WebSocket] JSON Parse Failed");
            return;
        }

        // Static pattern
        if (doc.hasOwnProperty("pattern"))
        {
            isAnimating = false; // Stop any running animation
            JSONVar pattern = doc["pattern"];
            applyPattern(pattern);
            Serial.println("[WebSocket] Static Pattern Applied");
        }
        // Animated pattern
        else if (doc.hasOwnProperty("frames") && doc.hasOwnProperty("frame_rate"))
        {
            animationFrames = doc["frames"];
            frameCount = animationFrames.length();
            double frame_rate = (double)doc["frame_rate"]; // Seconds per frame
            if (JSON.typeof(animationFrames) != "array" || frameCount < 1)
            {
                Serial.println("[WebSocket] Invalid frames array");
                return;
            }
            frameDelay = (unsigned long)(frame_rate * 1000); // ms
            currentFrame = 0;
            lastFrameTime = millis();
            isAnimating = true;
            Serial.printf("[WebSocket] Animation started: %d frames, %lu ms delay\n", frameCount, frameDelay);
        }
        else
        {
            Serial.println("[WebSocket] Unknown message format");
        }
        break;
    }
    case WStype_ERROR:
        Serial.printf("[WebSocket] Error: %s\n", payload);
        break;
    default:
        Serial.printf("[WebSocket] Event: %d\n", type);
        break;
    }
}

void setup()
{
    Serial.begin(115200);
    strip.begin();
    strip.show();

    WiFi.begin(ssid, password);
    Serial.print("[WiFi] Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[WiFi] Connected, IP: ");
    Serial.println(WiFi.localIP());

    Serial.printf("[WebSocket] Connecting to ws://%s:%d%s\n", ws_host, ws_port, ws_path);
    webSocket.begin(ws_host, ws_port, ws_path);
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
}

void loop()
{
    webSocket.loop();

    if (isAnimating)
    {
        unsigned long currentTime = millis();
        if (currentTime - lastFrameTime >= frameDelay)
        {
            applyPattern(animationFrames[currentFrame]);
            currentFrame = (currentFrame + 1) % frameCount; // Cycle frames
            lastFrameTime = currentTime;
        }
    }
}