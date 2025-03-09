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

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
    case WStype_DISCONNECTED: {
        Serial.println("[WebSocket] Disconnected");
        break;
    }
    case WStype_CONNECTED: {
        Serial.println("[WebSocket] Connected to server");
        break;
    }
    case WStype_TEXT: {
        Serial.printf("[WebSocket] Received message: %s\n", payload);
        JSONVar doc = JSON.parse((char *)payload);
        if (JSON.typeof(doc) == "undefined") {
            Serial.println("[WebSocket] Failed to parse JSON");
            return;
        }
        if (JSON.typeof(doc) != "object" || !doc.hasOwnProperty("pattern")) {
            Serial.println("[WebSocket] No valid 'pattern' in message");
            return;
        }
        JSONVar pattern = doc["pattern"];
        if (JSON.typeof(pattern) != "array" || pattern.length() != 10) {
            Serial.println("[WebSocket] Invalid pattern length or type");
            return;
        }
        for (int i = 0; i < 10; i++) {
            if (JSON.typeof(pattern[i]) != "array" || pattern[i].length() != 3) {
                Serial.printf("[WebSocket] Invalid RGB tuple at index %d\n", i);
                return;
            }
            int r = (int)pattern[i][0];
            int g = (int)pattern[i][1];
            int b = (int)pattern[i][2];
            // Validate RGB values
            r = constrain(r, 0, 255);
            g = constrain(g, 0, 255);
            b = constrain(b, 0, 255);
            for (int j = 0; j < 30; j++) { // 300 LEDs / 10 segments = 30 LEDs per segment
                strip.setPixelColor(i * 30 + j, strip.Color(r, g, b));
            }
        }
        strip.show();
        Serial.println("[WebSocket] Pattern applied to LEDs");
        break;
    }
    case WStype_ERROR: {
        Serial.println("[WebSocket] Error occurred");
        break;
    }
    default: {
        Serial.printf("[WebSocket] Event type: %d\n", type);
        break;
    }
    }
}

void setup() {
    Serial.begin(115200);
    strip.begin();
    strip.show(); // Initialize all LEDs to off

    Serial.println("[WiFi] Connecting to network...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[WiFi] Connected! IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("[WebSocket] Connecting to server...");
    webSocket.begin(ws_host, ws_port, ws_path);
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
}

void loop() {
    webSocket.loop();
}