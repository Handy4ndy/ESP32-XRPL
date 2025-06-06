/*
  Custom WebSocket Connection Example with WiFiManager
  This script demonstrates how to connect to a custom WebSocket server using WiFiManager and a custom WebSocket host parameter.
  It logs ledger updates in the serial monitor.

  Created by Handy_4ndy.

  This code is free software; you can redistribute it and/or modify it under the terms of the
  GNU Lesser General Public License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  Libraries:
  - WebSocketsClient - WebSockets_Generic by khoih-prog
  - ArduinoJson - ArduinoJson by Benoit Blanchon
  - WiFiManager - WiFiManager by tzapu
*/

#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <WiFiManager.h>

// XRPL WebSocket server address
// const char* websocket_host = "s2.ripple.com"; // XRPL - "s2.ripple.com", "xrplcluster.com" || Xahau - "xahau.network", "xahau.org"
const uint16_t websocket_port = 443;
const char* websocket_url = "/";

// Declare the WiFi Manager parameter
char websocket_host[40] = "";

// Variables to store ledger sequence and transaction count
int ledgerSequence = 0;
int transactionCount = 0;

unsigned long lastReconnectAttempt = 5;
const unsigned long reconnectInterval = 4000; 

// Initialize WiFi Manager
WiFiManager wifiManager;

// WebSocket client
WebSocketsClient webSocket;

void setup() {
  // Initialize Serial
  Serial.begin(9600);

  // Custom parameter for WiFiManager
  WiFiManagerParameter custom_text("websocket_host", "Custom host", websocket_host, 40);
  wifiManager.addParameter(&custom_text);

  //Comment the next line to disable reset
  wifiManager.resetSettings();

  // Attempt to connect to WiFi
  while (!wifiManager.autoConnect("WiFi_Manager")) {
    Serial.println("Failed to connect to WiFi. Retrying...");
  }

  Serial.println("Connected to WiFi");

  // Save custom parameter value
  strcpy(websocket_host, custom_text.getValue());

  // If custom host is found, print it to the serial monitor
  if (strlen(websocket_host) > 0) {
    Serial.print("Custom host was entered, custom host used: ");
    Serial.println(websocket_host);

  } else {
   // Use the hardcoded host if blank
    strcpy(websocket_host, "s2.ripple.com");
    Serial.print("No custom host was added, hardcoded parameter used: ");
    Serial.println(websocket_host);
  }

  // Initialize WebSocket
  webSocket.beginSSL(websocket_host, websocket_port, websocket_url);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(reconnectInterval);
}

void loop() {
  webSocket.loop();

  // Check if the WiFi is still connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");

    // Attempt to reconnect to WiFi
    while (!wifiManager.autoConnect("WiFi_Manager")) {
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
      Serial.print("Update received from: ");
      Serial.println(websocket_host);
      // Serial.println("Received: " + message);

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

        // // Log the full JSON response
        // Serial.println("Full JSON Response:");
        // serializeJsonPretty(doc, Serial);
      }
      break;
  }
}

