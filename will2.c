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
    // Validate pattern is an array of 10 RGB tuples
    if (JSON.typeof(pattern) != "array" || pattern.length() != 10)
    {
        // Fallback: Set LEDs to a debug color (e.g., blue) instead of turning off
        for (int i = 0; i < NUM_LEDS; i++)
        {
            strip.setPixelColor(i, strip.Color(0, 0, 255));
        }
        strip.show();
        return;
    }

    for (int i = 0; i < 10; i++)
    {
        if (JSON.typeof(pattern[i]) != "array" || pattern[i].length() != 3)
        {
            // Fallback: Set LEDs to red if RGB tuple is invalid
            for (int j = 0; j < NUM_LEDS; j++)
            {
                strip.setPixelColor(j, strip.Color(255, 0, 0));
            }
            strip.show();
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
        isAnimating = false;
        // Clear LEDs on disconnect
        strip.clear();
        strip.show();
        break;
    case WStype_CONNECTED:
        break;
    case WStype_TEXT:
    {
        JSONVar doc = JSON.parse((char *)payload);
        if (JSON.typeof(doc) == "undefined")
        {
            return;
        }

        // Static pattern
        if (doc.hasOwnProperty("pattern"))
        {
            isAnimating = false;
            JSONVar pattern = doc["pattern"];
            applyPattern(pattern);
        }
        // Animated pattern
        else if (doc.hasOwnProperty("frames") && doc.hasOwnProperty("frame_rate"))
        {
            animationFrames = doc["frames"];
            if (JSON.typeof(animationFrames) != "array" || animationFrames.length() < 1)
            {
                // Fallback: Set LEDs to green if frames are invalid
                for (int i = 0; i < NUM_LEDS; i++)
                {
                    strip.setPixelColor(i, strip.Color(0, 255, 0));
                }
                strip.show();
                return;
            }
            frameCount = animationFrames.length();
            double frame_rate = (double)doc["frame_rate"];
            // Ensure frame_rate is reasonable
            if (frame_rate < 0.05 || frame_rate > 1.0)
            {
                frame_rate = 0.1; // Default to 100ms if out of range
            }
            frameDelay = (unsigned long)(frame_rate * 1000);
            currentFrame = 0;
            lastFrameTime = millis();
            isAnimating = true;
            // Apply the first frame immediately
            applyPattern(animationFrames[currentFrame]);
        }
        break;
    }
    case WStype_ERROR:
        break;
    default:
        break;
    }
}

void setup()
{
    strip.begin();
    strip.clear(); // Ensure LEDs start off
    strip.show();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }

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
            currentFrame = (currentFrame + 1) % frameCount;
            applyPattern(animationFrames[currentFrame]);
            lastFrameTime = currentTime;
        }
    }
}