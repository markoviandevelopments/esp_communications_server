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
const int ws_port = 5048; // Updated to 5048
const char *ws_path = "/";

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.println("WebSocket Disconnected");
        break;
    case WStype_CONNECTED:
        Serial.println("WebSocket Connected");
        break;
    case WStype_TEXT:
        JSONVar doc = JSON.parse((char *)payload);
        if (JSON.typeof(doc) == "object" && doc.hasOwnProperty("pattern"))
        {
            JSONVar pattern = doc["pattern"];
            if (JSON.typeof(pattern) == "array" && pattern.length() == 10)
            {
                for (int i = 0; i < 10; i++)
                {
                    int r = (int)pattern[i][0];
                    int g = (int)pattern[i][1];
                    int b = (int)pattern[i][2];
                    for (int j = 0; j < 30; j++)
                    { // 300 LEDs / 10 segments = 30 LEDs per segment
                        strip.setPixelColor(i * 30 + j, strip.Color(r, g, b));
                    }
                }
                strip.show();
                Serial.println("Pattern applied to LEDs");
            }
            else
            {
                Serial.println("Invalid pattern length");
            }
        }
        else
        {
            Serial.println("No valid pattern in message");
        }
        break;
    }
}

void setup()
{
    Serial.begin(115200);
    strip.begin();
    strip.show(); // Initialize all LEDs to off

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");

    webSocket.begin(ws_host, ws_port, ws_path);
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
}

void loop()
{
    webSocket.loop();
}