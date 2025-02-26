/*
  Advanced Display Charts Example
  This script demonstrates various types of charts and chart animations on an OLED display using the Adafruit SSD1306 library.

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

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Initialize the display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000); // Pause for 2 seconds
}

void loop() {
  drawBarChart();
  delay(2000); // Pause for 2 seconds

  drawLineChart();
  delay(2000); // Pause for 2 seconds

  animateBarChart();
  delay(2000); // Pause for 2 seconds

  animateLineChart();
  delay(2000); // Pause for 2 seconds
}

void drawBarChart() {
  // Clear the buffer
  display.clearDisplay();
  
  // Draw the axes
  display.drawLine(10, 10, 10, 54, SSD1306_WHITE); // Y-axis
  display.drawLine(10, 54, 118, 54, SSD1306_WHITE); // X-axis
  
  // Sample data for the bar chart
  int data[] = {10, 20, 15, 30, 25, 35, 40};
  int numBars = sizeof(data) / sizeof(data[0]);
  int barWidth = 8;
  int maxBarHeight = 40;
  
  // Draw the bars
  for (int i = 0; i < numBars; i++) {
    int barHeight = map(data[i], 0, 40, 0, maxBarHeight);
    display.fillRect(12 + i * (barWidth + 2), 54 - barHeight, barWidth, barHeight, SSD1306_WHITE);
  }
  
  // Display the buffer
  display.display();
}

void drawLineChart() {
  // Clear the buffer
  display.clearDisplay();
  
  // Draw the axes
  display.drawLine(10, 16, 10, 54, SSD1306_WHITE); // Y-axis
  display.drawLine(10, 54, 118, 54, SSD1306_WHITE); // X-axis
  
  // Sample data for the line chart
  int data[] = {5, 10, 5, 18, 40, 35, 10, 15, 45};
  int numPoints = sizeof(data) / sizeof(data[0]);
  int maxPointHeight = 45;
  
  // Draw the line chart
  for (int i = 0; i < numPoints - 1; i++) {
    int x1 = 12 + i * 10;
    int y1 = 54 - map(data[i], 0, 40, 0, maxPointHeight);
    int x2 = 12 + (i + 1) * 10;
    int y2 = 54 - map(data[i + 1], 0, 40, 0, maxPointHeight);
    display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
  }
  
  // Display the buffer
  display.display();
}

void animateBarChart() {
  // Clear the buffer
  display.clearDisplay();
  
  // Draw the axes
  display.drawLine(10, 10, 10, 54, SSD1306_WHITE); // Y-axis
  display.drawLine(10, 54, 118, 54, SSD1306_WHITE); // X-axis
  
  // Sample data for the bar chart
  int data[] = {10, 20, 15, 30, 25, 35, 40};
  int numBars = sizeof(data) / sizeof(data[0]);
  int barWidth = 8;
  int maxBarHeight = 40;
  
  // Animate the bars
  for (int i = 0; i < numBars; i++) {
    for (int h = 0; h <= data[i]; h++) {
      int barHeight = map(h, 0, 40, 0, maxBarHeight);
      display.fillRect(12 + i * (barWidth + 2), 54 - barHeight, barWidth, barHeight, SSD1306_WHITE);
      display.display();
      delay(50);
    }
  }
}

void animateLineChart() {
  // Clear the buffer
  display.clearDisplay();
  
  // Draw the axes
  display.drawLine(10, 16, 10, 54, SSD1306_WHITE); // Y-axis
  display.drawLine(10, 54, 118, 54, SSD1306_WHITE); // X-axis
  
  // Sample data for the line chart
  int data[] = {5, 10, 5, 18, 40, 35, 10, 15, 45};
  int numPoints = sizeof(data) / sizeof(data[0]);
  int maxPointHeight = 45;
  
  // Animate the line chart
  for (int i = 0; i < numPoints - 1; i++) {
    int x1 = 12 + i * 10;
    int y1 = 54 - map(data[i], 0, 40, 0, maxPointHeight);
    int x2 = 12 + (i + 1) * 10;
    int y2 = 54 - map(data[i + 1], 0, 40, 0, maxPointHeight);
    display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
    display.display();
    delay(1000);
  }
}