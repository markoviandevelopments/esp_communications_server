#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Replace with your WiFi credentials
const char* ssid     = "Brubaker Wifi";
const char* password = "Pre$ton01";

// Server URL to poll
const char* serverUrl = "http://50.188.120.138:5000";

// Define the LED pin (adjust as needed for your board)
const int LED_PIN = 2;  // GPIO2 (Common LED pin on many ESP8266 modules)

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW); // Start with LED off

    // Connect to WiFi
    Serial.println("\nConnecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;
        
        Serial.println("Attempting to connect to server...");
        http.begin(client, serverUrl);
        int httpCode = http.GET();

        if (httpCode > 0) {  // Successful response
            String payload = http.getString();
            payload.trim(); // Remove whitespace

            Serial.print("Server response: ");
            Serial.println(payload);

            if (payload.equalsIgnoreCase("FAST")) {
                blinkLED(100, 10);  // Fast blink (100ms on/off, 10 times)
            } else if (payload.equalsIgnoreCase("SLOW")) {
                blinkLED(500, 10);  // Slow blink (500ms on/off, 10 times)
            } else if (payload.equalsIgnoreCase("VALENTINE")) { // Will's addition
                blinkLED(1000, 3)
            } else {
                Serial.println("Unrecognized response; doing occasional blink.");
                occasionalBlink();
            }
        } else {  // Server error or no response
            Serial.print("HTTP error: ");
            Serial.println(httpCode);
            occasionalBlink();
        }

        http.end(); // Close connection
    } else {
        Serial.println("WiFi not connected.");
        occasionalBlink();
    }

    delay(10000); // Wait 10 seconds before polling again
}

// Function to blink LED a given number of times with specified delay
void blinkLED(int blinkDelay, int count) {
    for (int i = 0; i < count; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(blinkDelay);
        digitalWrite(LED_PIN, LOW);
        delay(blinkDelay);
    }
}

// Function for occasional short blink when no valid response is received
void occasionalBlink() {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(1000);
}
