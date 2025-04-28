/*
  Basic RGB LED Example 
  This script demonstrates how to use a three-color RGB LED with three different modes:

  1. Slow Blink: Cycles through red, green, and blue LEDs with a slow blink.
  2. Fast Fade: Smoothly fades a random color in and out.
  3. Fast Blink: Randomly blinks one or more LEDs at a fast rate.

  Created by Handy_4ndy.

  This code is free software; you can redistribute it and/or modify it under the terms of the
  GNU Lesser General Public License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
*/


// Define the pins for the RGB LED
#define RED_PIN 5
#define GREEN_PIN 19
#define BLUE_PIN 18

// Declare variables for mode switching and timing
int currentMode = 0; // Tracks the current mode (0: Slow Blink, 1: Fast Fade, 2: Fast Blink)
unsigned long lastModeUpdateTime = 0; // Tracks the last mode update time
const unsigned long slowBlinkInterval = 1000; // Interval for slow blink mode
const unsigned long fastBlinkInterval = 200; // Interval for fast blink mode
const unsigned long fadeInterval = 20; // Interval for fade updates
int fadeValue = 0; // Current fade value for Fast Fade mode
int fadeDirection = 1; // Direction of fade (1: increasing, -1: decreasing)
int activeColor = 0; // Active color for Fast Fade mode (0: Red, 1: Green, 2: Blue)

void setup() {
  // Initialize pins and serial communication
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("Starting 3-Color LED Program");

  // Set initial LED state to off
  analogWrite(RED_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN, 0);

  Serial.println("Setup complete. Starting loop...");
  delay(2000); // Wait for 2 seconds before starting the loop
}

void loop() {
  // Check if 5 seconds have passed for the current mode
  if (millis() - lastModeUpdateTime >= 5000) {
    currentMode = (currentMode + 1) % 3; 
    Serial.print("Switching to mode: ");
    Serial.println(currentMode);
    lastModeUpdateTime = millis();           
  }

  // Execute the current mode
  if (currentMode == 0) {
    // Slow Blink
    static unsigned long lastUpdateTime = 0;
    if (millis() - lastUpdateTime >= slowBlinkInterval) {
      lastUpdateTime = millis();
      static int color = 0;

      // Turn off all LEDs
      analogWrite(RED_PIN, 0);
      analogWrite(GREEN_PIN, 0);
      analogWrite(BLUE_PIN, 0);

      // Turn on the current color LED
      if (color == 0) analogWrite(RED_PIN, 255);
      else if (color == 1) analogWrite(GREEN_PIN, 255);
      else if (color == 2) analogWrite(BLUE_PIN, 255);
      color = (color + 1) % 3;
    }
  } else if (currentMode == 1) {
    // Fast Fade
    static unsigned long lastUpdateTime = 0;
    if (millis() - lastUpdateTime >= fadeInterval) {
      lastUpdateTime = millis();
      fadeValue += fadeDirection * 5;
      if (fadeValue <= 0 || fadeValue >= 255) {
        fadeDirection *= -1;
        if (fadeValue <= 0) activeColor = random(0, 3);
        fadeValue = constrain(fadeValue, 0, 255);
      }

      // Turn off all LEDs
      analogWrite(RED_PIN, 0);
      analogWrite(GREEN_PIN, 0);
      analogWrite(BLUE_PIN, 0);

      // Set the active color LED to the fade value
      if (activeColor == 0) analogWrite(RED_PIN, fadeValue);
      else if (activeColor == 1) analogWrite(GREEN_PIN, fadeValue);
      else if (activeColor == 2) analogWrite(BLUE_PIN, fadeValue);
    }
  } else if (currentMode == 2) {
    // Fast Blink
    static unsigned long lastUpdateTime = 0;
    if (millis() - lastUpdateTime >= fastBlinkInterval) {
      lastUpdateTime = millis();

      // Turn off all LEDs
      analogWrite(RED_PIN, 0);
      analogWrite(GREEN_PIN, 0);
      analogWrite(BLUE_PIN, 0);

      // Randomly turn on one or more LEDs
      if (random(0, 2)) analogWrite(RED_PIN, 255);
      if (random(0, 2)) analogWrite(GREEN_PIN, 255);
      if (random(0, 2)) analogWrite(BLUE_PIN, 255);
    }
  }
}

// Uncomment and complete the following code to incorporate switch case logic ahead of our next tutorial
/*
void loop() {
  static int currentMode = 0;
  static unsigned long modeStartTime = 0;

  // Check if 5 seconds have passed for the current mode
  if (millis() - modeStartTime >= 5000) {
    currentMode = (currentMode + 1) % 3; 
    modeStartTime = millis();           
  }

  switch (currentMode) {
    case 0:
      slowBlink();
      break;
    case 1:
      fastFade();
      break;
    case 2:
      fastBlink();
      break;
  }
}

void slowBlink() {
  // Cycle through red, green, and blue LEDs with a slow blink
}

void fastFade() {
  // Smoothly fade a random color in and out
}

void fastBlink() {
  // Randomly blink one or more LEDs at a fast rate
}
*/
