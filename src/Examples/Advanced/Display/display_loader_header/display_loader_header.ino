/*
  Display Loader Example
  This script demonstrates how to display a loading screen with a glitch effect on an OLED display using the Adafruit SSD1306 library.

  Created by Handy_4ndy.

  This code is free software; you can redistribute it and/or modify it under the terms of the
  GNU Lesser General Public License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  Libraries:
  - Adafruit GFX
  - Adafruit SSD1306
*/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "XRPL_Logo.h"
#include "Xahau_Logo.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000); // Pause for 2 seconds

  // Clear the buffer.
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(7, 5);
  display.print("Built On");
  display.display();
}

void loop() {
  static unsigned long lastSwitchTime = 0;
  static bool showXRPL = true;
  static bool glitching = false;

  if (millis() - lastSwitchTime > 3000) {
    lastSwitchTime = millis();
    if (glitching) {
      showXRPL = !showXRPL;
    }
    glitching = !glitching;
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(15, 0);
  display.print("Built On");

  if (glitching) {
    // Add glitch effect
    for (int i = 0; i < 20; i++) {
      int x = random(0, SCREEN_WIDTH);
      int y = random(22, SCREEN_HEIGHT);
      int w = random(1, 20);
      int h = random(1, 5);
      display.fillRect(x, y, w, h, random(0, 2));
    }
  } else {
    if (showXRPL) {
      display.drawBitmap(0, 3, Xahau_Logo, 128, 64, 1);
    } else {
      display.drawBitmap(0, 5, XRPL_Logo, 128, 64, 1);
    }
  }

  display.display();
  delay(50); // Short delay to make the glitch effect visible
}