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

JSONVar animationFrames;
int frameCount = 0;
int currentFrame = 0;
unsigned long frameDelay = 0;
unsigned long lastFrameTime = 0;
bool isAnimating = false;

void applyPattern(JSONVar pattern)
{
    if (JSON.typeof(pattern) != "array" || pattern.length() != 10)
    {
        Serial.printf("Invalid pattern: type=%s, length=%d\n", JSON.typeof(pattern).c_str(), pattern.length());
        return;
    }

    for (int i = 0; i < 10; i++)
    {
        JSONVar pixel = pattern[i];
        if (JSON.typeof(pixel) != "array" || pixel.length() != 3)
        {
            Serial.printf("Invalid RGB at %d\n", i);
            return;
        }

        int r = (int)pixel[0];
        int g = (int)pixel[1];
        int b = (int)pixel[2];
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
        isAnimating = false;
        break;
    case WStype_CONNECTED:
        Serial.printf("[WebSocket] Connected to %s\n", payload);
        break;
    case WStype_TEXT:
    {
        Serial.printf("[WebSocket] Received: %s\n", (char *)payload);
        JSONVar doc = JSON.parse((char *)payload);
        if (JSON.typeof(doc) == "undefined")
        {
            Serial.println("JSON Parse Failed");
            return;
        }

        if (doc.hasOwnProperty("pattern"))
        {
            isAnimating = false;
            applyPattern(doc["pattern"]);
            Serial.println("Static Pattern Applied");
        }
        else if (doc.hasOwnProperty("frames") && doc.hasOwnProperty("frame_rate"))
        {
            animationFrames = doc["frames"];
            frameCount = animationFrames.length();
            frameDelay = (unsigned long)((double)doc["frame_rate"] * 1000);
            currentFrame = 0;
            lastFrameTime = millis();
            isAnimating = true;
            Serial.printf("Animation started: %d frames, %lu ms delay\n", frameCount, frameDelay);
        }
        break;
    }
    case WStype_ERROR:
        Serial.printf("[WebSocket] Error: %s\n", payload);
        break;
    }
}

void setup()
{
    Serial.begin(115200);
    strip.begin();
    strip.show();

    // Set max payload size (only works with WebSockets v2.6.1+)
    webSocket.setMaxPayloadSize(4096); // <-- Verify this line compiles

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected");

    webSocket.begin(ws_host, ws_port, ws_path);
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
}

void loop()
{
    webSocket.loop();
    if (isAnimating && (millis() - lastFrameTime >= frameDelay))
    {
        applyPattern(animationFrames[currentFrame]);
        currentFrame = (currentFrame + 1) % frameCount;
        lastFrameTime = millis();
    }
}