#pragma once
#include <Adafruit_SSD1306.h>

namespace WiFiConnection {
  void connect(Adafruit_SSD1306& display);
  bool isConnected();
}
