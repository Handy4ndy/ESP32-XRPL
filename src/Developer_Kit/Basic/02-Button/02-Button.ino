/*
  RGB LED with Button Control Example
  This script demonstrates how to use a button to control an RGB LED with three different modes:

  1. Slow Blink: Cycles through red, green, and blue LEDs with a slow blink.
  2. Fast Fade: Smoothly fades a random color in and out.
  3. Fast Blink: Randomly blinks one or more LEDs at a fast rate.

  The button is connected to GPIO4 with an internal pull-up resistor enabled.
  Pressing the button cycles through the three modes.

  Created by Handy_4ndy.

  This code is free software; you can redistribute it and/or modify it under the terms of the
  GNU Lesser General Public License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
*/

// Define the pins for the RGB LED and button
#define RED_PIN 5
#define GREEN_PIN 19
#define BLUE_PIN 18
#define BUTTON_PIN 4 // Button connected to GPIO4 with internal pull-up resistor

// Declare variables for mode switching and timing
int currentMode = 0; // Tracks the current mode (0: Slow Blink, 1: Fast Fade, 2: Fast Blink)
unsigned long lastButtonPressTime = 0; // Tracks the last button press time for debouncing
unsigned long lastModeUpdateTime = 0; // Tracks the last mode update time
const unsigned long debounceDelay = 200; // Debounce delay for button presses
const unsigned long slowBlinkInterval = 1000; // Interval for slow blink mode
const unsigned long fastBlinkInterval = 200; // Interval for fast blink mode
const unsigned long fadeInterval = 20; // Interval for fade updates
int lastButtonState = HIGH; // Tracks the last button state
int fadeValue = 0; // Current fade value for Fast Fade mode
int fadeDirection = 1; // Direction of fade (1: increasing, -1: decreasing)
int activeColor = 0; // Active color for Fast Fade mode (0: Red, 1: Green, 2: Blue)

void setup() {
  // Initialize pins 
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Enable internal pull-up resistor for the button
 
  // Initialize the Serial Monitor
  Serial.begin(9600);
  Serial.println("Starting LED Mode Select");

  // Turn off all LEDs initially
  analogWrite(RED_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN, 0);
}

void loop() {
  // Handle button press to switch modes
  handleButtonPress();

  // Execute the current mode
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

// Handles button press events and switches between modes
void handleButtonPress() {
  int buttonState = digitalRead(BUTTON_PIN); // Read the current button state

  // Check if the button state has changed and is pressed (LOW)
  if (buttonState != lastButtonState && buttonState == LOW) {
    if (millis() - lastButtonPressTime > debounceDelay) {
      // Cycle to the next mode
      currentMode = (currentMode + 1) % 3;
      lastButtonPressTime = millis(); // Update the last button press time

      // Turn off all LEDs
      analogWrite(RED_PIN, 0);
      analogWrite(GREEN_PIN, 0);
      analogWrite(BLUE_PIN, 0);

      // Print the current mode to the Serial Monitor
      Serial.print("Button pressed: Switching to mode ");
      switch (currentMode) {
        case 0:
          Serial.println("slowBlink");
          break;
        case 1:
          Serial.println("fastFade");
          break;
        case 2:
          Serial.println("fastBlink");
          break;
      }
    }
  }

  lastButtonState = buttonState; // Update the last button state
}

// Slow Blink mode
// Cycle through red, green, and blue LEDs with a slow blink
void slowBlink() {
  
  if (millis() - lastModeUpdateTime >= slowBlinkInterval) {
    lastModeUpdateTime = millis(); 
    static int color = 0; // Tracks the current color (0: Red, 1: Green, 2: Blue)

    // Turn off all LEDs
    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);

    // Turn on the current color LED
    if (color == 0) analogWrite(RED_PIN, 255);
    else if (color == 1) analogWrite(GREEN_PIN, 255);
    else if (color == 2) analogWrite(BLUE_PIN, 255);

    color = (color + 1) % 3; // Cycle to the next color
  }
}

// Fast Fade mode
// Smoothly fade a random color in and out
void fastFade() {
  
  if (millis() - lastModeUpdateTime >= fadeInterval) {
    lastModeUpdateTime = millis(); 

    // Update the fade value
    fadeValue += fadeDirection * 5;
    if (fadeValue <= 0 || fadeValue >= 255) {
      fadeDirection *= -1; // Reverse the fade direction
      if (fadeValue <= 0) {
        activeColor = random(0, 3); // Select a new random color
      }
      fadeValue = constrain(fadeValue, 0, 255); // Constrain the fade value
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
}

// Fast Blink mode
// Randomly blink one or more LEDs at a fast rate
void fastBlink() {
  
  if (millis() - lastModeUpdateTime >= fastBlinkInterval) {
    lastModeUpdateTime = millis(); 

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
