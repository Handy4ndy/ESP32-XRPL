/*
  XRPL Account Monitor with OLED Display and Alerts
  This script demonstrates how to monitor an XRPL account using a WebSocket connection.
  It logs ledger updates and account-specific transactions in the serial monitor and displays relevant data on an OLED screen.
  Alerts are provided via an RGB LED and speaker for specific transaction types.

  Features:
  - Connects to a WebSocket server and subscribes to the XRPL ledger stream and a specific account.
  - Displays WiFi and WebSocket connection status on an OLED screen.
  - Logs ledger sequence, transaction count, and account-specific transactions to the Serial Monitor and OLED display.
  - Provides visual and audio alerts for transactions such as payments, trust lines, and offers.
  - Tracks and displays the highest transaction count (high score) and its ledger sequence.
  - Includes a button for resetting the device, displaying high scores, or entering sleep mode.

  Libraries:
  - `Adafruit_GFX` and `Adafruit_SSD1306` for controlling the OLED display.
  - `WebSocketsClient` for WebSocket communication.
  - `ArduinoJson` for parsing JSON messages.
  - `WiFi` for managing WiFi connections.
  - `Preferences` for storing high scores.
  - `pitches.h` for defining musical note frequencies.

  Created by Handy_4ndy.
  This code is free software; you can redistribute it and/or modify it under the terms of the
  GNU Lesser General Public License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
*/
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <WiFi.h>
#include <Preferences.h> // Include Preferences for storing high score
#include "pitches.h"

// Define button pin
#define BUTTON_PIN 4

// Define speaker pin
#define SPEAKER_PIN 17

// Define the pins for the RGB LED
#define RED_PIN 5
#define GREEN_PIN 19
#define BLUE_PIN 18

// Replace with your network credentials
const char* ssid = "YOUR_NETWORK_SSID"; // Replace with your WiFi SSID
const char* password = "YOUR_NETWORK_PASSWORD"; // Replace with your WiFi password

// XRPL WebSocket server address
const char* websocket_host = "s1.ripple.com"; // XRPL - "s2.ripple.com", "xrplcluster.com" || Xahau - "xahau.network", "xahau.org"
const uint16_t websocket_port = 443;
const char* websocket_url = "/";

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables to store ledger sequence and transaction count
int ledgerSequence = 0;
int transactionCount = 0;
unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 4000; // Interval to attempt WebSocket reconnection
int previousTransactionCount = -1;

// WebSocket client
WebSocketsClient webSocket;

// Preferences for storing high score
Preferences preferences;

// Variables to store the highest transaction count and its ledger sequence
int highScore = 0;
int highScoreLedger = 0;

// Debounce variables
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // 50ms debounce delay

// Custom variable for wallet address
const char* walletAddress = "rADDRESSrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr"; // Replace with your wallet address

void setup() {
  
  // Initialize Pins
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  // Initialize Serial
  Serial.begin(9600);
  Serial.println("Starting WebSocket program...");
  
  // Set initial LED state to off
  analogWrite(RED_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN, 0);
  
  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C is the I2C address for the SSD1306
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
 
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(3, 0);
  display.print("Wifi/Websocket Setup");
  display.setCursor(5, 18);
  display.print("Connecting to:");
  display.setCursor(5, 27);
  display.print(ssid);
  display.display();
  
  delay(2000);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.println("Connecting to WiFi...");
  }
  
  // Log and display WiFi connection status
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Update OLED display with WiFi connection status
  display.fillRect(0, 20, SCREEN_WIDTH, 44, BLACK);
  display.setCursor(30, 20);
  display.print("Connected to:");
  display.setCursor(28, 35);
  display.print(ssid);
  display.setCursor(15, 45);
  display.print("IP: ");
  display.print(WiFi.localIP());
  display.display();
  
  // Delay to show the connection status
  delay(2000);
  
  // Initialize WebSocket
  Serial.println("Connecting to WebSocket server...");
  webSocket.beginSSL(websocket_host, websocket_port, websocket_url);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(reconnectInterval);
  
  // Update OLED display with WebSocket connection status
  display.fillRect(0, 16, SCREEN_WIDTH, 48, BLACK);
  display.setCursor(5, 18);
  display.print("Connecting to:");
  display.setCursor(5, 27);
  display.print(websocket_host);
  display.display();

  delay(2000); // Delay to show the connection status
  
  // Load high score from preferences
  loadHighScore();
}

void loop() {
  
  webSocket.loop();
  handleButtonPress();
  
  if (millis() - lastReconnectAttempt >= reconnectInterval) {
    lastReconnectAttempt = millis();

    // Check Wi-Fi connection and reconnect if necessary
    checkWiFiConnection();
    checkWebSocketConnection();
    
  }
}

// Function to load the high score from preferences
void loadHighScore() {
  preferences.begin("ledgerPrefs", false);
  highScore = preferences.getInt("highScore", 0);
  highScoreLedger = preferences.getInt("highScoreLedger", 0);
  preferences.end();
  
  Serial.print("Loaded High Score: ");
  Serial.println(highScore);
  Serial.print("From Ledger: ");
  Serial.println(highScoreLedger);
}

// Modify the WebSocket event handler to include account subscription and transaction processing
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
    Serial.println("WebSocket Disconnected");
    break;
    case WStype_CONNECTED:
    Serial.println("WebSocket Connected");
    display.clearDisplay();
    display.setCursor(10, 0);
    display.print("WebSocket Connected");
    display.display();
    
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
      displayData();
      
      // Update the high score if applicable
      updateHighScore(transactionCount, ledgerSequence);
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
// Payment notification send = negative note / received = positive note

  if (strcmp(transactionType, "Payment") == 0) {
    // Check if the payment is to or from walletAddress
    if (strcmp(account, walletAddress) == 0) {
      // Outgoing payment
      const char* amountValue = getAmountValue(transaction["Amount"]);
      if (amountValue != nullptr) {
        Serial.printf("Payment sent from %s of amount %s\n", walletAddress, amountValue);
        performAction("Payment", true);
        

        // Print on the screen the payment sent
        display.clearDisplay();
        display.setCursor(30, 2);
        display.print("Payment Sent");
        display.setTextSize(1);
        display.setCursor(30, 16);
        display.print("Destination:");
        display.setCursor(0, 27);
        display.print(destination);
        display.setCursor(35, 44);
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
        performAction("Payment", false);

        // Print on the screen the payment recieved
        display.clearDisplay();
        display.setCursor(2, 2);
        display.print("Payment recieved");
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

    // Trustset notification Limit === 0 / negative note / Limit >= 1 positive note
    else if (strcmp(transactionType, "TrustSet") == 0) {
    Serial.println("TrustSet transaction detected");
    const char* limitAmountValue = getAmountValue(transaction["LimitAmount"]);
    if (limitAmountValue != nullptr && strcmp(limitAmountValue, "0") == 0) {
      // Trust line removal
      performAction(transactionType, false);

      // Print on the screen the payment sent
        display.clearDisplay();
        display.setCursor(2, 2);
        display.print("Trustline Removed");
        display.setTextSize(1);
        display.setCursor(0, 16);
        display.print("Trustline has been removed from:");
        display.setCursor(0, 35);
        display.print(walletAddress);
        display.display();
        delay(10000);
        display.clearDisplay();
        display.display();
    } else {
      // Trust line addition
      performAction(transactionType, true);
     
      display.clearDisplay();
        display.setCursor(6, 2);
        display.print("Trustline Set");
        display.setTextSize(1);
        display.setCursor(0, 16);
        display.print("Trustline has been set on:");
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
    performAction(transactionType, true);
  
        display.clearDisplay();
        display.setCursor(2, 2);
        display.print("Offer Create");
        display.setTextSize(1);
        display.setCursor(0, 16);
        display.print("An offer has been created by:");
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
        performAction(transactionType, false);
        
        display.clearDisplay();
        display.setCursor(2, 2);
        display.print("Offer Cancel");
        display.setTextSize(1);
        display.setCursor(0, 16);
        display.print("An offer has been canceled by:");
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

    void displayData() {
      // Only update the display if the transaction count has changed
      if (transactionCount != previousTransactionCount) {
        // Clear the display before drawing new content
        display.clearDisplay();
        
    display.setTextColor(SSD1306_WHITE);
    
    // // Display ledger sequence at the top
    display.setCursor(17, 0);
    display.print("Index: ");
    display.println(ledgerSequence);
    
    display.setCursor(0, 8);
    display.print("Txns:");
    display.print(transactionCount);
    display.setCursor(85, 8);
    display.print("HS:");
    display.println(highScore);
    
    // Debug prints to verify values
    // Serial.print("Transaction Count: ");
    // Serial.println(transactionCount);
    
    // Set square size based on transaction count
    int squareSize = (transactionCount > 500) ? 1 : 2;
    
    // Adjust margin based on transaction count
    int margin = (transactionCount > 1250) ? 0 : 1;
    
    // Display 2x2 pixel squares representing transaction count at the bottom
    int topMargin = 20; // Space for the ledger sequence text
    
    int numSquares = transactionCount; // Number of squares to draw
    
    for (int i = 0; i < numSquares; i++) {
      int x = (i * (squareSize + margin)) % SCREEN_WIDTH;
      int y = SCREEN_HEIGHT - squareSize - ((i * (squareSize + margin)) / SCREEN_WIDTH) * (squareSize + margin);
      display.fillRect(x, y, squareSize, squareSize, SSD1306_WHITE);
    }
    
    display.display();
    
    // Update the previous transaction count
    previousTransactionCount = transactionCount;
    
    // // Debug prints to verify bar drawing
    // Serial.println("Display updated");
    
    // Set LED color based on transaction count
    if (transactionCount <= 150){
      analogWrite(RED_PIN, 0);
      analogWrite(GREEN_PIN, 0);
      analogWrite(BLUE_PIN, 255);
    }else if (transactionCount < 500){
      analogWrite(RED_PIN, 0);
      analogWrite(GREEN_PIN, 255);
      analogWrite(BLUE_PIN, 0);
      playTune1();
    }else {
      analogWrite(RED_PIN, 255);
      analogWrite(GREEN_PIN, 0);
      analogWrite(BLUE_PIN, 0);
      playTune2();
    }
    
  }

}

// Function to perform an action based on the transaction type and positivity
void performAction(const char* transactionType, bool positive) {
  Serial.printf("%s transaction %s! Performing action...\n", transactionType, positive ? "positive" : "negative");
  
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
    return;
  }

  // Play the selected melody
  playMelody(melody, melodySize);

}

// Function to play a melody
void playMelody(int* melody, int size) {
  // Define the duration of each note (eighth notes)
  int noteDurations[] = { 16, 16, 16, 16, 16, 16, 16 }; 

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

// Function to play a specific tune
void playTune1() {
  int melody[] = { NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_E4, NOTE_C4 };
  playMelody(melody, 7);
}

void playTune2() {
  int melody[] = { NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5 };
  playMelody(melody, 7);
}

void playHighScoreTune() {
  int melody[] = { NOTE_E5, NOTE_G5, NOTE_B5, NOTE_E6, NOTE_B5, NOTE_G5, NOTE_E5 };
  playMelody(melody, 7); 
}

// Function to update the high score if a new one is achieved
void updateHighScore(int transactionCount, int ledgerSequence) {
  if (transactionCount > highScore) {
    highScore = transactionCount;
    highScoreLedger = ledgerSequence;
    
    preferences.begin("ledgerPrefs", false);
    preferences.putInt("highScore", highScore);
    preferences.putInt("highScoreLedger", highScoreLedger);
    preferences.end();

    Serial.print("New High Score: ");
    Serial.println(highScore);
    Serial.print("Achieved in Ledger: ");
    Serial.println(highScoreLedger);
    playHighScoreTune();
  }
}


// handle button press events
void handleButtonPress() {
  static unsigned long buttonPressStart = 0;
  static int lastButtonState = HIGH;
  int buttonState = digitalRead(BUTTON_PIN);
  
  // Check if the button state has changed
  if (buttonState != lastButtonState) {
    lastDebounceTime = millis(); // Reset debounce timer
  }
  
  // If the button state is stable for the debounce delay
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonState == LOW) {
      if (buttonPressStart == 0) {
        buttonPressStart = millis();
      } else if (millis() - buttonPressStart > 10000) { // Long press detected (10 seconds)
        Serial.println("Long press detected. Entering sleep mode...");
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Entering Sleep Mode...");
        display.display();
        delay(2000); // Show the message for 2 seconds
        enterSleepMode(); // Enter low-power sleep mode
      }
    } else if (buttonState == HIGH && buttonPressStart > 0) {
      if (millis() - buttonPressStart > 2000 && millis() - buttonPressStart <= 10000) { // Medium press detected (2-10 seconds)
        Serial.println("Medium press detected. Resetting device...");
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Resetting device...");
        display.display();
        delay(2000); // Show the message for 2 seconds
        ESP.restart(); // Restart the ESP32
      } else if (millis() - buttonPressStart <= 2000) { // Short press detected
        Serial.println("Short press detected. Displaying high score...");
        displayHighScore();
        delay(5000); // Show the high score for 5 seconds
        display.clearDisplay(); // Clear the display after showing the high score
        display.display();
      }
      buttonPressStart = 0; // Reset the button press start time
    }
  }
  
  lastButtonState = buttonState; // Update the last button state
}

void enterSleepMode() {
  // Configure the wake-up source as the button pin
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW);
  
  // Display a message before entering sleep mode
  Serial.println("ESP32 is now in sleep mode. Press the button to wake up.");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Press button to");
  display.setCursor(0, 10);
  display.print("wake up...");
  display.display();
  
  delay(2000);
  
  display.clearDisplay();
  display.display();
  
  
  // Enter deep sleep mode
  esp_deep_sleep_start();
}

// Function to display the high score on the OLED display on button press
void displayHighScore() {
  display.clearDisplay();
  display.setCursor(5, 0);
  display.setTextSize(2);
  display.print("High Score");
  display.setTextSize(1);
  display.setCursor(5, 27);
  display.print("Transactions: ");
  display.println(highScore);
  display.setCursor(5, 36);
  display.print("Index: ");
  display.println(highScoreLedger);
  playHighScoreTune();
}

// Function to check WiFi connection status and reconnect if necessary
void checkWiFiConnection() {

  // Check if the WiFi is still connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("WiFi Reconnecting");
    display.display();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000); // Short delay during reconnection attempts
      Serial.println("Reconnecting to WiFi...");
    }
    Serial.println("Reconnected to WiFi");
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("WiFi Reconnected");
    display.setCursor(0, 18);
    display.print("IP Address: ");
    display.setCursor(0, 27);
    display.print(WiFi.localIP());
    display.display();
  }
}

// Check WebSocket connection status and reconnect if necessary
void checkWebSocketConnection() {

  // Check if the WebSocket is still connected
  if (!webSocket.isConnected()) {
    Serial.println("WebSocket is disconnected. Attempting to reconnect...");
    display.clearDisplay();
    display.setCursor(5, 0);
    display.print("Disconnected!");
    display.setCursor(5, 18);
    display.print("Connecting to:");
    display.setCursor(5, 27);
    display.print(websocket_host);
    display.setCursor(5, 38);
    display.print("awaiting connection.");
    display.display();
    delay(2000); // Delay to show the connection status
    webSocket.beginSSL(websocket_host, websocket_port, websocket_url);
  }
}