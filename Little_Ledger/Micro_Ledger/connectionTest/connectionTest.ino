/*
This connection test script is designed to test the functionality of the Micro_Ledger device by making use of all of the connections. 
If you experiance any issues whilst running this script reach out to use through our support channel - https://xspence.co.uk/contact.html
*/

// ESP32_Project.ino
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin definitions
#define BUTTON_PIN 9
#define HAPTIC_PIN 8
#define SPEAKER_PIN 7
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// I2C pins (SCL = GPIO 4, SDA = GPIO 5 by default)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Alert class definition
class Alert {
  private:
    const int hapticPin;
    const int speakerPin;

  public:
    Alert() : hapticPin(HAPTIC_PIN), speakerPin(SPEAKER_PIN) {
      pinMode(hapticPin, OUTPUT);
      pinMode(speakerPin, OUTPUT);
    }
    
    void triggerAlert(int duration, int frequency) {
      digitalWrite(hapticPin, HIGH);      // Turn on haptic motor
      tone(speakerPin, frequency, duration); // Play tone on speaker
      delay(duration);                    // Wait for the duration
      digitalWrite(hapticPin, LOW);       // Turn off haptic motor
    }
};

// Create Alert object
Alert alert;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("ESP32-S3 Ready");
  display.display();
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    alert.triggerAlert(200, 500);    
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Button Pressed!");
    display.display();
    
    delay(200); // Debounce
  } else {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Waiting...");
    display.display();
  }
  delay(10); // Stability
}
