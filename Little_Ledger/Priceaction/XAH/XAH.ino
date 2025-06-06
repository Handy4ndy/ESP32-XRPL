#include <WiFi.h>
#include <WebSocketsClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <cfloat> // Include cfloat to use DBL_MAX

// XRPL WebSocket server address
const char* websocket_host = "s2.ripple.com";
const uint16_t websocket_port = 443;
const char* websocket_url = "/";

// Replace with your network credentials
const char* ssid = "YOUR_NETWORK_SSID";
const char* password = "YOUR_NETWORK_PASSWORD";

// XRP/RLUSD pair (for conversion)
const char* baseCurrencyXRP = "XRP";
const char* quoteCurrencyRLUSD = "524C555344000000000000000000000000000000"; // RLUSD HEX
const char* quoteIssuerRLUSD = "rMxCKbEDwqr76QuheSUMdEGf4B9xJ8m5De";

// XAH/XRP pair
const char* baseCurrencyXAH = "XAH";
const char* quoteCurrencyXRP = "XRP";
const char* baseIssuerXAH = "rswh1fvyLqHizBS2awu1vs6QcmwTBd9qiv";

// WebSocket client
WebSocketsClient webSocket;

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Preferences for storing high score and 5-minute high/low prices
Preferences preferences;

double xrpPriceRLUSD = 0.0; // Store XRP/RLUSD price
double xahPriceXRP = 0.0;   // Store XAH/XRP price
double lastPrice = 0.0;     // XAH/RLUSD price
double highScore = 0.0;
double lowPrice = DBL_MAX;
double highPrice = 0.0;
double previousLowPrice = 0.0;
double previousHighPrice = 0.0;
double previousPercentageMove = 0.0;
unsigned long windowStartTime = 0;
const unsigned long windowDuration = 300000; // 5 minutes in milliseconds

const int redLedPin = 1;
const int greenLedPin = 2;

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 20000; // 20 seconds

int ledgerIndex = 0;

void setup() {
  // Initialize LED pins
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW);

  // Initialize Serial
  Serial.begin(9600);

  // Initialize WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  // Initialize WebSocket
  webSocket.beginSSL(websocket_host, websocket_port, websocket_url);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(reconnectInterval);

  // Initialize Preferences
  preferences.begin("priceInfo", false);

  // Retrieve stored values
  highScore = preferences.getDouble("highScore", 0.0);
  // Reset highScore if unrealistically large (e.g., from previous unscaled runs)
  if (highScore > 1.0) { // XAH/RLUSD should be ~0.05203
    highScore = 0.0;
    preferences.putDouble("highScore", highScore);
    Serial.println("Reset highScore to 0.0 due to invalid stored value");
  }
  Serial.printf("Initial highScore: %f RLUSD\n", highScore); // Debug initial highScore
  lowPrice = preferences.getDouble("lowPrice", DBL_MAX);
  highPrice = preferences.getDouble("highPrice", 0.0);

  // Initialize SSD1306 display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C is the I2C address for the SSD1306
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  // Initialize window start time
  windowStartTime = millis();
}

void loop() {
  webSocket.loop();

  if (!webSocket.isConnected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = now;
      webSocket.beginSSL(websocket_host, websocket_port, websocket_url);
    }
  }

  // Check if the 5-minute window has elapsed
  if (millis() - windowStartTime >= windowDuration) {
    // Calculate the percentage move
    double percentageMove = lowPrice != DBL_MAX ? ((highPrice - lowPrice) / lowPrice) * 100 : 0.0;
    percentageMove = round(percentageMove * 100) / 100; // Round to 2 decimal places
    Serial.printf("5-minute window: Low Price: %f, High Price: %f, Percentage Move: %.2f%%\n", lowPrice, highPrice, percentageMove);

    // Store the previous 5-minute high, low, and percentage move
    previousLowPrice = lowPrice;
    previousHighPrice = highPrice;
    previousPercentageMove = percentageMove;

    // Reset the window start time and low/high prices
    windowStartTime = millis();
    lowPrice = DBL_MAX;
    highPrice = 0.0;

    // Save the low and high prices for the next window
    preferences.putDouble("lowPrice", lowPrice);
    preferences.putDouble("highPrice", highPrice);
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket Disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket Connected");
      // Subscribe to the ledger stream
      webSocket.sendTXT("{\"command\": \"subscribe\", \"streams\": [\"ledger\"]}");
      break;
    case WStype_TEXT:
      // Parse the incoming message
      String message = String((char*)payload);

      // Parse JSON message
      DynamicJsonDocument doc(4096);
      DeserializationError error = deserializeJson(doc, message);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      // Check if the message contains ledger updates
      if (doc.containsKey("ledger_index")) {
        ledgerIndex = doc["ledger_index"];
        Serial.printf("Ledger Index: %d\n", ledgerIndex);

        // Query the order books
        queryOrderBook();
      }

      // Check if the message contains book offers
      if (doc.containsKey("result") && doc["result"].containsKey("offers")) {
        // Check for id to distinguish between XRP/RLUSD and XAH/XRP
        String queryId = doc["id"].as<String>();
        processBookOffers(doc["result"]["offers"].as<JsonArray>(), queryId);
      }
      break;
  }
}

void queryOrderBook() {
  // Query XRP/RLUSD
  String xrpQuery = "{\"id\": \"xrp_rlusd\", \"command\": \"book_offers\", \"taker_gets\": {\"currency\": \"" + String(baseCurrencyXRP) + "\"}, \"taker_pays\": {\"currency\": \"" + String(quoteCurrencyRLUSD) + "\", \"issuer\": \"" + String(quoteIssuerRLUSD) + "\"}, \"limit\": 1}";
  webSocket.sendTXT(xrpQuery);
  // Query XAH/XRP
  String xahQuery = "{\"id\": \"xah_xrp\", \"command\": \"book_offers\", \"taker_gets\": {\"currency\": \"" + String(baseCurrencyXAH) + "\", \"issuer\": \"" + String(baseIssuerXAH) + "\"}, \"taker_pays\": {\"currency\": \"" + String(quoteCurrencyXRP) + "\"}, \"limit\": 1}";
  webSocket.sendTXT(xahQuery);
}

void processBookOffers(JsonArray offers, String queryId) {
  if (offers.size() > 0) {
    JsonObject offer = offers[0];
    if (offer.containsKey("quality")) {
      String quality = offer["quality"].as<String>();
      Serial.printf("Raw quality for %s: %s\n", queryId.c_str(), quality.c_str()); // Debug quality
      double price = atof(quality.c_str());
      if (queryId == "xrp_rlusd") {
        price *= 1000000; // Scale for XRP/RLUSD
        xrpPriceRLUSD = price;
        Serial.printf("XRP Price: %f RLUSD\n", xrpPriceRLUSD);
      } else if (queryId == "xah_xrp") {
        xahPriceXRP = price / 1000000; // Scale down for XAH/XRP
        if (xrpPriceRLUSD != 0.0) {
          lastPrice = xahPriceXRP * xrpPriceRLUSD; // Calculate XAH/RLUSD
          Serial.printf("XAH Price: %f XRP, %f RLUSD\n", xahPriceXRP, lastPrice);
          priceAction(lastPrice);
        } else {
          Serial.println("Skipping XAH/RLUSD calculation: xrpPriceRLUSD not set");
        }
      }
    }
  } else {
    Serial.printf("No offers found for %s\n", queryId.c_str());
  }
  
  updateDisplay();
}

void priceAction(double price) {
  // Validate price before processing
  if (price <= 0.0) {
    Serial.printf("Invalid price: %f, skipping highScore update\n", price);
    return;
  }

  // Update high score
  Serial.printf("Comparing price: %f with highScore: %f\n", price, highScore); // Debug comparison
  if (price > highScore) {
    highScore = price;
    preferences.putDouble("highScore", highScore);
    Serial.printf("New XAH/RLUSD High Score: %f RLUSD\n", highScore);
  }

  // Update low and high prices within the 5-minute window
  if (price < lowPrice) {
    lowPrice = price;
    Serial.printf("New Low logged: %f RLUSD\n", lowPrice);
  }
  if (price > highPrice) {
    highPrice = price;
    Serial.printf("New high logged: %f RLUSD\n", highPrice);
  }

  // Update LEDs based on price action
  if (price > lastPrice && lastPrice != 0.0) {
    // Price increased
    Serial.printf("The price has increased\n");
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(redLedPin, LOW);
  } else if (price < lastPrice && lastPrice != 0.0) {
    // Price decreased
    Serial.printf("The price has decreased\n");
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
  } else {
    Serial.printf("No change found\n");
  }

  // Update lastPrice only if it's a valid update
  if (price != 0.0) {
    lastPrice = price;
  }
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(10, 0);
  display.printf("$%.6f", lastPrice);

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