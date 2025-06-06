#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <esp_sleep.h>
#include "loader.h"
#include "connection.h"
#include "websocket.h"
#include "XRPL_Logo.h"
// #include "Xahau_Logo.h"

// Global variable to track "off" state
bool isOff = false;

// Pin definitions
// #define SPEAKER_PIN 7        // GPIO pin for speaker (optional, not used in this example)
#define HAPTIC_PIN 8          // GPIO8 for haptic alerts
#define BUTTON_PIN 9          // GPIO pin connected to the button ( must be RTC capable )

// Variables for button handling
#define LONG_PRESS_TIME 2000  // Time in milliseconds to differentiate long and short presses
#define DEBOUNCE_DELAY 1000     // Debounce delay in milliseconds
static unsigned long lastDebounceTime = 0; // Timestamp of the last button state change
static bool buttonState = HIGH;           // Current stable state of the button

// Variables for WiFi connection
unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000; // 5 seconds


// Variables to store server info
String hostID;
int ioLatencyMs;
int uptime;
int previousUptime = -1;
String buildVersion;
String completeLedgers;
float loadFactor;
int peers;

// WebSocket server address
const char* websocket_host = "xrplcluster.com"; // XRPL - "s2.ripple.com", "xrplcluster.com" || Xahau - "xahau.network", "xahau.org"
const uint16_t websocket_port = 443;
const char* websocket_url = "/";

// Replace handleWebSocketMessage
void handleWebSocketMessage(const uint8_t* payload, size_t length) {
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  
  // Check if the message contains "ledger_index"
  if (doc.containsKey("ledger_index")) {
    Serial.println("Ledger update received, fetching server_info");
    WebSocketConnection::loop(); 
    delay(10);
    WebSocketConnection::send("{\"command\": \"server_info\"}");
  }
  
  // Check if the message contains "result" and "info".
  if (doc.containsKey("result")) {
    JsonObject result = doc["result"];

    // Check if the result contains "info" extract the data.
    if (result.containsKey("info")) {
      JsonObject info = result["info"];
      if (info.containsKey("hostid")) hostID = info["hostid"].as<String>();
      if (info.containsKey("io_latency_ms")) ioLatencyMs = info["io_latency_ms"].as<int>();
      if (info.containsKey("uptime")) uptime = info["uptime"];
      if (info.containsKey("build_version")) buildVersion = info["build_version"].as<String>();
      if (info.containsKey("complete_ledgers")) completeLedgers = info["complete_ledgers"].as<String>();
      if (info.containsKey("load_factor")) loadFactor = info["load_factor"];
      if (info.containsKey("peers")) peers = info["peers"];
      displayData();
    }
  }
}

// Alert simulation high i/o latancy limit reached
#define LATENCY_THRESHOLD 5 // IO latency threshold in milliseconds
#define ALERT_DURATION 5000   // Total alert duration in milliseconds
#define FLASH_INTERVAL 500    // Flash interval in milliseconds
#define ALERT_TIMEOUT 60000   // 60-second timeout in milliseconds
unsigned long lastAlertTime = 0; // Tracks the last time the alert was triggered

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables for display switching
unsigned long lastDisplayUpdate = 0;
const unsigned long displayInterval = 8000; // 8 seconds
int displayPage = 0;

void setup() {

  // Initialize the PINs
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(HAPTIC_PIN, OUTPUT);
  digitalWrite(HAPTIC_PIN, LOW);

  // Initialize Serial for debugging
  Serial.begin(9600);
  delay(500);
  
  Serial.println("ESP Starting");

  // Initialize SSD1306 display FIRST before using it
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed for display"));
    for (;;);
  }

  // Show loader with glitch effect and logo
  loader::show(display, XRPL_Logo, 128, 64);   //Xahau_Logo

  // Connect to WiFi and WebSocket
  WiFiConnection::connect(display);
  WebSocketConnection::begin(display, websocket_host, websocket_port, websocket_url, handleWebSocketMessage);
}

void loop() {

  // Handle button press for reset
  handleReset();

  // If in "off" mode, skip all other operations
    if (isOff) {
        return;
    }

  // Initialize the WebSocket connection
  WebSocketConnection::loop();
    
  // WiFi and WebSocket connection handling
  unsigned long now = millis();

  if (!WiFiConnection::isConnected()) {
    handleWiFiConnectionDisplay();
  } else {
    if (!WebSocketConnection::isConnected()) {
      handleWebSocketConnectionDisplay();
    }
  }

  // Check for high latency and trigger alert with timeout
  if (ioLatencyMs > LATENCY_THRESHOLD && (now - lastAlertTime > ALERT_TIMEOUT)) {
    Serial.println("High IO latency detected, triggering alert");
    alertLat(ioLatencyMs);
  }

  // Update regular display data periodically (only when connected)
  if (WiFiConnection::isConnected() && WebSocketConnection::isConnected() &&
      now - lastDisplayUpdate > displayInterval) {
    lastDisplayUpdate = now;
    displayPage = (displayPage + 1) % 2;
    displayData();
  }
}


// Function to display data on the OLED screen
void displayData() {
  // Only update the displays if the uptime has changed
  if (uptime != previousUptime) {
    // Clear the display before drawing new content
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    
    switch (displayPage) {
      case 0:
      // Display server info: host ID, build version, and complete ledgers
      display.setTextSize(1);
      printCentered(websocket_host, 3);
      
      printCentered(("Host ID: " + hostID).c_str(), 16);
      
      printCentered("Build Version:", 28);
      printCentered(buildVersion.c_str(), 38);
      
      printCentered(("Index:" + completeLedgers).c_str(), 50);
      
      break;
      
      case 1:
      // Display server info: uptime, I/O latency, load factor, and peers
      display.setTextSize(1);
      
      printCentered(websocket_host, 3);
      
      char buffUp[32];
      sprintf(buffUp, "Uptime %ds", uptime);
      printCentered(buffUp, 16);
      
      char buffLat[32];
      sprintf(buffLat, "I/O Latency: %d ms", ioLatencyMs);
      printCentered(buffLat, 28);
      
      
      char buffLoad[32];
      sprintf(buffLoad, "Load Factor: %.2f", loadFactor); 
      printCentered(buffLoad, 38);
      
      
      char buffPeer[32];
      sprintf(buffPeer, "Peers: %d", peers);
      printCentered(buffPeer, 50);
      
      break;
    }
    
    // Update the display
    display.display();
    
    // Update previous uptime
    previousUptime = uptime;
  }
}

// Function to create an alert for high latency
void alertLat(int latency) {
  unsigned long startTime = millis();
  bool invertState = false;
  
  // Run alert for ALERT_DURATION milliseconds
  while (millis() - startTime < ALERT_DURATION) {
    unsigned long currentTime = millis();
    
    // Toggle haptic feedback every FLASH_INTERVAL
    if ((currentTime - startTime) % FLASH_INTERVAL < FLASH_INTERVAL / 2) {
      digitalWrite(HAPTIC_PIN, HIGH); // Turn on haptic motor
    } else {
      digitalWrite(HAPTIC_PIN, LOW);  // Turn off haptic motor
    }
    
    // Flash/invert display
    display.clearDisplay();
    display.setTextSize(2); 
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(38, 1);
    display.print("HIGH");
    display.setCursor(24, 18);
    display.print("LATENCY");
    display.setCursor(40, 35);
    display.println(hostID);
    display.setCursor(40, 50);
    display.print(latency);
    display.print("ms");
    
    if ((currentTime - startTime) % FLASH_INTERVAL < FLASH_INTERVAL / 2) {
      if (!invertState) {
        display.invertDisplay(true); 
        invertState = true;
      }
    } else {
      if (invertState) {
        display.invertDisplay(false); 
        invertState = false;
      }
    }
    // Display the alert message
    display.display();
    
    delay(50); 
  }
  
  // Ensure haptic motor is off and display is normal after alert
  digitalWrite(HAPTIC_PIN, LOW);
  display.invertDisplay(false);
  display.clearDisplay();
  
  // Update the last alert time
  lastAlertTime = millis();
  displayData();
}

// Function to print text centered on the display
void printCentered(const char* text, int y) {
  int16_t textWidth = strlen(text) * 6; 
  int16_t x = (128 - textWidth) / 2; 
  display.setCursor(x, y);
  display.print(text);
}

// Handle button press for reset and low power mode.
void handleReset() {
    static unsigned long lastWakeTime = 0;
    unsigned long currentTime = millis();

    // Prevent button handling for 10 seconds after last action
    if (currentTime - lastWakeTime < 10000) {
        return;
    }

    bool reading = digitalRead(BUTTON_PIN);

    // Check if the button state has changed
    if (reading != buttonState && (currentTime - lastDebounceTime) > DEBOUNCE_DELAY) {
        lastDebounceTime = currentTime;
        buttonState = reading;

        if (buttonState == LOW) {
            unsigned long startTime = millis();

            while (digitalRead(BUTTON_PIN) == LOW) {
                delay(10);
            }
            unsigned long duration = millis() - startTime;

            // Short press detected
            if (duration < LONG_PRESS_TIME) {
                Serial.println("Short press - Restarting");
                Serial.flush();

                display.clearDisplay();
                display.setTextSize(1);
                display.setCursor(0, 0);
                display.print("Restarting device");
                display.display();
                delay(2000);
                ESP.restart();

            } else {
                // Long press detected
                if (!isOff) {
                  // First long press: Turn off components
                  Serial.println("Long press - Turning off components");
                  Serial.flush();
                  
                  display.clearDisplay();
                  display.setTextSize(1);
                  display.setCursor(0, 0);
                  display.print("Entering sleep mode");
                  display.display();
                  delay(2000);

                   // Disconnect from the Websocket
                  Serial.println("Disconneting websocket");
                  WebSocketConnection::disconnect();

                  // Disconnect WiFi and turn off WiFi modem
                  Serial.println("Disconneting WiFi");
                  WiFi.disconnect();
                  WiFi.mode(WIFI_OFF);

                  // Turn off the display
                  Serial.println("Turning off the display");
                  display.ssd1306_command(SSD1306_DISPLAYOFF);

                  isOff = true;
                  lastWakeTime = millis();
                } else {

                  // Second long press: Restart device
                  Serial.println("Long press - Restarting device");
                  Serial.flush();

                  display.clearDisplay();
                  display.setTextSize(1);
                  display.setCursor(0, 0);
                  display.print("Restarting device");
                  display.display();
                  delay(2000);
                  ESP.restart();
                }
            }
        }
    }
}

// Function to handle WiFi connection display
void handleWiFiConnectionDisplay() {
  unsigned long now = millis();
  static unsigned long lastWiFiAttempt = 0;
  if (now - lastWiFiAttempt > 5000) {
    lastWiFiAttempt = now;
    Serial.println("Connecting to WiFi...");
  }
}

// Function to handle WebSocket connection display
void handleWebSocketConnectionDisplay() {
  unsigned long now = millis();
  static unsigned long lastWebSocketDisplay = 0;
  if (now - lastReconnectAttempt > reconnectInterval) {
    lastReconnectAttempt = now;
    Serial.println("Connecting to WebSocket...");
  }
  if (now - lastWebSocketDisplay > 5000) {
    lastWebSocketDisplay = now;
    Serial.println("Connecting to WebSocket...");
  }
}


