/*
  RGB LED with Button Control, Speaker Alerts, and OLED Display
  This program demonstrates controlling an RGB LED using a button to cycle through three modes:

  1. Slow Blink: Cycles through red, green, and blue LEDs with a slow blink.
  2. Fast Fade: Smoothly fades a random color in and out.
  3. Fast Blink: Randomly blinks one or more LEDs at a fast rate.

  Additional Features:
  - An OLED display shows a loading image at startup and displays the current mode.

  Libraries and Header Files:
  - `Adafruit_GFX.h` and `Adafruit_SSD1306.h` for controlling the OLED display.
  - `pitches.h` for defining musical note frequencies.
  - `XRPL_Logo.h` for displaying the XRPL logo on the OLED screen.

  Created by Handy_4ndy.
  This code is free software; you can redistribute it and/or modify it under the terms of the
  GNU Lesser General Public License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
*/

// To complete the assembly of the developer_kit we are going to add in the display to do so we have reassigned the RED_Pin to pin 4 to free up the SLA con
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "pitches.h"
#include "XRPL_Logo.h"

// Pin definitions
#define RED_PIN 5
#define GREEN_PIN 19
#define BLUE_PIN 18
#define BUTTON_PIN 4
#define SPEAKER_PIN 17

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables for mode switching and timing
int currentMode = 0;
unsigned long lastButtonPressTime = 0;
unsigned long lastModeUpdateTime = 0;
const unsigned long debounceDelay = 200;
const unsigned long longPressDuration = 5000; // Duration for a long press in milliseconds
const unsigned long slowBlinkInterval = 1000;
const unsigned long fastBlinkInterval = 200;
const unsigned long fadeInterval = 20;
int lastButtonState = HIGH;
int fadeValue = 0;
int fadeDirection = 1;
int activeColor = 0;
bool isLongPress = false;

void setup() {
  // Initialize pins 
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  pinMode(SPEAKER_PIN, OUTPUT);

  // Start serial communication
  Serial.begin(9600);
  Serial.println("Starting 3-Color LED Program with Alerts");

  // Turn off all LEDs initially
  analogWrite(RED_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN, 0);

  // Initialize SSD1306 display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C is the I2C address for the SSD1306
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  // Ensure the speaker is completely turned off after the melody
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(17, 0);
  display.print("Built On");
  display.drawBitmap(0, 0, XRPL_Logo, 128, 64, 1);
  display.setTextSize(1);
  display.display();
  delay(4000); // Pause for 5 seconds to show the logo

  // Clear the display and set the text size for the main screen
  display.clearDisplay(); 
  display.setTextSize(1);
  display.display();
}


void loop() {

  handleButtonPress();

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

  // Button is pressed (LOW)
  if (buttonState == LOW) { 
    if (millis() - lastButtonPressTime > longPressDuration) {

      // Check if the long press duration has been reached
      if (!isLongPress) {
        isLongPress = true;
        Serial.println("Long press detected. Playing melody...");
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setCursor(8, 0);
        display.print("Long press detected");
        display.setCursor(25, 18);
        display.print("Playing melody");
        display.display();
        playMelody();
      }
    }
  } else { 
    
    // Button is released
    if (isLongPress) {
      isLongPress = false; 

      // Check if the button was released after a long press andd play the alert
    } else if (buttonState != lastButtonState && buttonState == HIGH) {
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

        Serial.print("Button pressed: Switching to mode ");

        // update the display with the current mode
        switch (currentMode) {
          case 0:
            Serial.println("slowBlink");
            display.clearDisplay();
            display.setTextColor(WHITE);
            display.setTextSize(1);
            display.setCursor(25, 0);
            display.print("Button Pressed");
            display.setCursor(15, 30);
            display.print("Slow Blink Active");
            display.display();
            break;
          case 1:
            Serial.println("fastFade");
            display.clearDisplay();
            display.setTextColor(WHITE);
            display.setCursor(25, 0);
            display.print("Button Pressed");
            display.setCursor(23, 30);
            display.print("fastFade Active");
            display.display();
            break;
          case 2:
            Serial.println("fastBlink");
            display.clearDisplay();
            display.setTextColor(WHITE);
            display.setCursor(25, 0);
            display.print("Button Pressed");
            display.setCursor(20, 30);
            display.print("fastBlink Active");
            display.display();
            break;
        }
      }
    }

    // Reset the last button press time when released
    lastButtonPressTime = millis(); 
  }

  // Update the last button state
  lastButtonState = buttonState; 
}

// Plays a short alert sound on button press
void playAlert() {
  tone(SPEAKER_PIN, NOTE_C4, 200); 
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
