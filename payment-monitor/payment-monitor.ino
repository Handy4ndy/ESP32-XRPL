// XRPL Payment monitor created by Handy_4ndy

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
void performAction();

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT); // Set speaker pin as output

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  webSocket.begin(wsServer, wsPort, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void loop() {
  webSocket.loop();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      subscribeToAccount(walletAddress); // Replace with your account address
      break;
    case WStype_TEXT:
      Serial.printf("WebSocket message: %s\n", payload);
      processTransaction(payload, length);
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
  DynamicJsonDocument doc(256);
  doc["command"] = "subscribe";
  JsonArray accounts = doc.createNestedArray("accounts");
  accounts.add(account);

  String message;
  serializeJson(doc, message);
  webSocket.sendTXT(message);
}

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

  if (transactionType == nullptr) {
    Serial.println("TransactionType is null");
    return;
  }

  if (strcmp(transactionType, "Payment") == 0) {
    const char* destination = transaction["Destination"];
    const char* amount = transaction["Amount"]["value"];

    if (destination == nullptr || amount == nullptr) {
      Serial.println("Destination or Amount is null");
      return;
    }

    Serial.printf("Payment to %s of amount %s\n", destination, amount);
    if (strcmp(destination, walletAddress) == 0) {
      // Perform action on valid payment
      performAction();
    }
  }
}

void performAction() {
  Serial.println("Payment received! Performing action...");
  // Example: Toggle an LED
  digitalWrite(LED, HIGH);  // Turn the LED on
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
  delay(500); 
  digitalWrite(LED, LOW);   // Turn the LED off
}
