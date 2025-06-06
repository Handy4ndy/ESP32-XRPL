/*
  WiFi Connection Script with OLED Display
  This program demonstrates how to connect an ESP32 to a specified WiFi network using hardcoded credentials.
  It also includes functionality to handle disconnections and reconnections.

  Features:
  - Displays connection status on an OLED screen.
  - Prints the IP address to the Serial Monitor and OLED display upon successful connection.
  - Automatically reconnects to WiFi if disconnected.

  Libraries:
  - `Adafruit_GFX` and `Adafruit_SSD1306` for controlling the OLED display.
  - `WiFi.h` for managing WiFi connections.

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
#include <WiFi.h>

// Replace with your network credentials
const char *ssid = "YOUR_NETWORK_SSID";
const char *password = "YOUR_NETWORK_PASSWORD";

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables for WiFi connection and status checking
static unsigned long lastCheckTime = 0;    // Tracks the last time the WiFi status was checked
static unsigned long lastPrintTime = 0;    // Tracks the last time the message was printed
const unsigned long checkInterval = 10000; // Interval to check WiFi status (10 seconds)
const unsigned long printInterval = 5000;  // Interval to print the message (5 seconds)


/////////////////////////// SETUP ///////////////////////////////

void setup()
{

  // Open the serial port for monitoring the connection
  Serial.begin(9600);
  Serial.println("Starting Connectivity program...");
  delay(500);

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // 0x3C is the I2C address for the SSD1306
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  // Ensure the speaker is completely turned off after the melody
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(5, 0);
  display.print("WiFi Connection Test");
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
}

void loop()
{

  // Check if it's time to verify the WiFi status
  if (millis() - lastCheckTime >= checkInterval)
  {
    lastCheckTime = millis(); // Update the last check time
    checkWiFiConnection();    // Call the function
  }

  //   // Uncomment the following lines to simulate a disconnection
  // if (WiFi.status() == WL_CONNECTED) {
  //   Serial.println("Simulating disconnection in 10 seconds");
  //   delay(10000);
  //   display.setCursor(15, 57);
  //   display.print("!!DISCONNECTING!!");
  //   display.display();
  //   delay(2000);
  //   WiFi.disconnect(); // Disconnect from WiFi
  // }
}

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

//////////////////////////// Wifi Manager  ///////////////////////////////////

// In addition to the above code, you can use the WiFiManager library to manage WiFi connection through a captive portal, for ease of use we will continue with the hardcoded credentials throught these tutorials.
// This library makes it easier to connect to different networks without hardcoding credentials and allows for custom parameters to be set.

// #include <WiFiManager.h> // Include the WiFiManager library

// // Initialize WiFi Manager
// WiFiManager wifiManager;

// // Declare the WiFi Manager parameter
// char customparam[40] = "";

// // Variables for non-blocking WiFiManager logic
// static unsigned long lastReconnectAttempt = 0;
// const unsigned long reconnectInterval = 10000; // Interval to attempt reconnection (10 seconds)
// static unsigned long lastPrintTime = 0; // Tracks the last time the message was printed
// const unsigned long printInterval = 5000; // Interval to print the message (5 seconds)

// void setup() {
//   // Initialize Serial for debugging
//   Serial.begin(9600);

//   // Custom parameter for WiFiManager
//   WiFiManagerParameter custom_text("customparam", "Custom Parameter", customparam, 40);
//   wifiManager.addParameter(&custom_text);

//   // Uncomment the next line to reset WiFi settings
//   // wifiManager.resetSettings();

//   // Attempt to connect to WiFi using WiFiManager
//   if (!wifiManager.autoConnect("WiFi_Test")) {
//     Serial.println("Failed to connect to WiFi. Retrying...");
//   } else {
//     Serial.println("Connected to WiFi");
//   }

//   // Save custom parameter value
//   strcpy(customparam, custom_text.getValue());

//   // If custom parameter is found, print it to the serial monitor
//   if (strlen(customparam) > 0) {
//     Serial.print("Custom parameter was entered, custom parameter used: ");
//     Serial.println(customparam);
//   } else {
//     // Use the hardcoded parameter if blank
//     strcpy(customparam, "hardcodedParam");
//     Serial.print("No custom parameter was added, hardcoded parameter used: ");
//     Serial.println(customparam);
//   }
// }

// void loop() {
//   // Non-blocking logic for WiFiManager reconnection
//   if (WiFi.status() != WL_CONNECTED) {
//     if (millis() - lastReconnectAttempt >= reconnectInterval) {
//       lastReconnectAttempt = millis(); // Update the last reconnect attempt time
//       Serial.println("WiFi disconnected. Attempting to reconnect...");

//       // Attempt to reconnect using WiFiManager
//       if (!wifiManager.autoConnect("WiFi_Test")) {
//         Serial.println("Failed to reconnect to WiFi. Retrying...");
//       } else {
//         Serial.println("Reconnected to WiFi");
//       }
//     }
//   } else {
//     // Print "WiFi is connected..."every 20 seconds
//   if (millis() - lastPrintTime >= printInterval) {
//     lastPrintTime = millis(); // Update the last print time
//     Serial.println("WiFi is connected...");
//   }
//   }

//   // Add other non-blocking tasks here
//}