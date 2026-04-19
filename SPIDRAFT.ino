#include <Arduino.h>
#include <ESP32SPISlave.h>

#define SPI_MODE SPI_MODE0

// XIAO ESP32-C3 SPI pins
#define HSPI_MISO 9
#define HSPI_MOSI 10
#define HSPI_SCLK 8
#define HSPI_SS   7

ESP32SPISlave slave;

// Sentence to send to NIOS master
const char sentence[] = "HELLO FROM ESP32";
size_t sentence_index = 0;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    slave.setDataMode(SPI_MODE);
    slave.setQueueSize(1);

    // Use this if your library version supports explicit pin assignment
    // slave.begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);

    // Otherwise use the default begin()
    slave.begin();

    Serial.println("SPI slave ready!");
    Serial.print("Sentence to send: ");
    Serial.println(sentence);
}

void loop()
{
    // Pick the current character to send
    uint8_t tx_byte = sentence[sentence_index];

    // Queue exactly 1 byte for the next SPI transaction
    slave.queue(&tx_byte, NULL, 1);

    Serial.print("Queued char: ");
    if (tx_byte == '\0') {
        Serial.println("\\0");
    } else {
        Serial.print("'");
        Serial.print((char)tx_byte);
        Serial.println("'");
    }

    // Wait for the master to clock this byte out
    const std::vector<size_t> received_bytes = slave.wait(5000);

    if (!received_bytes.empty()) {
        Serial.print("Master clocked ");
        Serial.print(received_bytes[0]);
        Serial.println(" byte(s)");

        // Move to next character
        sentence_index++;

        // strlen(sentence) does NOT include '\0'
        // so when index becomes greater than strlen, restart
        if (sentence_index > strlen(sentence)) {
            sentence_index = 0;
            Serial.println("Restarting sentence...");
        }
    } else {
        Serial.println("Timeout waiting for SPI master");
    }

    delay(10);
}