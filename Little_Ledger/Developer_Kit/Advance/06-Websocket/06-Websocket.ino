/*
  Custom WebSocket Connection Example with OLED Display and Reset Button
  This script demonstrates how to connect to a custom WebSocket server using hardcoded WiFi credentials.
  It logs ledger updates in the serial monitor and displays connection status and received data on an OLED screen.
  A button is included to reset the device.

  Features:
  - Connects to a WebSocket server and subscribes to the XRPL ledger stream.
  - Displays WiFi and WebSocket connection status on an OLED screen.
  - Logs ledger sequence and transaction count to the Serial Monitor and OLED display.
  - Includes a button to reset the ESP32.

  Libraries:
  - `Adafruit_GFX` and `Adafruit_SSD1306` for controlling the OLED display.
  - `WebSocketsClient` for WebSocket communication.
  - `ArduinoJson` for parsing JSON messages.
  - `WiFi` for managing WiFi connections.

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
#include <ArduinoJson.h> // Include ArduinoJson for JSON parsing
#include <WebSocketsClient.h> // Include WebSocketsClient for WebSocket communication
#include <WiFi.h>

// Define button pin
#define BUTTON_PIN 4

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

// Variables for WiFi connection and status checking
static unsigned long lastCheckTime = 0;   // Last time WiFi status was checked
static unsigned long lastPrintTime = 0;    // Last time WiFi status was printed
const unsigned long checkInterval = 20000;  // Interval to check WiFi connection
const unsigned long printInterval = 10000; // Interval to print WiFi status

// Variables for WebSocket connection and status checking
static unsigned long lastWebSocketPrintTime = 0; // Last time WebSocket status was printed
unsigned long lastReconnectAttempt = 0; // Last time WebSocket reconnection was attempted
const unsigned long reconnectInterval = 5000; // Interval to attempt WebSocket reconnection

// Variables to store ledger sequence and transaction count
int ledgerSequence = 0; // Ledger sequence number
int transactionCount = 0; // Transaction count
int previousTransactionCount = 0; // Previous transaction count

// WebSocket client
WebSocketsClient webSocket;

void setup() {
  
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  // Initialize Serial
  Serial.begin(9600);
  Serial.println("Starting WebSocket program...");

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
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
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(5000);
    Serial.println("Connecting to WiFi...");
  }

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

  delay(2000); // Delay to show the connection status
  
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
}

void loop() {
  webSocket.loop();
  handleButtonPress();

  if (millis() - lastReconnectAttempt >= reconnectInterval) {
    lastReconnectAttempt = millis();
    checkWiFiConnection();
    checkWebSocketConnection(); 
  }

  //simulateWiFiDisconnection(); // WiFi disconnection simulation
  //simulateWebSocketDisconnection(); // WebSocket disconnection simulation
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
        
        Serial.print("Ledger Sequence: ");
        Serial.println(ledgerSequence);
        Serial.print("Total Transactions: ");
        Serial.println(transactionCount);
        
        // // Log the full JSON response
        // Serial.println("Full JSON Response:");
        // serializeJsonPretty(doc, Serial);
        
        // Update OLED display with received data
        display.clearDisplay();
        display.setCursor(5, 0);
        display.print("Update from:");
        display.setCursor(5, 8);
        display.print(websocket_host);
        display.setCursor(5, 18);
        display.print("Ledger Seq: ");
        display.print(ledgerSequence);
        display.setCursor(5, 27);
        display.print("Transactions: ");
        display.print(transactionCount);
        display.display();
      }
      break;
    }
  }

  
  // Function to handle button press for resetting the device
  void handleButtonPress(){
    if (digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("Button pressed. Resetting device...");
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Resetting device...");
      display.display();
      delay(2000); 
      ESP.restart(); 
    }
  }
  
  // Check WiFi connection status and reconnect if necessary
  void checkWiFiConnection()
  {
    // Confirm the WiFi is still Connected in the Serial Monitor
    if (WiFi.status() == WL_CONNECTED && millis() - lastPrintTime >= printInterval)
    {
      lastPrintTime = millis();
      Serial.println("WiFi is connected...");
    } 
    
    // WiFi Disconnection alert and reconnect logic
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected. Reconnecting...");

      display.fillRect(0, 16, SCREEN_WIDTH, 48, BLACK);
      display.setCursor(5, 18);
      display.print("Wifi Disconnected");
      display.display();
      
      // Reconnect to the WiFi
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(1000); 
        Serial.println("Reconnecting to WiFi...");
      }
      Serial.println("Reconnected to WiFi");
  
      display.fillRect(0, 16, SCREEN_WIDTH, 48, BLACK);
      display.setCursor(5, 18);
      display.print("WiFi Reconnected");
      display.setCursor(5, 27);
      display.print("SSID: ");
      display.println(ssid);
      display.setCursor(5, 38);
      display.print("IP Address:");
      display.setCursor(5, 47);
      display.print(WiFi.localIP());
      display.display();
    }
  }

  // Check WebSocket connection status and reconnect if necessary
  void checkWebSocketConnection() {
    
    // Check if the WebSocket is still connected 
    if (webSocket.isConnected()) {
      if (millis() - lastWebSocketPrintTime >= printInterval) { 
        lastWebSocketPrintTime = millis(); 
      Serial.println("WebSocket is connected.");
    }
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

//  // Simulate WiFi disconnection
// void simulateWiFiDisconnection() {
//   static unsigned long wifiDisconnectStartTime = 0;
//   static bool wifiDisconnectTriggered = false;

//   if (WiFi.status() == WL_CONNECTED && !wifiDisconnectTriggered) {
//     Serial.println("Simulating WiFi disconnection in 10 seconds...");
//     wifiDisconnectStartTime = millis();
//     wifiDisconnectTriggered = true;
//   }

//   if (wifiDisconnectTriggered && millis() - wifiDisconnectStartTime >= 10000) {
//     display.setCursor(5, 57);
//     display.print("!!DISCONNECTING WIFI!!");
//     display.display();
//     Serial.println("WiFi disconnected.");
//     delay(2000); 
//     WiFi.disconnect(); 
//     wifiDisconnectTriggered = false; 
//   }
// }

// // Simulate WebSocket disconnection
// void simulateWebSocketDisconnection() {
//   static unsigned long wsDisconnectStartTime = 0;
//   static bool wsDisconnectTriggered = false;

//   if (webSocket.isConnected() && !wsDisconnectTriggered) {
//     Serial.println("Simulating WebSocket disconnection in 10 seconds...");
//     wsDisconnectStartTime = millis();
//     wsDisconnectTriggered = true;
//   }

//   if (wsDisconnectTriggered && millis() - wsDisconnectStartTime >= 10000) {
//     display.setCursor(5, 57);
//     display.print("!!DISCONNECTING WSS!!");
//     display.display();
//     Serial.println("WebSocket disconnected.");
//     delay(2000); 
//     webSocket.disconnect(); 
//     wsDisconnectTriggered = false; 
//   }
// }