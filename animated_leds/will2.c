#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino_JSON.h>

#define NUM_LEDS 300
#define LED_PIN 2 // GPIO2

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Error Color Codes
#define COLOR_INVALID_STRUCTURE strip.Color(255, 0, 0)   // Red
#define COLOR_INVALID_RGB strip.Color(255, 165, 0)       // Orange
#define COLOR_WS_ERROR strip.Color(0, 0, 255)            // Blue
#define COLOR_JSON_ERROR strip.Color(128, 0, 128)        // Purple
#define COLOR_CONNECTION_OK strip.Color(0, 255, 0)       // Green
#define COLOR_FRAME_COUNT_ERROR strip.Color(255, 255, 0) // Yellow
#define COLOR_ANIMATION_ENDED strip.Color(0, 255, 255)   // Cyan

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
unsigned long frameDelay = 0;
unsigned long lastFrameTime = 0;
bool isAnimating = false;

void showError(uint32_t color)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        strip.setPixelColor(i, color);
    }
    strip.show();
    delay(500);
    strip.clear();
    strip.show();
    delay(500);
}

void applyPattern(JSONVar pattern)
{
    strip.clear();

    if (JSON.typeof(pattern) != "array")
    {
        showError(COLOR_INVALID_STRUCTURE);
        return;
    }

    if (pattern.length() != 10)
    {
        for (int i = 0; i < 10; i++)
        {
            showError(COLOR_INVALID_STRUCTURE);
            delay(100);
        }
        return;
    }

    for (int i = 0; i < 10; i++)
    {
        JSONVar pixelGroup = pattern[i];

        if (JSON.typeof(pixelGroup) != "array" || pixelGroup.length() != 3)
        {
            for (int j = 0; j < 30; j++)
            {
                strip.setPixelColor(i * 30 + j, COLOR_INVALID_RGB);
            }
            strip.show();
            delay(1000);
            return;
        }

        int r = (int)pixelGroup[0];
        int g = (int)pixelGroup[1];
        int b = (int)pixelGroup[2];

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
        showError(COLOR_WS_ERROR);
        isAnimating = false;
        break;

    case WStype_CONNECTED:
        showError(COLOR_CONNECTION_OK);
        break;

    case WStype_TEXT:
    {
        JSONVar doc = JSON.parse((char *)payload);

        if (JSON.typeof(doc) == "undefined")
        {
            for (int i = 0; i < 3; i++)
                showError(COLOR_JSON_ERROR);
            return;
        }

        // Handle static pattern
        if (doc.hasOwnProperty("pattern"))
        {
            isAnimating = false;
            applyPattern(doc["pattern"]);
        }
        // Handle animation
        else if (doc.hasOwnProperty("frames") && doc.hasOwnProperty("frame_rate"))
        {
            // Validation
            if (JSON.typeof(doc["frames"]) != "array" || doc["frames"].length() < 1)
            {
                showError(COLOR_INVALID_STRUCTURE);
                return;
            }

            JSONVar firstFrame = doc["frames"][0];
            if (JSON.typeof(firstFrame) != "array" || firstFrame.length() != 10)
            {
                showError(COLOR_INVALID_STRUCTURE);
                return;
            }

            animationFrames = doc["frames"];
            frameCount = animationFrames.length();

            if (frameCount < 1)
            {
                showError(COLOR_FRAME_COUNT_ERROR);
                return;
            }

            // Correct frame delay calculation (convert seconds to milliseconds)
            double frameRate = (double)doc["frame_rate"];
            if (frameRate <= 0)
            {
                showError(COLOR_FRAME_COUNT_ERROR);
                return;
            }
            frameDelay = (unsigned long)(1000.0 / frameRate); // Convert FPS to ms delay
            currentFrame = 0;
            lastFrameTime = millis();
            isAnimating = true;

            // Debug: Flash cyan to confirm animation start
            showError(COLOR_ANIMATION_ENDED);
        }
        break;
    }

    case WStype_ERROR:
        showError(COLOR_WS_ERROR);
        break;
    }
}

void setup()
{
    strip.begin();
    strip.show();

    for (int i = 0; i < 3; i++)
    {
        showError(COLOR_CONNECTION_OK);
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        showError(COLOR_WS_ERROR);
        delay(500);
    }

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
        currentFrame++;
        if (currentFrame >= frameCount)
        {
            currentFrame = 0; // Loop animation
            // Optional: Uncomment to stop after one cycle
            // isAnimating = false;
            // showError(COLOR_ANIMATION_ENDED);
        }
        lastFrameTime = millis();
    }
}