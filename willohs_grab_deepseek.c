#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
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

void setStaticPattern(JSONVar &doc) {
    for (int i = 0; i < 10; i++) {
        int r = (int)doc["colors"][i][0];
        int g = (int)doc["colors"][i][1];
        int b = (int)doc["colors"][i][2];
        for (int j = 0; j < 30; j++) {
            strip.setPixelColor(i * 30 + j, strip.Color(r, g, b));
        }
    }
    strip.show();
}

void runAnimation(JSONVar &doc) {
    int speed = (int)doc["speed"]; // Speed in milliseconds per step
    JSONVar colors = doc["colors"];
    int numColors = colors.length();
    unsigned long lastUpdate = millis();

    while (true) { // Animation loop; break if new data arrives
        unsigned long currentTime = millis();
        if (currentTime - lastUpdate >= speed) {
            for (int i = 0; i < NUM_LEDS; i++) {
                int colorIndex = (i + (currentTime / speed)) % numColors;
                int r = (int)colors[colorIndex][0];
                int g = (int)colors[colorIndex][1];
                int b = (int)colors[colorIndex][2];
                strip.setPixelColor(i, strip.Color(r, g, b));
            }
            strip.show();
            lastUpdate = currentTime;
        }
        delay(10); // Small delay to avoid overwhelming the ESP
        if (WiFi.status() != WL_CONNECTED) break; // Exit if disconnected
    }
}

void runGradient(JSONVar &doc) {
    JSONVar colors = doc["colors"];
    int numColors = colors.length();
    for (int i = 0; i < NUM_LEDS; i++) {
        float position = (float)i / (NUM_LEDS - 1); // 0.0 to 1.0 across strip
        int segment = position * (numColors - 1);
        float fraction = position * (numColors - 1) - segment;
        
        int r1 = (int)colors[segment][0];
        int g1 = (int)colors[segment][1];
        int b1 = (int)colors[segment][2];
        int r2 = (int)colors[segment + 1][0];
        int g2 = (int)colors[segment + 1][1];
        int b2 = (int)colors[segment + 1][2];
        
        int r = r1 + (r2 - r1) * fraction;
        int g = g1 + (g2 - g1) * fraction;
        int b = b1 + (b2 - b1) * fraction;
        strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();
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

            JSONVar doc = JSON.parse(payload);
            if (JSON.typeof(doc) != "undefined") {
                String patternType = (const char*)doc["type"];
                if (patternType == "color") {
                    setStaticPattern(doc);
                } else if (patternType == "animation") {
                    runAnimation(doc);
                } else if (patternType == "gradient") {
                    runGradient(doc);
                } else {
                    Serial.println("Unknown pattern type");
                }
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
    delay(1000);
}
