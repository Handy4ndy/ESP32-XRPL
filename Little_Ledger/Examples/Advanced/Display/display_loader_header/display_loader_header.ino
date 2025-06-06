/*
  Display Loader Example
  Modified to repeat: START > Glitch > logoSWAP > haptic > Glitch > logo > haptic > Glitch > Alert
  Improved glitch effect, 5 seconds per glitch step
*/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "XRPL_Logo.h"
#include "Xahau_Logo.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define HAPTIC_PIN 8  // GPIO8

void setup() {
  Serial.begin(9600);
   
  pinMode(HAPTIC_PIN, OUTPUT);
  digitalWrite(HAPTIC_PIN, LOW);
  
  Serial.println("Screen starting...");

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000); // Pause for 2 seconds
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(7, 5);
  display.print("Built On");
  display.display();
}

// Improved glitch effect function (5-second duration)
void glitchEffect() {
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
      int y = random(20, SCREEN_HEIGHT);
      int length = random(20, SCREEN_WIDTH);
      display.drawFastHLine(0, y, length, WHITE);
    }

    display.display();
    delay(100); // Update every 100ms for smoother animation
  }
}

void loop() {
  static unsigned long lastSwitchTime = 0;
  static bool showXRPL = true;
  static int step = 0; // Track sequence steps
  const int shortStepDuration = 100; // Non-glitch steps (0.5s)
  const int glitchStepDuration = 3000; // Glitch steps (5s)

  // Adjust duration based on whether it's a glitch step
  int currentDuration = (step == 1 || step == 4 || step == 7) ? glitchStepDuration : shortStepDuration;

  if (millis() - lastSwitchTime >= currentDuration) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(15, 0);
    display.print("Built On");

    switch (step) {
      case 0: // START
        // Just show "Built On" (already set above)
        break;

      case 1: // Glitch
        glitchEffect();
        break;

      case 2: // logoSWAP
        display.drawBitmap(0, 5, XRPL_Logo, 128, 64, 1);
        break;

      case 3: // haptic
        display.drawBitmap(0, 5, XRPL_Logo, 128, 64, 1);
        Serial.println("Pattern 2: Short bursts (5x 0.5s on/off)");
        for (int i = 0; i < 5; i++) {
          // digitalWrite(HAPTIC_PIN, HIGH);
          // delay(100);
          // digitalWrite(HAPTIC_PIN, LOW);
          // delay(100);
        }
        break;

      case 4: // Glitch
        glitchEffect();
        break;

      case 5: // logo
        display.drawBitmap(0, 3, Xahau_Logo, 128, 64, 1);
        break;

      case 6: // haptic
        display.drawBitmap(0, 3, Xahau_Logo, 128, 64, 1);
        Serial.println("Pattern 2: Short bursts (5x 0.5s on/off)");
        for (int i = 0; i < 5; i++) {
          // digitalWrite(HAPTIC_PIN, HIGH);
          // delay(100);
          // digitalWrite(HAPTIC_PIN, LOW);
          // delay(100);
        }
        break;

      case 7: // Glitch
        glitchEffect();
        break;

    }

    // Only call display.display() for non-glitch steps (glitchEffect handles its own)
    if (step != 1 && step != 4 && step != 7) {
      display.display();
    }

    lastSwitchTime = millis();
    step = (step + 1) % 8; // Loop back to 0 after 8
  }
}