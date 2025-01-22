/*
 XRPL Account Monitor - Monitor XRP Ledger account transactions using WebSockets.
 Created by Handy_4ndy.

 This project connects to a specified WiFi network and monitors an XRPL (XRP Ledger) account
 using WebSockets. It processes different types of transactions such as payments, trust lines,
 and offers, and provides audio-visual feedback using an LED and a speaker.

 Please see the README.md File to get started.

 This code is free software; you can redistribute it and/or modify it under the terms of the
 GNU Lesser General Public License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

*/

// Required header files
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include "pitches.h" // Include the pitches header file for note definitions


// XRPL WebSocket server address
const char* websocket_host = "s1.ripple.com";
const uint16_t websocket_port = 443;
const char* websocket_url = "/";

// Custom variable for wallet address
char walletAddress[40] = "";

// Pin definitions
const int LED = 5; // LED Pin
const int SPEAKER_PIN = 4; // Speaker pin

// Create a WebSocket client object
WebSocketsClient webSocket;

void setup() {

  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);

  // Set pin modes
  pinMode(LED, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);

  // Initialize WiFi Manager
  WiFiManager wifiManager;

  // Custom parameter for WiFiManager
  WiFiManagerParameter custom_text("walletAddress", "Wallet Address", walletAddress, 40);
  wifiManager.addParameter(&custom_text);

  // comment the next line to disable reset
  wifiManager.resetSettings();
  wifiManager.autoConnect("Micro_Ledger"); 

  // Save custom parameter value
  strcpy(walletAddress, custom_text.getValue());

  // Print the wallet address to the serial monitor
  Serial.print("Wallet Address: ");
  Serial.println(walletAddress);


  // Initialize WebSocket
  webSocket.begin(websocket_host, websocket_port, websocket_url, "arduino");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  // Keep the WebSocket connection alive
  webSocket.loop();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {

    // Handle WebSocket disconnection
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    // Handle WebSocket connection
    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      subscribeToAccount(walletAddress);
      break;
    // Handle incoming text message
    case WStype_TEXT:
      Serial.printf("WebSocket message: %s\n", payload);
      processTransaction(payload, length); // Process the transaction
      break;
    // Handle incoming binary message
    case WStype_BIN:
      Serial.println("Binary message received");
      break;
    // Handle ping message
    case WStype_PING:
      Serial.println("Ping received");
      break;
    // Handle pong message
    case WStype_PONG:
      Serial.println("Pong received");
      break;
  }
}

void subscribeToAccount(const char* account) {
  // Create a JSON document to subscribe to the account
  DynamicJsonDocument doc(256);
  doc["command"] = "subscribe";
  JsonArray accounts = doc.createNestedArray("accounts");
  accounts.add(account);

  // Serialize the JSON document to a string
  String message;
  serializeJson(doc, message);

  // Send the subscription message via WebSocket
  webSocket.sendTXT(message);
}

void processTransaction(uint8_t* payload, size_t length) {
  // Create a JSON document to hold the incoming payload
  StaticJsonDocument<2048> doc;
  // Deserialize the JSON payload
  DeserializationError error = deserializeJson(doc, payload, length);

  // Check for deserialization errors
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Check if the payload contains a transaction
  if (!doc.containsKey("transaction")) {
    Serial.println("No transaction found");
    return;
  }

  // Extract the transaction object
  JsonObject transaction = doc["transaction"];

  // Extract the transaction type, account, and destination
  const char* transactionType = transaction["TransactionType"];
  const char* account = transaction["Account"];
  const char* destination = transaction["Destination"];

  // Check if the transaction type or account is null
  if (transactionType == nullptr || account == nullptr) {
    Serial.println("TransactionType or Account is null");
    return;
  }

  // Payment notification send = negative note / received = positive note

  if (strcmp(transactionType, "Payment") == 0) {
    // Check if the payment is to or from walletAddress
    if (strcmp(account, walletAddress) == 0) {
      // Outgoing payment
      const char* amountValue = getAmountValue(transaction["Amount"]);
      if (amountValue != nullptr) {
        Serial.printf("Payment sent from %s of amount %s\n", walletAddress, amountValue);
        performAction("Payment", true);
      }
    } else if (strcmp(destination, walletAddress) == 0) {
      // Incoming payment
      const char* amountValue = getAmountValue(transaction["Amount"]);
      if (amountValue != nullptr) {
        Serial.printf("Payment received to %s of amount %s\n", walletAddress, amountValue);
        performAction("Payment", false);
      }
    }
  } 

    // Trustset notification Limit === 0 / negative note / Limit >= 1 positive note
    else if (strcmp(transactionType, "TrustSet") == 0) {
    Serial.println("TrustSet transaction detected");
    const char* limitAmountValue = getAmountValue(transaction["LimitAmount"]);
    if (limitAmountValue != nullptr && strcmp(limitAmountValue, "0") == 0) {
      // Trust line removal
      performAction(transactionType, false);
    } else {
      // Trust line addition
      performAction(transactionType, true);
    }
  } 
  
    // OfferCreate notification
    else if (strcmp(transactionType, "OfferCreate") == 0) {
    Serial.println("OfferCreate transaction detected");
    performAction(transactionType, true);
    
   
  } 
    // OfferCancel notification
    else if (strcmp(transactionType, "OfferCancel") == 0) {
    Serial.println("OfferCancel transaction detected");
    performAction(transactionType, false);
  } else {
    Serial.printf("Unhandled transaction type: %s\n", transactionType);
  }
}

// Function to get the amount value from the transaction
const char* getAmountValue(JsonVariant transaction) {
  // Check if the Amount is a JSON object
  if (transaction.is<JsonObject>()) {
    // Return the value field from the Amount object
    return transaction["value"];
  } else if (transaction.is<const char*>()) {
    // Return the value directly if it is a string
    return transaction.as<const char*>();
  } else {
    // Print an error message if the Amount format is not recognized
    Serial.println("Amount format not recognized");
    return nullptr;
  }
}


// Function to perform an action based on the transaction type and positivity
void performAction(const char* transactionType, bool positive) {
  Serial.printf("%s transaction %s! Performing action...\n", transactionType, positive ? "positive" : "negative");

  // Turn the LED on
  digitalWrite(LED, HIGH);

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
    // Print an error message if the transaction type is unknown
    Serial.println("Unknown transaction type");
    digitalWrite(LED, LOW);
    return;
  }

  // Play the selected melody
  playMelody(melody, melodySize);

  // Turn the LED off
  digitalWrite(LED, LOW);
}

// Function to play a melody
void playMelody(int* melody, int size) {
  // Define the duration of each note (eighth notes)
  int noteDurations[] = { 8, 8, 8, 8, 8, 8, 8 }; 

  // Loop through each note in the melody
  for (int thisNote = 0; thisNote < size; thisNote++) {
    // Calculate the note duration in milliseconds
    int noteDuration = 1000 / noteDurations[thisNote];
    // Play the note on the speaker
    tone(SPEAKER_PIN, melody[thisNote], noteDuration);

    // Pause between notes
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // Stop the tone
    noTone(SPEAKER_PIN);
  }
}

