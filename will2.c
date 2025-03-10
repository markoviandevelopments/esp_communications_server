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

void setDebugColor(uint32_t color)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        strip.setPixelColor(i, color);
    }
    strip.show();
}

void applyPattern(JSONVar &pattern)
{
    if (strcmp(JSON.typeof(pattern), "array") != 0 || pattern.length() != 10)
    {
        setDebugColor(strip.Color(0, 0, 255)); // Blue: Invalid pattern
        return;
    }

    for (int i = 0; i < 10; i++)
    {
        if (strcmp(JSON.typeof(pattern[i]), "array") != 0 || pattern[i].length() != 3)
        {
            setDebugColor(strip.Color(255, 0, 0)); // Red: Invalid RGB tuple
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
        strip.clear();
        strip.show();
        break;
    case WStype_CONNECTED:
        break;
    case WStype_TEXT:
    {
        JSONVar doc = JSON.parse((char *)payload);
        if (strcmp(JSON.typeof(doc), "undefined") == 0)
        {
            setDebugColor(strip.Color(255, 255, 0)); // Yellow: JSON parse failed
            return;
        }

        if (doc.hasOwnProperty("pattern"))
        {
            isAnimating = false;
            JSONVar pattern = doc["pattern"];
            applyPattern(pattern);
        }
        else if (doc.hasOwnProperty("frames") && doc.hasOwnProperty("frame_rate"))
        {
            animationFrames = doc["frames"];
            if (strcmp(JSON.typeof(animationFrames), "array") != 0 || animationFrames.length() < 1)
            {
                setDebugColor(strip.Color(0, 255, 0)); // Green: Invalid frames array
                return;
            }
            frameCount = animationFrames.length();
            double frame_rate = (double)doc["frame_rate"];
            if (frame_rate < 0.05 || frame_rate > 1.0)
            {
                frame_rate = 0.1;
            }
            frameDelay = (unsigned long)(frame_rate * 1000);
            currentFrame = 0;
            lastFrameTime = millis();
            isAnimating = true;
            applyPattern(animationFrames[currentFrame]);
        }
        else
        {
            setDebugColor(strip.Color(255, 0, 255)); // Magenta: Unknown format
        }
        break;
    }
    case WStype_ERROR:
        setDebugColor(strip.Color(255, 165, 0)); // Orange: WebSocket error
        break;
    default:
        break;
    }
}

void setup()
{
    strip.begin();
    strip.clear();
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