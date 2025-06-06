#pragma once
#include <Adafruit_SSD1306.h>

namespace loader {
  void show(Adafruit_SSD1306& display, const unsigned char* logo, int logoWidth, int logoHeight);
}