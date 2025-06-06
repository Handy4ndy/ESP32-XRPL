#include <esp_sleep.h>

#define BUTTON_PIN 9          // GPIO pin connected to the button ( must be RTC capable )
#define LONG_PRESS_TIME 2000  // Time in milliseconds to differentiate long and short presses
#define DEBOUNCE_DELAY 50     // Debounce delay in milliseconds

void setup() {
    Serial.begin(9600);       // Initialize serial communication at 9600 baud
    delay(100);               // Short delay to stabilize the system
    pinMode(BUTTON_PIN, INPUT_PULLUP);  // Configure the button pin as input with pull-up resistor

    Serial.println("ESP Starting");

    // Check if the ESP32 woke up from deep sleep
    if (esp_reset_reason() == ESP_RST_DEEPSLEEP) {
        Serial.println("Woke up from deep sleep");
        
        // Wait until the button is released after waking up
        while (digitalRead(BUTTON_PIN) == LOW) {
            delay(10);
        }
        Serial.println("Button released after wake-up");
    } else {
        Serial.println("Board started or restarted");  // Normal startup or reset
    }
}

void loop() {
    static unsigned long lastDebounceTime = 0; // Timestamp of the last button state change
    static bool buttonState = HIGH;           // Current stable state of the button

    // Read the button state
    bool reading = digitalRead(BUTTON_PIN);

    // Check if the button state has changed
    if (reading != buttonState && (millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
        lastDebounceTime = millis(); // Update the debounce timer
        buttonState = reading;       // Update the stable button state

        // If the button is pressed (LOW state)
        if (buttonState == LOW) {
            unsigned long startTime = millis(); // Record the time when the button press started

            // Wait until the button is released
            while (digitalRead(BUTTON_PIN) == LOW) {
                delay(10);
            }
            unsigned long duration = millis() - startTime; // Calculate the press duration

            // Determine if it was a short or long press
            if (duration < LONG_PRESS_TIME) {
                Serial.println("Short press - Resetting");
                Serial.flush();  // Ensure all serial data is sent before restarting
                ESP.restart();   // Restart the ESP32
            } else {
                Serial.println("Long press - Entering deep sleep");
                Serial.flush();  // Ensure all serial data is sent before sleeping
                esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, 0);  // Configure wake-up on button press (LOW)
                esp_deep_sleep_start();  // Enter deep sleep mode
            }
        }
    }
    delay(10); // Small delay to prevent busy looping
}