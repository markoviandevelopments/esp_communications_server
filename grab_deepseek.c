#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>

#define NUM_LEDS 300
#define LED_PIN 2  // GPIO2 for data signal

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

const char *ssid = "Brubaker Wifi";
const char *password = "Pre$ton01";
const char *serverUrl = "http://10.1.10.79:5048/led_pattern";

void setup() {
    Serial.begin(115200);
    strip.begin();
    strip.show();
    
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;

        http.begin(client, serverUrl);
        int httpCode = http.GET();
        if (httpCode > 0) {
            String payload = http.getString();
            Serial.println("Received JSON:");
            Serial.println(payload);

            StaticJsonDocument<1024> doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                for (int i = 0; i < 10; i++) {
                    int r = doc[i][0];
                    int g = doc[i][1];
                    int b = doc[i][2];
                    
                    for (int j = 0; j < 30; j++) {
                        strip.setPixelColor(i * 30 + j, strip.Color(r, g, b));
                    }
                }
                strip.show();
            } else {
                Serial.println("Failed to parse JSON");
            }
        } else {
            Serial.print("HTTP error: ");
            Serial.println(httpCode);
        }
        http.end();
    } else {
        Serial.println("WiFi not connected.");
    }
    delay(5000);
}
