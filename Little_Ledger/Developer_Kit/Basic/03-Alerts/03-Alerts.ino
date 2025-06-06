/*
  3-Color LED Program with Button Control and Speaker Alerts
  This program demonstrates controlling a 3-color LED using a button to cycle through three modes:

  1. Slow Blink: Cycles through red, green, and blue LEDs with a slow blink.
  2. Fast Fade: Smoothly fades a random color in and out.
  3. Fast Blink: Randomly blinks one or more LEDs at a fast rate.

  Additional Features:
  - A speaker connected to pin 17 plays an alert sound on button press.
  - A long press on the button (5 seconds) plays a melody using the notes defined in pitches.h.

  Note:
  - The `pitches.h` file is included to define musical note frequencies for the melody.

  Created by Handy_4ndy.
  This code is free software; you can redistribute it and/or modify it under the terms of the
  GNU Lesser General Public License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
*/

#include "pitches.h"

// Pin definitions
#define RED_PIN 5
#define GREEN_PIN 19
#define BLUE_PIN 18
#define BUTTON_PIN 4  
#define SPEAKER_PIN 17

// Variables for mode switching and timing
int currentMode = 0; // Tracks the current mode (0: Slow Blink, 1: Fast Fade, 2: Fast Blink)
unsigned long lastButtonPressTime = 0; // Tracks the last button press time for debouncing
unsigned long lastModeUpdateTime = 0; // Tracks the last mode update time
const unsigned long debounceDelay = 200; // Debounce delay for button presses
const unsigned long longPressDuration = 5000; // Duration for a long press in milliseconds
const unsigned long slowBlinkInterval = 1000; // Interval for slow blink mode
const unsigned long fastBlinkInterval = 200; // Interval for fast blink mode
const unsigned long fadeInterval = 20; // Interval for fade updates
int lastButtonState = HIGH; // Tracks the last button state
int fadeValue = 0; // Current fade value for Fast Fade mode
int fadeDirection = 1; // Direction of fade (1: increasing, -1: decreasing)
int activeColor = 0; // Active color for Fast Fade mode (0: Red, 1: Green, 2: Blue)
bool isLongPress = false; // Tracks if a long press is detected

void setup() {
  // Initialize pins 
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  pinMode(SPEAKER_PIN, OUTPUT); // Set the speaker pin as output

  // Start serial communication
  Serial.begin(9600); 
  Serial.println("Starting 3-Color LED Program with Alerts");

  // Turn off all LEDs initially
  analogWrite(RED_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN, 0);
}

void loop() {

  // Handle button press events
  handleButtonPress();

  // Execute the current mode if no long press is detected
  if (!isLongPress) {
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
}

// Handles button press events and switches between modes or plays a melody
void handleButtonPress() {

  int buttonState = digitalRead(BUTTON_PIN); 

  // Check if the button state has changed and is pressed (LOW)
  if (buttonState == LOW) { 

    if (millis() - lastButtonPressTime > longPressDuration) {
      if (!isLongPress) {
        isLongPress = true;
        Serial.println("Long press detected. Playing melody...");
        playMelody();
      }
    }
  } else { // Button is released

    // Check if a long press was detected
    if (isLongPress) {
      isLongPress = false; // Reset the long press flag

    } else if (buttonState != lastButtonState && buttonState == HIGH) {

      // Check for button debounce and switch modes
      if (millis() - lastButtonPressTime > debounceDelay) {
        currentMode = (currentMode + 1) % 3;
        lastButtonPressTime = millis();

        lastModeUpdateTime = 0;
        fadeValue = 0;
        fadeDirection = 1;
        activeColor = 0;

        // Turn off all LEDs
        analogWrite(RED_PIN, 0);
        analogWrite(GREEN_PIN, 0);
        analogWrite(BLUE_PIN, 0);

        // Play alert sound
        playAlert();

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
    // Update the last button press time
    lastButtonPressTime = millis(); 
  }

  // Update the last button state
  lastButtonState = buttonState; 
}

// Plays a short alert sound on button press
void playAlert() {
  tone(SPEAKER_PIN, NOTE_C4, 200); // Play a C4 note for 200ms
  delay(200);
  noTone(SPEAKER_PIN); 
}

// Plays a melody on long button press
void playMelody() {
  int melody[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5};
  int noteDurations[] = {4, 4, 4, 4, 4, 4, 4, 4}; // Quarter notes

  for (int i = 0; i < 8; i++) {
    int noteDuration = 200 / noteDurations[i];
    tone(SPEAKER_PIN, melody[i], noteDuration);
    delay(noteDuration * 1.3); // Pause between notes
    noTone(SPEAKER_PIN); 
  }

  // Ensure the speaker is completely turned off after the melody
  noTone(SPEAKER_PIN); 
}

// Implements the Slow Blink mode
void slowBlink() {
  
  if (millis() - lastModeUpdateTime >= slowBlinkInterval) {
    lastModeUpdateTime = millis();
    static int color = 0;

    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);

    if (color == 0) analogWrite(RED_PIN, 255);
    else if (color == 1) analogWrite(GREEN_PIN, 255);
    else if (color == 2) analogWrite(BLUE_PIN, 255);

    color = (color + 1) % 3;
  }
}

// Implements the Fast Fade mode
void fastFade() {

  if (millis() - lastModeUpdateTime >= fadeInterval) {
    lastModeUpdateTime = millis();

    fadeValue += fadeDirection * 5;
    if (fadeValue <= 0 || fadeValue >= 255) {
      fadeDirection *= -1;
      if (fadeValue <= 0) {
        activeColor = random(0, 3);
      }
      fadeValue = constrain(fadeValue, 0, 255);
    }

    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);

    if (activeColor == 0) analogWrite(RED_PIN, fadeValue);
    else if (activeColor == 1) analogWrite(GREEN_PIN, fadeValue);
    else if (activeColor == 2) analogWrite(BLUE_PIN, fadeValue);
  }
}

// Implements the Fast Blink mode
void fastBlink() {

  if (millis() - lastModeUpdateTime >= fastBlinkInterval) {
    lastModeUpdateTime = millis();

    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);

    if (random(0, 2)) analogWrite(RED_PIN, 255);
    if (random(0, 2)) analogWrite(GREEN_PIN, 255);
    if (random(0, 2)) analogWrite(BLUE_PIN, 255);
  }
}
