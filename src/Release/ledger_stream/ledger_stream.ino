/*
  Ledger Stream Example
  This script demonstrates how to connect to the XRPL WebSocket server, subscribe to the ledger stream,
  and display the ledger sequence and transaction count on an OLED screen.

  Created by Handy_4ndy.

  This code is free software; you can redistribute it and/or modify it under the terms of the
  GNU Lesser General Public License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  Libraries:
  - WiFi
  - WebSocketsClient
  - Adafruit GFX
  - Adafruit SSD1306
  - ArduinoJson
  - WiFiManager
*/

#include <WiFi.h>
#include <WebSocketsClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>

// XRPL WebSocket server address
const char* websocket_host = "s1.ripple.com"; // XRPL - "s2.ripple.com", "xrplcluster.com" || Xahau - "xahau.network", "xahau.org"
const uint16_t websocket_port = 443;
const char* websocket_url = "/";

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WebSocket client
WebSocketsClient webSocket;

// Variables to store ledger sequence and transaction count
int ledgerSequence = 0;
int transactionCount = 0;
int previousTransactionCount = -1;

void setup() {
  // Initialize Serial
  Serial.begin(9600);

  // Initialize WiFi Manager
  WiFiManager wifiManager;

  // comment the next line to disable reset
  wifiManager.resetSettings();

  while (!wifiManager.autoConnect("Ledger_Monitor")) {
    Serial.println("Failed to connect to WiFi. Retrying...");
  }

  Serial.println("Connected to WiFi..")

  // Initialize WebSocket
  webSocket.begin(websocket_host, websocket_port, websocket_url);
  webSocket.onEvent(webSocketEvent);

  // Initialize SSD1306 display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C is the I2C address for the SSD1306
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  webSocket.loop();

  // Check if the WiFi is still connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");

    // Attempt to reconnect to WiFi
    while (!wifiManager.autoConnect("Ledger_Monitor")) {
      Serial.println("Failed to connect to WiFi. Retrying...");
    }
    Serial.println("Reconnected to WiFi");
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket Disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket Connected");
      // Subscribe to ledger stream
      webSocket.sendTXT("{\"command\": \"subscribe\", \"streams\": [\"ledger\"]}");
      break;
    case WStype_TEXT:
      // Parse the incoming message
      String message = String((char*)payload);
      Serial.println("Received: " + message);

      // Parse JSON message
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, message);

      // Check if it's a ledger closed message
      if (doc.containsKey("type") && String(doc["type"]) == "ledgerClosed") {
        ledgerSequence = doc["ledger_index"];
        transactionCount = doc["txn_count"];
        displayData();
      }
      break;
  }
}

// Function to display data on the OLED screen
void displayData() {
  // Only update the display if the transaction count has changed
  if (transactionCount != previousTransactionCount) {
    // Clear the display before drawing new content
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);

    // Display ledger sequence at the top
    display.setCursor(10, 0);
    display.print("Sequence: ");
    display.println(ledgerSequence);

    // Display transaction count below the ledger sequence
    display.setCursor(13, 18);
    display.print("Transactions: ");
    display.println(transactionCount);

    // Debug prints to verify values
    Serial.print("Transaction Count: ");
    Serial.println(transactionCount);

    // Set square size based on transaction count
    int squareSize = (transactionCount > 500) ? 1 : 2;

    // Space for the ledger sequence text
    int topMargin = 30; 
    // Margin between squares
    int margin = 1; 
    // Number of squares to draw
    int numSquares = transactionCount; 

    // Draw squares based on transaction count
    for (int i = 0; i < numSquares; i++) {
      int x = (i * (squareSize + margin)) % SCREEN_WIDTH;
      int y = SCREEN_HEIGHT - squareSize - ((i * (squareSize + margin)) / SCREEN_WIDTH) * (squareSize + margin);
      display.fillRect(x, y, squareSize, squareSize, SSD1306_WHITE);
    }

    display.display();

    // Update the previous transaction count
    previousTransactionCount = transactionCount;

    // Debug prints to verify bar drawing
    Serial.println("Display updated");
  }
}