// XRPL Trustline monitor created by Handy_4ndy

// To establish a connection with your ESP32 device we must define the network ssid and the password 
// Then set the walletAddress variable with the wallet address you wish to monitor.

#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "pitches.h"

// WiFi network credentials
const char* ssid = "your_network_SSID";      // Insert the network SSID you want to connect to
const char* password = "your_network_password";  // Provide the password for the network

// XRPL wallet Address

const char* walletAddress = "your_account_address"; // Set the wallet address you wish to monitor

// WebSocket server details
const char* wsServer = "s1.ripple.com";
const int wsPort = 51233;

// Pin definitions
const int LED = 5;
const int SPEAKER_PIN = 4;

WebSocketsClient webSocket;

// Function declarations
void subscribeToAccount(const char* account);
void processTransaction(uint8_t* payload, size_t length);
void performAction(bool positive);
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Set pin modes
  pinMode(LED, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);  // Set speaker pin as output

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize WebSocket connection
  webSocket.begin(wsServer, wsPort, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void loop() {
  // Keep WebSocket connection alive
  webSocket.loop();
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  // Handle WebSocket events
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      subscribeToAccount(walletAddress);  // Subscribe to issuers account
      break;
    case WStype_TEXT:
      Serial.printf("WebSocket message: %s\n", payload);
      processTransaction(payload, length);  // Process incoming transaction
      break;
    case WStype_BIN:
      Serial.println("Binary message received");
      break;
    case WStype_PING:
      Serial.println("Ping received");
      break;
    case WStype_PONG:
      Serial.println("Pong received");
      break;
  }
}

void subscribeToAccount(const char* account) {
  // Subscribe to the specified account on the XRPL
  DynamicJsonDocument doc(256);
  doc["command"] = "subscribe";
  JsonArray accounts = doc.createNestedArray("accounts");
  accounts.add(account);

  String message;
  serializeJson(doc, message);
  webSocket.sendTXT(message);
}

void processTransaction(uint8_t* payload, size_t length) {
  // Deserialize incoming JSON transaction data
  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc, payload, length);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Check if transaction data exists
  if (!doc.containsKey("transaction")) {
    Serial.println("No transaction found");
    return;
  }

  JsonObject transaction = doc["transaction"];
  const char* transactionType = transaction["TransactionType"];

  // Ensure TransactionType is valid
  if (transactionType == nullptr) {
    Serial.println("TransactionType is null");
    return;
  }

  // Handle TrustSet transactions
  if (strcmp(transactionType, "TrustSet") == 0) {
    const char* account = transaction["Account"];
    const char* limitAmount = transaction["LimitAmount"]["value"];
    const char* issuer = transaction["LimitAmount"]["issuer"];  // Get the issuer

    // Ensure account, limitAmount, and issuer are valid
    if (account == nullptr || limitAmount == nullptr || issuer == nullptr) {
      Serial.println("Account, LimitAmount, or Issuer is null");
      return;
    }

    Serial.printf("Trustline set by %s with limit amount %s\n", account, limitAmount);

    // Check the issuer and determine if the trustline limit is positive or zero
    if (strcmp(issuer, walletAddress) == 0) {
      bool positive = (atof(limitAmount) > 0);
      performAction(positive);  // Perform action based on the trustline limit
    }
  }
}

void performAction(bool positive) {
  // Indicate a trustline was set
  Serial.println("TrustLine set! Performing action...");

  // Example: Toggle an LED
  digitalWrite(LED, HIGH);  // Turn the LED on
  Serial.println("LED turned on");

  // Play a sound based on the trustline limit
  if (positive) {
    // Play a positive sound
    int melody[] = { NOTE_E6, NOTE_C6, NOTE_A5, NOTE_F5, NOTE_D5, NOTE_B4, NOTE_G4 };
    int noteDurations[] = { 8, 8, 8, 8, 8, 8, 8 };  // eighth notes

    for (int thisNote = 0; thisNote < 7; thisNote++) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(SPEAKER_PIN, melody[thisNote], noteDuration);

      // To distinguish the notes, set a minimum time between them
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(SPEAKER_PIN);  // Stop the tone playing
      Serial.printf("Playing positive note %d: %d\n", thisNote, melody[thisNote]);
    }
  } else {
    // Play a negative sound
    int melody[] = { NOTE_C4, NOTE_B3, NOTE_A3, NOTE_G3, NOTE_F3, NOTE_E3, NOTE_D3 };
    int noteDurations[] = { 8, 8, 8, 8, 8, 8, 8 };  // eighth notes

    for (int thisNote = 0; thisNote < 7; thisNote++) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(SPEAKER_PIN, melody[thisNote], noteDuration);

      // To distinguish the notes, set a minimum time between them
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(SPEAKER_PIN);  // Stop the tone playing
      Serial.printf("Playing negative note %d: %d\n", thisNote, melody[thisNote]);
    }
  }

  delay(500);              // Keep the LED on for 0.5 seconds
  digitalWrite(LED, LOW);  // Turn the LED off
  Serial.println("LED turned off");
}
