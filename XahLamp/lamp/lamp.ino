#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <WiFi.h>
#include <FastLED.h>
#include "pitches.h"

// Define speaker pin
#define SPEAKER_PIN 9

// FastLED configuration
#define LED_PIN 7        // Data pin for LED strip
#define NUM_LEDS 40      // Number of LEDs
#define LED_TYPE WS2812B // LED strip type
#define COLOR_ORDER RGB  // Color order for WS2812B
CRGB leds[NUM_LEDS];     // Array to store LED colors

// Replace with your network credentials
const char* ssid = "YOUR_NETWORK_SSID";
const char* password = "YOUR_NETWORK_PASSWORD"; 

// XRPL WebSocket server address
const char* websocket_host = "xahau.network"; // XRPL - "s2.ripple.com", "xrplcluster.com" || Xahau - "xahau.network", "xahau.org"
const uint16_t websocket_port = 443;
const char* websocket_url = "/";

// Variables to store ledger sequence and transaction count
int ledgerSequence = 0;
int transactionCount = 0;
unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 4000; // Interval to attempt WebSocket reconnection
int previousTransactionCount = -1;

// WebSocket client
WebSocketsClient webSocket;

// Custom variable for wallet address
const char* walletAddress = "YOUR_WALLET_RADDRESS"; // Replace with your wallet address

// Function to show a rainbow effect for 3 seconds
void showRainbowEffect() {
  uint8_t initialHue = 0;
  const uint8_t deltaHue = 255 / NUM_LEDS; // Spread hues across LEDs
  const int duration = 5000; // 3 seconds
  const int frameDelay = 5; // 50ms per frame for smooth animation
  const int numFrames = duration / frameDelay;

  for (int frame = 0; frame < numFrames; frame++) {
    fill_rainbow(leds, NUM_LEDS, initialHue++, deltaHue); // Cycle hues
    FastLED.show();
    delay(frameDelay);
  }
}

void setup() {
  // Initialize pins
  pinMode(SPEAKER_PIN, OUTPUT);
  
  // Initialize FastLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(255); // Set brightness (0-255, adjust as needed)
  
  // Test LED strip
  Serial.begin(9600);
  Serial.println("Starting WebSocket program...");
  Serial.println("Testing LED strip...");
  fill_solid(leds, NUM_LEDS, CRGB::Red);   // Red
  FastLED.show();
  delay(1000);
  fill_solid(leds, NUM_LEDS, CRGB::Green); // Green
  FastLED.show();
  delay(1000);
  fill_solid(leds, NUM_LEDS, CRGB::Blue);  // Blue
  FastLED.show();
  delay(1000);
  showRainbowEffect();
  Serial.println("LED strip test complete");
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    showRainbowEffect();
    Serial.println("Connecting to WiFi...");
  }
  
  // Log WiFi connection status
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  showRainbowEffect();
  
  // Initialize WebSocket
  Serial.println("Connecting to WebSocket server...");
  webSocket.beginSSL(websocket_host, websocket_port, websocket_url);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(reconnectInterval);

}

void loop() {
  
  webSocket.loop();
  
  if (millis() - lastReconnectAttempt >= reconnectInterval) {
    lastReconnectAttempt = millis();
    // Check Wi-Fi connection and reconnect if necessary
    checkWiFiConnection();
    checkWebSocketConnection();
  }
}

// WebSocket event handler
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket Disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket Connected");
      // Subscribe to ledger stream
      webSocket.sendTXT("{\"command\": \"subscribe\", \"streams\": [\"ledger\"]}");
      // Subscribe to account
      subscribeToAccount(walletAddress);
      break;
    case WStype_TEXT:
      // Parse the incoming message
      String message = String((char*)payload);
      Serial.print("Update received from: ");
      Serial.println(websocket_host);
      
      // Parse JSON message
      DynamicJsonDocument doc(4096);
      DeserializationError error = deserializeJson(doc, message);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      
      // Check if it's a ledger closed message
      if (doc.containsKey("type") && String(doc["type"]) == "ledgerClosed") {
        ledgerSequence = doc["ledger_index"];
        transactionCount = doc["txn_count"];
        
        Serial.print("Ledger Sequence: ");
        Serial.println(ledgerSequence);
        Serial.print("Total Transactions: ");
        Serial.println(transactionCount);
        updateTransactionCountLED();
      }
      
      processTransaction(payload, length); // Process transactions
      break;
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
  webSocket.sendTXT(message);
}

// Function to process transactions and handle alerts
void processTransaction(uint8_t* payload, size_t length) {
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

  // Show rainbow effect before transaction-specific color
  Serial.println("Displaying rainbow effect for transaction");
  showRainbowEffect();

  // Payment notification: send = negative (red), received = positive (green)
  if (strcmp(transactionType, "Payment") == 0) {
    if (strcmp(account, walletAddress) == 0) {
      // Outgoing payment
      const char* amountValue = getAmountValue(transaction["Amount"]);
      if (amountValue != nullptr) {
        Serial.printf("Payment sent from %s of amount %s\n", walletAddress, amountValue);
        performAction("Payment", false);
        fill_solid(leds, NUM_LEDS, CRGB::Red); // Red
        FastLED.show();
        Serial.println("LED strip set to Red");
        delay(2000);
      }
    } else if (strcmp(destination, walletAddress) == 0) {
      // Incoming payment
      const char* amountValue = getAmountValue(transaction["Amount"]);
      if (amountValue != nullptr) {
        Serial.printf("Payment received to %s of amount %s\n", walletAddress, amountValue);
        fill_solid(leds, NUM_LEDS, CRGB::Green); // Green
        FastLED.show();
        Serial.println("LED strip set to Green");
        performAction("Payment", true);
        delay(2000);
      }
    }
  } 
  // TrustSet notification: Limit == 0 / negative (red), Limit >= 1 positive (green)
  else if (strcmp(transactionType, "TrustSet") == 0) {
    Serial.println("TrustSet transaction detected");
    const char* limitAmountValue = getAmountValue(transaction["LimitAmount"]);
    if (limitAmountValue != nullptr && strcmp(limitAmountValue, "0") == 0) {
      // Trust line removal
      performAction(transactionType, false);
      fill_solid(leds, NUM_LEDS, CRGB::Red); // Red
      FastLED.show();
      Serial.println("LED strip set to Red");
      delay(2000);
    } else {
      // Trust line addition
      performAction(transactionType, true);
      fill_solid(leds, NUM_LEDS, CRGB::Green); // Green
      FastLED.show();
      Serial.println("LED strip set to Green");
      delay(2000);
    }
  } 
  // OfferCreate notification: positive (green)
  else if (strcmp(transactionType, "OfferCreate") == 0) {
    Serial.println("OfferCreate transaction detected");
    performAction(transactionType, true);
    fill_solid(leds, NUM_LEDS, CRGB::Green); // Green
    FastLED.show();
    Serial.println("LED strip set to Green");
    delay(2000);
  } 
  // OfferCancel notification: negative (red)
  else if (strcmp(transactionType, "OfferCancel") == 0) {
    Serial.println("OfferCancel transaction detected");
    performAction(transactionType, false);
    fill_solid(leds, NUM_LEDS, CRGB::Red); // Red
    FastLED.show();
    Serial.println("LED strip set to Red");
    delay(2000);
  } else {
    Serial.printf("Unhandled transaction type: %s\n", transactionType);
  }
}

// Function to get the amount value from the transaction
const char* getAmountValue(JsonVariant transaction) {
  if (transaction.is<JsonObject>()) {
    return transaction["value"];
  } else if (transaction.is<const char*>()) {
    return transaction.as<const char*>();
  } else {
    Serial.println("Amount format not recognized");
    return nullptr;
  }
}

// Function to update LED strip based on transaction count
void updateTransactionCountLED() {
  if (transactionCount != previousTransactionCount) {
    Serial.print("Updating LED strip for transaction count: ");
    Serial.println(transactionCount);
    if (transactionCount <= 20) {
      fill_solid(leds, NUM_LEDS, CRGB::Blue); // Blue
      Serial.println("LED strip set to Blue");
    } else if (transactionCount < 150) {
      fill_solid(leds, NUM_LEDS, CRGB::Green); // Green
      Serial.println("LED strip set to Green");
    } else {
      fill_solid(leds, NUM_LEDS, CRGB::Red); // Red
      Serial.println("LED strip set to Red");
    }
    FastLED.show();
    previousTransactionCount = transactionCount;
  }
}

// Function to perform an action based on the transaction type and positivity
void performAction(const char* transactionType, bool positive) {
  Serial.printf("%s transaction %s! Performing action...\n", transactionType, positive ? "positive" : "negative");
  Serial.println(positive ? "LED strip set to Green (Positive)" : "LED strip set to Red (Negative)");
  
  // Define melodies for different transaction types
  int paymentPositiveMelody[] = { NOTE_E6, NOTE_C6, NOTE_A5, NOTE_F5, NOTE_D5, NOTE_B4, NOTE_G4 };
  int paymentNegativeMelody[] = { NOTE_G4, NOTE_B4, NOTE_D5, NOTE_F5, NOTE_A5, NOTE_C6, NOTE_E6 };
  int trustSetPositiveMelody[] = { NOTE_C6, NOTE_D6, NOTE_E6, NOTE_F6, NOTE_G6, NOTE_A6, NOTE_B6 };
  int trustSetNegativeMelody[] = { NOTE_B6, NOTE_A6, NOTE_G6, NOTE_F6, NOTE_E6, NOTE_D6, NOTE_C6 };
  int offerCreatePositiveMelody[] = { NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5 };
  int offerCreateNegativeMelody[] = { NOTE_F5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4 };

  int* melody;
  int melodySize;

  // Select the appropriate melody based on transaction type and positivity
  if (strcmp(transactionType, "Payment") == 0) {
    melody = positive ? paymentPositiveMelody : paymentNegativeMelody;
    melodySize = sizeof(paymentPositiveMelody) / sizeof(paymentPositiveMelody[0]);
  } else if (strcmp(transactionType, "TrustSet") == 0) {
    melody = positive ? trustSetPositiveMelody : trustSetNegativeMelody;
    melodySize = sizeof(trustSetPositiveMelody) / sizeof(trustSetPositiveMelody[0]);
  } else if (strcmp(transactionType, "OfferCreate") == 0) {
    melody = positive ? offerCreatePositiveMelody : offerCreateNegativeMelody;
    melodySize = sizeof(offerCreatePositiveMelody) / sizeof(offerCreatePositiveMelody[0]);
  } else if (strcmp(transactionType, "OfferCancel") == 0) {
    melody = positive ? offerCreateNegativeMelody : offerCreatePositiveMelody;
    melodySize = sizeof(offerCreateNegativeMelody) / sizeof(offerCreateNegativeMelody[0]);
  } else {
    Serial.println("Unknown transaction type");
    return;
  }

  // Play the selected melody
  playMelody(melody, melodySize);
  delay(2000);
  updateTransactionCountLED();
}

// Function to play a melody
void playMelody(int* melody, int size) {
  int noteDurations[] = { 16, 16, 16, 16, 16, 16, 16 }; 
  for (int thisNote = 0; thisNote < size; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(SPEAKER_PIN, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(SPEAKER_PIN);
  }
}

// Function to check WiFi connection status and reconnect if necessary
void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Reconnecting to WiFi...");
    }
    Serial.println("Reconnected to WiFi");
  }
}

// Check WebSocket connection status and reconnect if necessary
void checkWebSocketConnection() {
  if (!webSocket.isConnected()) {
    Serial.println("WebSocket is disconnected. Attempting to reconnect...");
    delay(2000);
    webSocket.beginSSL(websocket_host, websocket_port, websocket_url);
  }
}