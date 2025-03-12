#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino_JSON.h>

#define NUM_LEDS 300
#define LED_PIN 2 // GPIO2

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Error Color Codes (All LEDs will pulse these colors)
#define COLOR_INVALID_STRUCTURE strip.Color(255, 0, 0) // Red
#define COLOR_INVALID_RGB strip.Color(255, 165, 0)     // Orange
#define COLOR_WS_ERROR strip.Color(0, 0, 255)          // Blue
#define COLOR_JSON_ERROR strip.Color(128, 0, 128)      // Purple
#define COLOR_CONNECTION_OK strip.Color(0, 255, 0)     // Green
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
    // First clear previous pattern
    strip.clear();

    // Structure Validation
    if (JSON.typeof(pattern) != "array")
    {
        showError(COLOR_INVALID_STRUCTURE);
        return;
    }

    if (pattern.length() != 10)
    {
        // Show error with number of segments (10 blinks)
        for (int i = 0; i < 10; i++)
        {
            showError(COLOR_INVALID_STRUCTURE);
            delay(100);
        }
        return;
    }

    // Process Pattern
    for (int i = 0; i < 10; i++)
    {
        JSONVar pixelGroup = pattern[i];

        // Row Validation
        if (JSON.typeof(pixelGroup) != "array")
        {
            // Blink the faulty segment (i)
            for (int j = 0; j < 30; j++)
            {
                strip.setPixelColor(i * 30 + j, COLOR_INVALID_RGB);
            }
            strip.show();
            delay(1000);
            return;
        }

        if (pixelGroup.length() != 3)
        {
            // Blink segment with invalid RGB length
            for (int j = 0; j < 30; j++)
            {
                strip.setPixelColor(i * 30 + j, COLOR_INVALID_RGB);
            }
            strip.show();
            delay(1000);
            return;
        }

        // Extract RGB values
        int r = (int)pixelGroup[0];
        int g = (int)pixelGroup[1];
        int b = (int)pixelGroup[2];

        // Clamp values
        r = constrain(r, 0, 255);
        g = constrain(g, 0, 255);
        b = constrain(b, 0, 255);

        // Apply to 30 LEDs
        for (int j = 0; j < 30; j++)
        {
            strip.setPixelColor(i * 30 + j, strip.Color(r, g, b));
        }
    }
    strip.show();
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_TEXT: {
            JSONVar doc = JSON.parse((char *)payload);

            // ===== ADD VALIDATION =====
            if (doc.hasOwnProperty("frames"))
            {
                // Check frames array
                if (JSON.typeof(doc["frames"]) != "array" || doc["frames"].length() < 1)
                {
                    showError(COLOR_INVALID_STRUCTURE);
                    return;
                }

                // Check first frame structure
                JSONVar firstFrame = doc["frames"][0];
                if (JSON.typeof(firstFrame) != "array" || firstFrame.length() != 10)
                {
                    showError(COLOR_INVALID_STRUCTURE);
                    return;
                }
            }
        }
        case WStype_DISCONNECTED:
            showError(COLOR_WS_ERROR);
            isAnimating = false;
            break;

        case WStype_CONNECTED:
            // Quick green pulse on connection
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

            if (doc.hasOwnProperty("pattern"))
            {
                isAnimating = false;
                applyPattern(doc["pattern"]);
            }
            else if (doc.hasOwnProperty("frames") && doc.hasOwnProperty("frame_rate"))
            {
                animationFrames = doc["frames"];
                frameCount = animationFrames.length();

                if (frameCount < 1)
                {
                    showError(COLOR_FRAME_COUNT_ERROR);
                    return;
                }

                for (int i = 0; i < frameCount; i++)
                {
                    showError(COLOR_ANIMATION_ENDED);
                    delay(200);
                }

                frameDelay = (unsigned long)((double)doc["frame_rate"] * 1000);
                currentFrame = 0;
                lastFrameTime = millis();
                isAnimating = true;
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

            // Initial connection sequence
            for (int i = 0; i < 3; i++)
            {
                showError(COLOR_CONNECTION_OK);
            }

            WiFi.begin(ssid, password);
            while (WiFi.status() != WL_CONNECTED)
            {
                showError(COLOR_WS_ERROR); // Red flash while connecting
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
                if (currentFrame >= frameCount)
                {
                    showError(COLOR_INVALID_STRUCTURE);
                    isAnimating = false;
                    return;
                }

                applyPattern(animationFrames[currentFrame]);
                currentFrame = (currentFrame + 1) % frameCount;
                lastFrameTime = millis();
            }
        }