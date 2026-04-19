#include <Arduino.h>

static const int UART_RX_PIN = 20;
static const int UART_TX_PIN = 21;

String buffer = "";

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);

  Serial.println("ESP READY");
}

void loop() {

  // ===== RECEIVE FROM NIOS =====
  while (Serial1.available()) {
    char c = Serial1.read();

    if (c == '\r') {
      // ignore
    }
    else if (c == '\n') {
      Serial.print("Received: ");
      Serial.println(buffer);

      // ===== MATCH COMMAND =====
      if (buffer == "CALL ME DADDY") {
        Serial.println("Replying...");
        Serial1.println("YES DADDY");
      }

      buffer = "";
    }
    else {
      buffer += c;
    }
  }
}