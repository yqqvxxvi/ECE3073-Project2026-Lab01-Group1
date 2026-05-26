#include <Arduino.h>

static const int UART_RX_PIN = 20;
static const int UART_TX_PIN = 21;

String buffer = "";

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP READY");

  Serial1.begin(9600, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
  Serial.println("SERIAL1 READY");
}

void loop() {
  while (Serial1.available() > 0) {
    char c = Serial1.read();

    if (c == '\r') {
      continue;
    }
    else if (c == '\n') {
      Serial.print("Received: ");
      Serial.println(buffer);

      if (buffer == "CALL ME DADDY") {
        Serial.println("Replying...");
        Serial1.println("YES DADDY HOWGEN");
      }

      buffer = "";
    }
    else {
      buffer += c;
    }
  }
}