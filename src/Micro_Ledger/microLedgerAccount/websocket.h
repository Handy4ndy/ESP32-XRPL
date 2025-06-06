#pragma once
#include <Adafruit_SSD1306.h>

// Callback type for WebSocket messages
using WebSocketMessageCallback = void (*)(const uint8_t* payload, size_t length);

namespace WebSocketConnection {
  void begin(Adafruit_SSD1306& display, const char* host, uint16_t port, const char* url, WebSocketMessageCallback callback = nullptr);
  void loop();
  bool isConnected();
  void send(const char* message); // New function to send WebSocket messages
  void disconnect();
}