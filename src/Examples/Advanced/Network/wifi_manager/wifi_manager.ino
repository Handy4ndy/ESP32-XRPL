/*
  WiFi Manager Setup
  This script demonstrates how to use the WiFiManager library to connect an ESP32 to a specified WiFi network.
  It includes functionality to handle reconnections, simulate disconnections, and enter custom parameters.

  Created by Handy_4ndy.

  This code is free software; you can redistribute it and/or modify it under the terms of the
  GNU Lesser General Public License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  Libraries:
  - WiFiManager by tzapu: https://github.com/tzapu/WiFiManager

  Custom Parameter Functionality:
  - Uncomment the relevant lines to add a custom parameter for WiFiManager.
  - The custom parameter can be used to enter additional configuration data.
*/

#include <WiFiManager.h> // Include the WiFiManager library

// Initialize WiFi Manager
WiFiManager wifiManager;

// // Declare the WiFi Manager parameter
// char customparam[40] = "";

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);

  // // Custom parameter for WiFiManager
  // WiFiManagerParameter custom_text("customparam", "Custom Parameter", customparam, 40);
  // wifiManager.addParameter(&custom_text);

  // Comment the next line to disable reset
  wifiManager.resetSettings();

  // Attempt to connect to WiFi
  while (!wifiManager.autoConnect("WiFi_Manager")) {
    Serial.println("Failed to connect to WiFi. Retrying...");
  }

  Serial.println("Connected to WiFi");

  // // Save custom parameter value
  // strcpy(customparam, custom_text.getValue());

  // // If custom parameter is found, print it to the serial monitor
  // if (strlen(customparam) > 0) {
  //   Serial.print("Custom parameter was entered, custom parameter used: ");
  //   Serial.println(customparam);

  // } else {
  //  // Use the hardcoded parameter if blank
  //   strcpy(customparam, "hardcodedParam");
  //   Serial.print("No custom parameter was added, hardcoded parameter used: ");
  //   Serial.println(customparam);
  // }

}

void loop() {
  // Check if the WiFi is still connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");

    // Attempt to reconnect to WiFi
    while (!wifiManager.autoConnect("WiFi_Manager")) {
      Serial.println("Failed to connect to WiFi. Retrying...");
    }
    Serial.println("Reconnected to WiFi");
  }

  // Add a delay to avoid constant checking
  delay(10000); // Check every 10 seconds (10000 milliseconds)
  Serial.println("Connected...");
  

  // // Uncomment the following lines to simulate a disconnection
  // if (WiFi.status() == WL_CONNECTED) {
  //   Serial.println("Simulating disconnection in 5 seconds");
  //   delay(5000); // Wait for 5 seconds
  //   WiFi.disconnect(); // Disconnect from WiFi
  // }
  
}