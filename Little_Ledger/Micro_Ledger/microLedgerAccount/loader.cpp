#include "loader.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

namespace {
  void glitchEffect(Adafruit_SSD1306& display) {
    unsigned long glitchStart = millis();
    while (millis() - glitchStart < 5000) { // Run for 5 seconds
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(2);
      display.setCursor(15, 0);
      display.print("Built On");

      // Subtle flicker and distortion
      for (int i = 0; i < 5; i++) {
        int xOffset = random(-2, 3); // Small horizontal jitter
        int yOffset = random(-2, 3); // Small vertical jitter
        int x = 15 + xOffset;
        int y = 0 + yOffset;
        display.setCursor(x, y);
        display.print("Built On");
      }

      // Random horizontal lines for glitch aesthetic
      for (int i = 0; i < 3; i++) {
        int y = random(20, 64); // SCREEN_HEIGHT = 64
        int length = random(20, 128); // SCREEN_WIDTH = 128
        display.drawFastHLine(0, y, length, WHITE);
      }

      display.display();
      delay(100); // Update every 100ms for smoother animation
    }
  }
}

void loader::show(Adafruit_SSD1306& display, const unsigned char* logo, int logoWidth, int logoHeight) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(15, 0);
  display.print("Built On");
  glitchEffect(display);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(15, 0);
  display.print("Built On");
  display.drawBitmap(0, 3, logo, logoWidth, logoHeight, 1);
  display.display();
  delay(3000);
  display.clearDisplay();
}