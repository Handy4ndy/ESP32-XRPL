#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <esp_sleep.h>
#include <cfloat>
#include "loader.h"
#include "connection.h"
#include "websocket.h"
#include "XRPL_Logo.h"

// Global variable to track "off" state
bool isOff = false;

// Pin definitions
// #define SPEAKER_PIN 7        // GPIO pin for speaker (optional, not used in this example)
#define HAPTIC_PIN 8          // GPIO8 for haptic alerts
#define BUTTON_PIN 9          // GPIO pin connected to the button (must be RTC capable)

// Variables for button handling
#define LONG_PRESS_TIME 2000  // Time in milliseconds to differentiate long and short presses
#define DEBOUNCE_DELAY 1000   // Debounce delay in milliseconds
static unsigned long lastDebounceTime = 0; // Timestamp of the last button state change
static bool buttonState = HIGH;           // Current stable state of the button

// Variables for WiFi connection
unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000; // 5 seconds

// Preferences for storing high score and 5-minute high/low prices
Preferences preferences;

double currentPrice = 0.0; // Current XRP price
double lastPrice = 0.0;    // Previous price for comparison
double highScore = 0.0;    // All-time high price
double lowPrice = DBL_MAX; // Lowest price in current window
double highPrice = 0.0;    // Highest price in current window
double previousLowPrice = 0.0;     // Low from previous window
double previousHighPrice = 0.0;    // High from previous window
double previousPercentageMove = 0.0; // Percentage move from previous window
unsigned long windowStartTime = 0;   // Start time of current 5-minute window
const unsigned long windowDuration = 300000; // 5 minutes in milliseconds
int ledgerIndex = 0;       // Current ledger index

// WebSocket server address
const char* websocket_host = "xrplcluster.com";
const uint16_t websocket_port = 443;
const char* websocket_url = "/";

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Callback function for WebSocket messages
void handleWebSocketMessage(const uint8_t* payload, size_t length) {
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  
  // Check for ledger updates
  if (doc.containsKey("ledger_index")) {
    ledgerIndex = doc["ledger_index"].as<int>();
    Serial.println("Ledger update received, fetching book_offers");
    WebSocketConnection::send("{\"command\": \"book_offers\", \"taker_gets\": {\"currency\": \"XRP\"}, \"taker_pays\": {\"currency\": \"524C555344000000000000000000000000000000\", \"issuer\": \"rMxCKbEDwqr76QuheSUMdEGf4B9xJ8m5De\"}, \"limit\": 1}");
  }
  
  // Check for book_offers response
  if (doc.containsKey("result") && doc["result"].containsKey("offers")) {
    JsonArray offers = doc["result"]["offers"].as<JsonArray>();
    if (offers.size() > 0) {
      JsonObject offer = offers[0];
      if (offer.containsKey("quality")) {
        String quality = offer["quality"].as<String>();
        currentPrice = atof(quality.c_str()) * 1000000; // Convert quality to price (RLUSD per XRP)
        Serial.printf("XRP Price: %f RLUSD\n", currentPrice);
        priceAction(currentPrice); // Update price tracking
        displayData();            // Update the OLED display
      }
    } else {
      Serial.println("No offers found.");
    }
  }
}

void setup() {
  // Initialize the pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(HAPTIC_PIN, OUTPUT);
  digitalWrite(HAPTIC_PIN, LOW);

  // Initialize Serial for debugging
  Serial.begin(9600);
  delay(500);
  
  Serial.println("ESP Starting");

  // Initialize SSD1306 display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed for display"));
    for (;;);
  }

  // Initialize Preferences
  preferences.begin("priceInfo", false);

  // Retrieve stored values
  highScore = preferences.getDouble("highScore", 0.0);
  lowPrice = preferences.getDouble("lowPrice", DBL_MAX);
  highPrice = preferences.getDouble("highPrice", 0.0);

  // Show loader with glitch effect and logo
  loader::show(display, XRPL_Logo, 128, 64);

  // Connect to WiFi and WebSocket
  WiFiConnection::connect(display);
  WebSocketConnection::begin(display, websocket_host, websocket_port, websocket_url, handleWebSocketMessage);

  // Initialize window start time
  windowStartTime = millis();
}

void loop() {
  // Handle button press for reset
  handleReset();

  // If in "off" mode, skip all other operations
  if (isOff) {
    return;
  }

  // Maintain WebSocket connection
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

  // Check if the 5-minute window has elapsed
  if (now - windowStartTime >= windowDuration) {
    // Calculate the percentage move
    double percentageMove = 0.0;
    if (lowPrice != 0.0 && lowPrice != DBL_MAX) {
      percentageMove = ((highPrice - lowPrice) / lowPrice) * 100;
      percentageMove = round(percentageMove * 100) / 100; // Round to 2 decimal places
    }
    Serial.printf("5-minute window: Low Price: %f, High Price: %f, Percentage Move: %.2f%%\n", lowPrice, highPrice, percentageMove);

    // Store the previous 5-minute high, low, and percentage move
    previousLowPrice = lowPrice;
    previousHighPrice = highPrice;
    previousPercentageMove = percentageMove;

    // Reset the window start time and low/high prices
    windowStartTime = now;
    lowPrice = DBL_MAX;
    highPrice = 0.0;

    // Save the low and high prices for the next window
    preferences.putDouble("lowPrice", lowPrice);
    preferences.putDouble("highPrice", highPrice);
  }
}


void priceAction(double price) {
  // Update high score
  if (price > highScore) {
    highScore = price;
    preferences.putDouble("highScore", highScore);
    Serial.printf("New High Score: %f RLUSD\n", highScore);
  }
  
  // Update low and high prices within the 5-minute window
  if (price < lowPrice) {
    lowPrice = price;
    Serial.printf("New Low logged: %f RLUSD\n", lowPrice);
  }
  if (price > highPrice) {
    highPrice = price;
    Serial.printf("New High logged: %f RLUSD\n", highPrice);
  }
  
  // Update lastPrice
  lastPrice = price;
}

// Function to display data on the OLED screen
void displayData() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(10, 0);
  display.printf("$%.6f", currentPrice);
  
  display.setTextSize(1);
  // Display Previous 5-minute High/Low
  display.setCursor(2, 23);
  display.printf("Last 5min H:%.6f", previousHighPrice);
  display.setCursor(2, 33);
  display.printf("Last 5min L:%.6f", previousLowPrice);
  
  // Display Percentage Movement
  display.setCursor(2, 43);
  display.printf("Last 5min %%:%.2f%%", previousPercentageMove);
  
  // Display High Score
  display.setCursor(5, 55);
  display.printf("High Score:%.6f", highScore);
  
  display.display();
}

// //----------------------------------------------------------------TODO----------------------------------------------------------------
// // Function to print text centered on the display
// void printCentered(const char* text, int y) {
//   int16_t textWidth = strlen(text) * 6; 
//   int16_t x = (128 - textWidth) / 2; 
//   display.setCursor(x, y);
//   display.print(text);
// }

// Handle button press for reset and low power mode
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

        Serial.println("Short press - Resetting");
        Serial.flush();

        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("Resetting device");
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
          display.print("Entering Sleep Mode...");
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