#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>
#include "XRPL_Logo.h"
#include "Xahau_Logo.h"

// XRPL WebSocket server address
const char* websocket_host = "xrplcluster.com";
const uint16_t websocket_port = 443;
const char* websocket_url = "/";

// WiFi credentials
const char* ssid = "Spencer-wifi";
const char* password = "fuckoffu";

// Create a WebSocket client object
WebSocketsClient webSocket;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// OLED display settings (U8g2, I2C)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Block size for drawing transactions
const int BLOCK_SIZE = 1;

// Variables to store the last seen ledger index and counters
int lastLedgerIndex = -1;

int counters[3] = {0, 0, 0};
// int totalTransactions = 0;
// int validatedTransactions = 0;
// int successfulTransactions = 0;
int xPos = 0;
int yPos = 0;

void setup() {
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);

  // Initialize OLED display
  display.begin();
  display.clearBuffer();
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize WebSocket
  webSocket.beginSSL(websocket_host, websocket_port, websocket_url);
  webSocket.onEvent(webSocketEvent);

  showLoadingScreen();
  setupDisplayTemplate();
}

void loop() {
  // Keep the WebSocket connection alive
  webSocket.loop();
}

void showLoadingScreen() {
  unsigned long startTime = millis();
  unsigned long lastSwitchTime = 0;
  bool showXRPL = true;
  bool glitching = false;

  while (millis() - startTime < 6000) {
    if (millis() - lastSwitchTime > 1000) {
      lastSwitchTime = millis();
      if (glitching) {
        showXRPL = !showXRPL;
      }
      glitching = !glitching;
    }

    display.clearBuffer();
    display.setDrawColor(1); // White
    display.setFont(u8g2_font_ncenB14_tr); // Approx size 2
    display.drawStr(15, 15, "Built On"); // Adjusted Y for font size

    if (glitching) {
      // Add glitch effect
      for (int i = 0; i < 20; i++) {
        int x = random(0, 128);
        int y = random(22, 64);
        int w = random(1, 20);
        int h = random(1, 5);
        display.drawBox(x, y, w, h); // Random fill (0 or 1 not supported natively)
      }
    } else {
      if (showXRPL) {
        display.drawXBMP(0, 3, 128, 64, Xahau_Logo); // U8g2 XBM format
      } else {
        display.drawXBMP(0, 5, 128, 64, XRPL_Logo); // U8g2 XBM format
      }
    }

    display.sendBuffer();
    delay(50); // Short delay to make the glitch effect visible
  }

  // Clear the display after the loading screen
  display.clearBuffer();
  display.sendBuffer();
}

void setupDisplayTemplate() {
  display.clearBuffer();
  display.setFont(u8g2_font_5x7_tf); // Approx size 1
  display.setDrawColor(1); // White
  display.drawStr(0, 24, "Index:");
  display.drawStr(0, 34, "Total:");
  display.drawStr(0, 44, "Validated:");
  display.drawStr(0, 54, "tesSUCCESS:");
  display.sendBuffer();
}

void updateDisplayCounters() {
  display.setDrawColor(0); // Black
  display.drawBox(0, 0, 128, 15); // Clear previous index
  display.drawBox(70, 0, 50, 64); // Clear previous counters
  display.setDrawColor(1); // White

  display.setFont(u8g2_font_5x7_tf);
  char buf[16];
  sprintf(buf, "%d", lastLedgerIndex);
  display.drawStr(70, 24, buf);
  sprintf(buf, "%d Txn's", counters[0]);
  display.drawStr(70, 34, buf);
  sprintf(buf, "%d Txn's", counters[1]);
  display.drawStr(70, 44, buf);
  sprintf(buf, "%d Txn's", counters[2]);
  display.drawStr(70, 54, buf);
  display.sendBuffer();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      subscribeToTransactionStream();
      break;
    case WStype_TEXT:
      processTransaction(payload, length);
      break;
    case WStype_BIN:
      Serial.println("Binary message received");
      break;
    case WStype_PING:
      Serial.println("Ping received");
      break;
    case WStype_PONG:
      Serial.println("Pong received");
      break;
  }
}

void subscribeToTransactionStream() {
  // Create a JSON document to subscribe to the transaction stream
  DynamicJsonDocument doc(256);
  doc["command"] = "subscribe";
  JsonArray streams = doc.createNestedArray("streams");
  streams.add("transactions");

  // Serialize the JSON document to a string
  String message;
  serializeJson(doc, message);

  // Send the subscription message via WebSocket
  webSocket.sendTXT(message);
}

void processTransaction(uint8_t* payload, size_t length) {
  // Create a JSON document to hold the incoming payload
  StaticJsonDocument<1024> doc; // Reduced size for optimization
  // Deserialize the JSON payload
  DeserializationError error = deserializeJson(doc, payload, length);

  // Check for deserialization errors
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Increment the total transactions counter
  counters[0]++;

  // Check if the ledger index has changed
  if (doc.containsKey("ledger_index")) {
    int ledgerIndex = doc["ledger_index"].as<int>();
    if (ledgerIndex != lastLedgerIndex) {
      // Log the totals for the previous ledger index
      if (lastLedgerIndex != -1) {
        updateDisplayCounters();
      }

      // Reset the counters for the new ledger index
      lastLedgerIndex = ledgerIndex;
      counters[0] = 1; // Start with 1 for the current transaction
      counters[1] = 0;
      counters[2] = 0;
      xPos = 0;
      yPos = 0;
    }
  }

  // Check if the transaction is validated and successful
  if (doc.containsKey("validated") && doc["validated"].as<bool>()) {
    counters[1]++;

    if (doc.containsKey("meta") && doc["meta"].containsKey("TransactionResult")) {
      const char* transactionResult = doc["meta"]["TransactionResult"];
      if (strcmp(transactionResult, "tesSUCCESS") == 0) {
        counters[2]++;
      }
    }
  }

  // Draw a block for each transaction (faster with U8g2)
  display.setDrawColor(1); // White
  display.drawBox(xPos, yPos, BLOCK_SIZE, BLOCK_SIZE);
  display.sendBuffer(); // ~20ms for small update

  // Update the position for the next block
  xPos = (xPos + BLOCK_SIZE) % SCREEN_WIDTH;
  if (xPos == 0) {
    yPos = (yPos + BLOCK_SIZE) % SCREEN_HEIGHT;
  }

  // // Update the display less frequently
  // if (counters[0] % 10 == 0) {
  //   display.sendBuffer();
  // }
}