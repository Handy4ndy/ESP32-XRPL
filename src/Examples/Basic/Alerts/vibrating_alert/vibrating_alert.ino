/*
  Basic Vibrating Alert Example
  This script demonstrates how to use vibrating alerts for different events using a vibration motor.

  Created by Handy_4ndy.

  This code is free software; you can redistribute it and/or modify it under the terms of the
  GNU Lesser General Public License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
*/

#include <Arduino.h>

// Pin definitions
const int VIBRATION_MOTOR_PIN = 5; // Vibration motor pin

void setup() {
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);

  // Set pin mode
  pinMode(VIBRATION_MOTOR_PIN, OUTPUT);
}

void loop() {
  // Example usage of vibrating alerts
  vibratePattern1();
  delay(2000); // Pause for 2 seconds

  vibratePattern2();
  delay(2000); // Pause for 2 seconds

  vibrateHighScorePattern();
  delay(2000); // Pause for 2 seconds
}

void vibratePattern1() {
  int pattern[] = { 200, 100, 200, 100, 200, 100, 200 };
  playVibrationPattern(pattern, 7); 
}

void vibratePattern2() {
  int pattern[] = { 100, 200, 100, 200, 100, 200, 100 };
  playVibrationPattern(pattern, 7); 
}

void vibrateHighScorePattern() {
  int pattern[] = { 300, 100, 300, 100, 300, 100, 300 };
  playVibrationPattern(pattern, 7); 
}

// Function to play a vibration pattern
void playVibrationPattern(int* pattern, int size) {
  // Loop through each duration in the pattern
  for (int i = 0; i < size; i++) {
    // Turn the vibration motor on
    digitalWrite(VIBRATION_MOTOR_PIN, HIGH);
    delay(pattern[i]);

    // Turn the vibration motor off
    digitalWrite(VIBRATION_MOTOR_PIN, LOW);
    delay(100); // Short pause between vibrations
  }
}
