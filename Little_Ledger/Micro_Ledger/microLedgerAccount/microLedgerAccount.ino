#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <Preferences.h>
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
#define BUTTON_PIN 9          // GPIO pin connected to the button (must be RTC capable)

// Variables for button handling
#define SHORT_PRESS_TIME 2000  // Short press threshold (< 2s)
#define LONG_PRESS_TIME 5000   // Long press threshold (> 5s)
#define DEBOUNCE_DELAY 1000    // Debounce delay in milliseconds
static unsigned long lastDebounceTime = 0; // Timestamp of the last button state change
static bool buttonState = HIGH;            // Current stable state of the button

// Variables for WiFi connection
unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000; // 5 seconds

// Preferences for storing high score
Preferences preferences;

// Variables for transaction tracking
int ledgerIndex = 0;       // Current ledger index
int transactionCount = 0;  // Current transaction count
int highScore = 0;         // Highest transaction count
int highScoreLedger = 0;   // Ledger index of highest transaction count
int previousTransactionCount = -1; // Previous transaction count for display updates

// Custom variable for wallet address
const char* walletAddress = "YOUR_WALLET_RADDRESS"; // XRPL account to monitor

// WebSocket server address
const char* websocket_host = "s2.ripple.com";  // XRPL - "s2.ripple.com", "xrplcluster.com" || Xahau - "xahau.network", "xahau.org"
const uint16_t websocket_port = 443;
const char* websocket_url = "/";

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Flag to track account subscription status
static bool accountSubscribed = false;

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
  if (doc.containsKey("type") && String(doc["type"]) == "ledgerClosed") {
    ledgerIndex = doc["ledger_index"].as<int>();
    transactionCount = doc["txn_count"].as<int>();
    Serial.print("Ledger Sequence: ");
    Serial.println(ledgerIndex);
    Serial.print("Total Transactions: ");
    Serial.println(transactionCount);
    updateHighScore(transactionCount, ledgerIndex); // Update high score
    displayData(); // Update the OLED display
  }

  // Process account transactions
  processTransaction(payload, length);
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
  preferences.begin("ledgerPrefs", false);

  // Retrieve stored values
  highScore = preferences.getInt("highScore", 0);
  highScoreLedger = preferences.getInt("highScoreLedger", 0);

  // Show loader with glitch effect and logo
  loader::show(display, XRPL_Logo, 128, 64);

  // Connect to WiFi and WebSocket
  WiFiConnection::connect(display);
  WebSocketConnection::begin(display, websocket_host, websocket_port, websocket_url, handleWebSocketMessage);
}

void loop() {
  // Handle button press for high score, reset, or power off
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
      accountSubscribed = false; // Reset subscription flag on disconnection
    } else {
      // Subscribe to account stream if not already subscribed
      if (!accountSubscribed) {
        subscribeToAccount(walletAddress);
        accountSubscribed = true;
      }
    }
  }
}

// Function to update high score
void updateHighScore(int transactionCount, int ledgerIndex) {
  if (transactionCount > highScore) {
    highScore = transactionCount;
    highScoreLedger = ledgerIndex;
    preferences.begin("ledgerPrefs", false);
    preferences.putInt("highScore", highScore);
    preferences.putInt("highScoreLedger", highScoreLedger);
    preferences.end();
    Serial.printf("New High Score: %d transactions at Ledger %d\n", highScore, highScoreLedger);
    // Trigger haptic feedback for high score
    performHapticAction("HighScore", true);
  }
}

// Function to display data on the OLED screen
void displayData() {
  if (transactionCount != previousTransactionCount) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    
    // Display ledger sequence at the top
    display.setCursor(17, 0);
    display.print("Index: ");
    display.println(ledgerIndex);
    
    // Display transaction count and high score
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.print("Txns: ");
    display.print(transactionCount);
    display.setCursor(80, 16);
    display.print("HS: ");
    display.println(highScore);
    
    // Set square size based on transaction count
    int squareSize = (transactionCount > 500) ? 1 : 2;
    int margin = (transactionCount > 1250) ? 0 : 1;
    int topMargin = 30; // Adjusted to leave space for text
    
    // Display 2x2 or 1x1 pixel squares representing transaction count
    int numSquares = min(transactionCount, 100); // Limit to avoid overflow
    for (int i = 0; i < numSquares; i++) {
      int x = (i * (squareSize + margin)) % SCREEN_WIDTH;
      int y = SCREEN_HEIGHT - squareSize - ((i * (squareSize + margin)) / SCREEN_WIDTH) * (squareSize + margin);
      if (y >= topMargin) {
        display.fillRect(x, y, squareSize, squareSize, SSD1306_WHITE);
      }
    }
    
    display.display();
    
    // Update the previous transaction count
    previousTransactionCount = transactionCount;
  }
}

// Function to subscribe to an account
void subscribeToAccount(const char* account) {
  DynamicJsonDocument doc(256);
  doc["command"] = "subscribe";
  JsonArray accounts = doc.createNestedArray("accounts");
  accounts.add(account);
  
  String message;
  serializeJson(doc, message);
  WebSocketConnection::send(message.c_str());
  Serial.println("Subscribed to account: " + String(account));
}

// Function to get the amount value from a transaction
const char* getAmountValue(JsonVariant transaction) {
    static char buffer[32]; 
    if (transaction.is<JsonObject>()) {
        // Non-XRP (issued currency): return the "value" field directly
        return transaction["value"];
    } else if (transaction.is<const char*>()) {
        // XRP in drops: convert to XRP
        const char* dropsStr = transaction.as<const char*>();
        double drops = atof(dropsStr); 
        double xah = drops / 1000000.0; 
        snprintf(buffer, sizeof(buffer), "%.6f", xah); 
        return buffer;
    } else {
        // Error case: unrecognized format
        Serial.println("Amount format not recognized");
        return nullptr;
    }
}

// Function to process transactions and handle haptic alerts
void processTransaction(const uint8_t* payload, size_t length) {
  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  
  if (!doc.containsKey("transaction")) {
    Serial.println("No transaction found");
    return;
  }
  
  JsonObject transaction = doc["transaction"];
  const char* transactionType = transaction["TransactionType"];
  const char* account = transaction["Account"];
  const char* destination = transaction["Destination"];

  if (transactionType == nullptr || account == nullptr) {
    Serial.println("TransactionType or Account is null");
    return;
  }

  // Payment notification
  if (strcmp(transactionType, "Payment") == 0) {
    if (strcmp(account, walletAddress) == 0) {
      // Outgoing payment
      const char* amountValue = getAmountValue(transaction["Amount"]);
      if (amountValue != nullptr) {
        Serial.printf("Payment sent from %s of amount %s\n", walletAddress, amountValue);
        performHapticAction("Payment", true);
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(30, 2);
        display.print("Payment Sent");
        display.setTextSize(1);
        display.setCursor(0, 16);
        display.print("Destination:");
        display.setCursor(0, 27);
        display.print(destination);
        display.setCursor(0, 44);
        display.print("Amount:");
        display.setCursor(0, 54);
        display.print(amountValue);
        display.display();
        delay(10000);
        display.clearDisplay();
        display.display();
      }
    } else if (strcmp(destination, walletAddress) == 0) {
      // Incoming payment
      const char* amountValue = getAmountValue(transaction["Amount"]);
      if (amountValue != nullptr) {
        Serial.printf("Payment received to %s of amount %s\n", walletAddress, amountValue);
        performHapticAction("Payment", false);
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(2, 2);
        display.print("Payment Received");
        display.setTextSize(1);
        display.setCursor(0, 16);
        display.print("From:");
        display.setCursor(0, 25);
        display.print(destination);
        display.setCursor(0, 44);
        display.print("Amount:");
        display.setCursor(0, 54);
        display.print(amountValue);
        display.display();
        delay(10000);
        display.clearDisplay();
        display.display();
      }
    }
  } 
  // TrustSet notification
  else if (strcmp(transactionType, "TrustSet") == 0) {
    Serial.println("TrustSet transaction detected");
    const char* limitAmountValue = getAmountValue(transaction["LimitAmount"]);
    if (limitAmountValue != nullptr && strcmp(limitAmountValue, "0") == 0) {
      // Trust line removal
      performHapticAction("TrustSet", false);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(2, 2);
      display.print("Trustline Removed");
      display.setTextSize(1);
      display.setCursor(0, 16);
      display.print("Trustline removed from:");
      display.setCursor(0, 35);
      display.print(walletAddress);
      display.display();
      delay(10000);
      display.clearDisplay();
      display.display();
    } else {
      // Trust line addition
      performHapticAction("TrustSet", true);
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(6, 2);
      display.print("Trustline Set");
      display.setTextSize(1);
      display.setCursor(0, 16);
      display.print("Trustline set on:");
      display.setCursor(0, 35);
      display.print(walletAddress);
      display.display();
      delay(10000);
      display.clearDisplay();
      display.display();
    }
  } 
  // OfferCreate notification
  else if (strcmp(transactionType, "OfferCreate") == 0) {
    Serial.println("OfferCreate transaction detected");
    performHapticAction("OfferCreate", true);
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(2, 2);
    display.print("Offer Create");
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.print("Offer created by:");
    display.setCursor(0, 35);
    display.print(walletAddress);
    display.display();
    delay(10000);
    display.clearDisplay();
    display.display();
  } 
  // OfferCancel notification
  else if (strcmp(transactionType, "OfferCancel") == 0) {
    Serial.println("OfferCancel transaction detected");
    performHapticAction("OfferCancel", false);
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(2, 2);
    display.print("Offer Cancel");
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.print("Offer canceled by:");
    display.setCursor(0, 35);
    display.print(walletAddress);
    display.display();
    delay(10000);
    display.clearDisplay();
    display.display();
  } else {
    Serial.printf("Unhandled transaction type: %s\n", transactionType);
  }
}


// Function to perform haptic feedback based on transaction type and positivity
void performHapticAction(const char* transactionType, bool positive) {
  Serial.printf("%s transaction %s! Performing haptic action...\n", transactionType, positive ? "positive" : "negative");
  
  // Define haptic patterns: duration (ms) and pause (ms)
  int positivePattern[] = {200, 100, 200, 100, 200}; // Three short pulses
  int negativePattern[] = {500, 200, 500};           // Two longer pulses
  int patternSize = positive ? 5 : 3;

  int* pattern = positive ? positivePattern : negativePattern;
  
  for (int i = 0; i < patternSize; i += 2) {
    digitalWrite(HAPTIC_PIN, HIGH);
    delay(pattern[i]);
    digitalWrite(HAPTIC_PIN, LOW);
    if (i + 1 < patternSize) {
      delay(pattern[i + 1]);
    }
  }
}

// Function to display the high score on the OLED display
void displayHighScore() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(5, 0);
  display.print("High Score");
  display.setTextSize(1);
  display.setCursor(5, 27);
  display.print("Transactions: ");
  display.println(highScore);
  display.setCursor(5, 36);
  display.print("Index: ");
  display.println(highScoreLedger);
  display.display();
  
  // Trigger haptic feedback for high score
  performHapticAction("HighScore", true);

  Serial.print("Displayed High Score: ");
  Serial.println(highScore);
  Serial.print("Displayed Ledger: ");
  Serial.println(highScoreLedger);
  
  delay(5000); // Show for 5 seconds
  display.clearDisplay();
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

// Handle button press for high score, reset, or power off
void handleReset() {
  static unsigned long lastWakeTime = 0;
  unsigned long currentTime = millis();

  // Prevent button handling for 3 seconds after last action
  if (currentTime - lastWakeTime < 3000) {
    return;
  }

  bool reading = digitalRead(BUTTON_PIN);

  if (reading != buttonState && (currentTime - lastDebounceTime) > DEBOUNCE_DELAY) {
    lastDebounceTime = currentTime;
    buttonState = reading;

    if (buttonState == LOW) {
      unsigned long startTime = millis();

      while (digitalRead(BUTTON_PIN) == LOW) {
        delay(10);
      }
      unsigned long duration = millis() - startTime;

      if (duration < SHORT_PRESS_TIME) {
        // Short press: Display high score
        Serial.println("Short press - Displaying high score");
        Serial.flush();

        displayHighScore();
        lastWakeTime = millis();

      } else if (duration >= SHORT_PRESS_TIME && duration <= LONG_PRESS_TIME) {
        // Medium press: Reset device
        Serial.println("Medium press - Resetting device");
        Serial.flush();

        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("Resetting device...");
        display.display();
        delay(2000);
        ESP.restart();

      } else {
        // Long press: Toggle power state
        if (!isOff) {
          Serial.println("Long press - Turning off components");
          Serial.flush();

          display.clearDisplay();
          display.setTextSize(1);
          display.setCursor(0, 0);
          display.print("Entering Sleep Mode");
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
