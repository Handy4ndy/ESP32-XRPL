/*
  Basic Audio Alert Example
  This script demonstrates how to use audio alerts for different events using a speaker or buzzer.

  Created by Handy_4ndy.

  This code is free software; you can redistribute it and/or modify it under the terms of the
  GNU Lesser General Public License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  Header file:
  - pitches.h: Contains note definitions for melodies.
*/

#include <Arduino.h>
#include "pitches.h" // Include the pitches header file for note definitions

// Pin definitions
const int SPEAKER_PIN = 4; // Speaker pin

void setup() {
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);

  // Set pin mode
  pinMode(SPEAKER_PIN, OUTPUT);
}

void loop() {
  // Example usage of audio alerts
  playTune1();
  delay(2000); // Pause for 2 seconds

  playTune2();
  delay(2000); // Pause for 2 seconds

  playHighScoreTune();
  delay(2000); // Pause for 2 seconds
}

void playTune1() {
  int melody[] = { NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_E4, NOTE_C4 };
  playMelody(melody, 7); 
}

void playTune2() {
  int melody[] = { NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5 };
  playMelody(melody, 7); 
}

void playHighScoreTune() {
  int melody[] = { NOTE_E5, NOTE_G5, NOTE_B5, NOTE_E6, NOTE_B5, NOTE_G5, NOTE_E5 };
  playMelody(melody, 7); 
}

// Function to play a melody
void playMelody(int* melody, int size) {
  // Define the duration of each note (eighth notes)
  int noteDurations[] = { 16, 16, 16, 16, 16, 16, 16 }; 

  // Loop through each note in the melody
  for (int thisNote = 0; thisNote < size; thisNote++) {
    // Calculate the note duration in milliseconds
    int noteDuration = 1000 / noteDurations[thisNote];
    // Play the note on the speaker
    tone(SPEAKER_PIN, melody[thisNote], noteDuration);

    // Pause between notes
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // Stop the tone
    noTone(SPEAKER_PIN);
  }
}
