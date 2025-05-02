/*
  Advanced WebSocket Connection with High Score Tracking
  This script demonstrates how to connect to a custom WebSocket server using hardcoded WiFi credentials.
  It logs ledger updates in the serial monitor and displays connection status and received data on an OLED screen.
  A button is included to reset the device, display high scores, or enter sleep mode.

  Features:
  - Connects to a WebSocket server and subscribes to the XRPL ledger stream.
  - Displays WiFi and WebSocket connection status on an OLED screen.
  - Logs ledger sequence and transaction count to the Serial Monitor and OLED display.
  - Tracks and displays the highest transaction count (high score) and its ledger sequence.
  - Includes a button for resetting the device, displaying high scores, or entering sleep mode.
  - Uses RGB LEDs to indicate transaction count thresholds.

  Libraries:
  - `Adafruit_GFX` and `Adafruit_SSD1306` for controlling the OLED display.
  - `WebSocketsClient` for WebSocket communication.
  - `ArduinoJson` for parsing JSON messages.
  - `WiFi` for managing WiFi connections.
  - `Preferences` for storing high scores.

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

// Define button pin
#define BUTTON_PIN 4

// Define speaker pin (not integrated)
#define SPEAKER_PIN 17 

// Define the pins for the RGB LED
#define RED_PIN 5
#define GREEN_PIN 19
#define BLUE_PIN 18

// Replace with your network credentials
const char* ssid = "YOUR_NETWORK_SSID";
const char* password = "YOUR_NETWORK_PASSWORD";

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

// Variables for WebSocket status printing
unsigned long lastWebSocketPrintTime = 0;
const unsigned long printInterval = 1000; 

void setup() {

  // Initialize pins
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
  display.fillRect(0, 16, SCREEN_WIDTH, 48, BLACK);
  display.setCursor(5, 18);
  display.print("Connected to:");
  display.setCursor(5, 27);
  display.print(ssid);
  display.setCursor(5, 38);
  display.print("IP Address: ");
  display.setCursor(5, 47);
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

    // Check WebSocket connection and reconnect if necessary
    if (millis() - lastWebSocketPrintTime >= printInterval) {
      lastWebSocketPrintTime = millis();
      checkWebSocketConnection();
    }
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

// Function to handle WebSocket events
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
    Serial.println("WebSocket Disconnected");
    break;

    case WStype_CONNECTED:
    Serial.println("WebSocket Connected");
    display.fillRect(0, 38, SCREEN_WIDTH, 26, BLACK);
    display.setCursor(5, 38);
    display.print("WebSocket Connected");
    display.display();
    
    // Subscribe to ledger stream
    webSocket.sendTXT("{\"command\": \"subscribe\", \"streams\": [\"ledger\"]}");
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
      
      //// Debug prints to verify values
      // Serial.print("Ledger Sequence: ");
      // Serial.println(ledgerSequence);
      // Serial.print("Total Transactions: ");
      // Serial.println(transactionCount);

      // Update OLED display with received data
      displayData();
      
      // Update the high score if applicable
      updateHighScore(transactionCount, ledgerSequence);
    }
    break;
  }
}

// Function to update the OLED display with transaction count and ledger sequence
void displayData() {
  // Update the display if the transaction count has changed
  if (transactionCount != previousTransactionCount) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(17, 0);
    display.print("Index: ");
    display.println(ledgerSequence);
    display.setCursor(0, 8);
    display.print("Txns:");
    display.print(transactionCount);
    display.setCursor(83, 8);
    display.print("HS:");
    display.println(highScore);
    
    // Debug prints to verify values
    // Serial.print("Transaction Count: ");
    // Serial.println(transactionCount);
    
    // Set square size based on transaction count
    int squareSize = (transactionCount > 500) ? 1 : 2;
    
    // Adjust margin based on transaction count
    int margin = (transactionCount > 1250) ? 0 : 1;
    
    // Space for the ledger sequence text
    int topMargin = 20; 
    
    // Number of squares to draw
    int numSquares = transactionCount; 
    
    // Calculate the number of squares that can fit on the screen
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
    }else {
      analogWrite(RED_PIN, 255);
      analogWrite(GREEN_PIN, 0);
      analogWrite(BLUE_PIN, 0);
    }
  
  }
}

// Function to update the high score if a new one is achieved
void updateHighScore(int transactionCount, int ledgerSequence) {

  // Check if the current transaction count is greater than the high score
  if (transactionCount > highScore) {
    highScore = transactionCount;
    highScoreLedger = ledgerSequence;

    // Update the high score in preferences
    preferences.begin("ledgerPrefs", false);
    preferences.putInt("highScore", highScore);
    preferences.putInt("highScoreLedger", highScoreLedger);
    preferences.end();

    // // Log the new high score
    // Serial.print("New High Score: ");
    // Serial.println(highScore);
    // Serial.print("Achieved in Ledger: ");
    // Serial.println(highScoreLedger);
  }
}

// Function to handle button press events
// This function handles button presses for short, medium, and long presses
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
      } else if (millis() - buttonPressStart > 10000) { 
        
        // Long press detected (10 seconds)
        Serial.println("Long press detected. Entering sleep mode...");
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Entering Sleep Mode..");
        display.display();

        delay(2000); // Show the message for 2 seconds

        enterSleepMode(); // Enter low-power sleep mode
      }
    
    } else if (buttonState == HIGH && buttonPressStart > 0) {
      // Medium press detected (2-10 seconds)
      if (millis() - buttonPressStart > 2000 && millis() - buttonPressStart <= 10000) { 
        Serial.println("Medium press detected. Resetting device...");
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Resetting device...");
        display.display();

        delay(2000); // Show the message for 2 seconds

        ESP.restart(); // Restart the ESP32

        // Short press detected
      } else if (millis() - buttonPressStart <= 2000) { 
        Serial.println("Short press detected. Displaying high score...");
        displayHighScore();

        delay(5000); // Show the high score for 5 seconds

        display.clearDisplay(); // Clear the display after showing the high score
        display.display();
      }

      // Reset the button press start time
      buttonPressStart = 0; 
    }
  }

  // Update the last button state
  lastButtonState = buttonState; 
}

// Function to enter low-power sleep mode
// This function configures the wake-up source and enters deep sleep mode
void enterSleepMode() {

  // Configure the wake-up source as the button pin
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW);
  
  // Display a message before entering sleep mode
  Serial.println("ESP32 is now in sleep mode. Press the button to wake up.");
  display.setCursor(0, 18);
  display.print("Press button to");
  display.setCursor(0, 27);
  display.print("wake up...");
  display.display();
  
  delay(4000); // Show the message for 2 seconds
  
  display.clearDisplay(); // Clear the display before entering sleep mode
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
  display.display();
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
  if (webSocket.isConnected()) {
    Serial.println("WebSocket is connected.");
  } else {
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