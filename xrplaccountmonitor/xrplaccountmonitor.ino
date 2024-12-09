// XRPL Account monitor created by Handy_4ndy


#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "pitches.h" // Include the pitches header file for note definitions

// WiFi credentials
const char* ssid = "Handy_wifi"; // Insert the network SSID you want to connect to
const char* password = "qwerty123"; // Provide the password for the network

// WebSocket server details
const char* wsServer = "s1.ripple.com";
const int wsPort = 51233;

// XRPL wallet address to monitor
const char* walletAddress = "rJBFRnbNiTskPhMdL4Q6f82tpMDmKLvVCV"; // Replace with your account address

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

  // Connect to WiFi
  WiFi.begin(ssid, password);

  // Wait until the WiFi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print a message when connected to WiFi
  Serial.println("Connected to WiFi");

  // Initialize WebSocket connection
  webSocket.begin(wsServer, wsPort, "/");
  webSocket.onEvent(webSocketEvent); // Set the WebSocket event handler
  webSocket.setReconnectInterval(5000); // Set the reconnection interval to 5000 ms
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

