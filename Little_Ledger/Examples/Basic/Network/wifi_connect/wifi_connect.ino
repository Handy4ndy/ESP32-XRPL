/*
  WiFi Connection Script
  This script demonstrates how to connect an ESP32 to a specified WiFi network using hardcoded credentials.
  It also includes functionality to simulate a disconnection and handle reconnections.

  Created by Handy_4ndy.

  This code is free software; you can redistribute it and/or modify it under the terms of the
  GNU Lesser General Public License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
*/

#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "Spencer-wifi";
const char* password = "fuckoffu";

void setup() {

  // Open the serial port for monitoring the connection
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

}

void loop() {
  // Check if the WiFi is still connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");

    // Attempt to reconnect to WiFi
    WiFi.begin(ssid, password); 
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Reconnecting to WiFi...");
    }
    Serial.println("Reconnected to WiFi");
  }
  
  // Add a delay to avoid constant checking
  delay(10000); // Check every 10 seconds (10000 milliseconds)
  Serial.println("Connected...");

  // // Uncomment the following lines to simulate a disconnection
  // if (WiFi.status() == WL_CONNECTED) {
  //   Serial.println("Simulating disconnection in 5 seconds");
  //   delay(5000);
  //   WiFi.disconnect(); // Disconnect from WiFi
  // }
}