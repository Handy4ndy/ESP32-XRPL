/*
  Basic LED Alert Example
  This script demonstrates how to use a three-color LED for different alerts.

  Created by Handy_4ndy.

  This code is free software; you can redistribute it and/or modify it under the terms of the
  GNU Lesser General Public License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
*/

#include <Arduino.h>

// Pin definitions for the three-color LED
const int RED_PIN = 5;
const int GREEN_PIN = 18;
const int BLUE_PIN = 19;

void setup() {
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);

  // Set pin modes
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

void loop() {
  // Cycle through colors
  cycleColors();
  delay(2000); // Pause for 2 seconds

  // Flash SOS
  flashSOS();
  delay(2000); // Pause for 2 seconds

  // Create a rainbow effect for 20 seconds
  rainbowEffect(20000);
  delay(2000); // Pause for 2 seconds
}

// Function to set the color of the three-color LED
void setColor(int red, int green, int blue) {
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}

// Function to cycle through colors
void cycleColors() {

  // Off
  setColor(0, 0, 0); 
  delay(2000); 

  // Red
  setColor(255, 0, 0); 
  delay(2000); 

  // Green
  setColor(0, 255, 0); 
  delay(2000); 

  // Blue
  setColor(0, 0, 255);
  delay(2000); 

  // Yellow
  setColor(255, 255, 0); 
  delay(2000); 

  // Cyan
  setColor(0, 255, 255);
  delay(2000); 

  // Magenta
  setColor(255, 0, 255); 
  delay(2000); 

  // White
  setColor(255, 255, 255); 
  delay(2000);

  // Off
  setColor(0, 0, 0); 
  delay(2000); 
}

// Function to flash SOS in Morse code
void flashSOS() {

  // Short flash
  for (int i = 0; i < 3; i++) {
    setColor(255, 0, 0);
    delay(250); 
    setColor(0, 0, 0);
    delay(250);
  }

  // Long flash
  for (int i = 0; i < 3; i++) {
    setColor(255, 0, 0); 
    delay(750); 
    setColor(0, 0, 0); 
    delay(250); 
  }
  
  // Short flash
  for (int i = 0; i < 3; i++) {
    setColor(255, 0, 0); 
    delay(250); 
    setColor(0, 0, 0); 
    delay(250); 
  }
}

// Function to create a rainbow effect
void rainbowEffect(int duration) {
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {

    // Red to Green
    for (int i = 0; i < 256; i++) {
      setColor(i, 255 - i, 0); 
      delay(10);
    }

    // Green to Blue
    for (int i = 0; i < 256; i++) {
      setColor(255 - i, 0, i); 
      delay(10);
    }

    // Blue to Red
    for (int i = 0; i < 256; i++) {
      setColor(0, i, 255 - i); 
      delay(10);
    }
  }
}
