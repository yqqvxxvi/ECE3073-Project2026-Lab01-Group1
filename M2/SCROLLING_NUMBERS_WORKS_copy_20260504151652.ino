#include <Arduino.h>
#include <Seeed_Arduino_SSCMA.h>
#include <ESP32SPISlave.h>
#include <vector>
#include <algorithm>

#define SPI_MODE SPI_MODE0

SSCMA AI;
ESP32SPISlave slave;

/*
   Water meter digit detector.

   Expected classes:
   target 0 -> zero
   target 1 -> one
   target 2 -> two
   target 3 -> three
   target 4 -> four
   target 5 -> five
   target 6 -> six
   target 7 -> seven
   target 8 -> eight
   target 9 -> nine

   IMPORTANT:
   Check Serial Monitor once.
   If the model target numbers are different, only change targetToDigit().
*/

#define SCORE_THRESHOLD 50
#define MAX_DIGITS 10
#define FRAME_BUFFER_SIZE 16

/*
   Shared SPI frame.

   Example:
   latest_frame = "02012\n"

   If no detection:
   latest_frame = "N\n"
*/
char latest_frame[FRAME_BUFFER_SIZE] = "N\n";
volatile int latest_frame_length = 2;

portMUX_TYPE frameMux = portMUX_INITIALIZER_UNLOCKED;

/*
   SPI sending position.
   This advances one byte every time FPGA clocks a byte.
*/
int spi_tx_index = 0;

char targetToDigit(int target)
{
    if (target == 0) return '0';
    if (target == 1) return '1';
    if (target == 2) return '2';
    if (target == 3) return '3';
    if (target == 4) return '4';
    if (target == 5) return '5';
    if (target == 6) return '6';
    if (target == 7) return '7';
    if (target == 8) return '8';
    if (target == 9) return '9';

    return '?';
}

const char* targetToName(int target)
{
    if (target == 0) return "ZERO";
    if (target == 1) return "ONE";
    if (target == 2) return "TWO";
    if (target == 3) return "THREE";
    if (target == 4) return "FOUR";
    if (target == 5) return "FIVE";
    if (target == 6) return "SIX";
    if (target == 7) return "SEVEN";
    if (target == 8) return "EIGHT";
    if (target == 9) return "NINE";

    return "UNKNOWN";
}

/*
   Box structure used for sorting detected digits from left to right.
*/
struct DigitBox {
    int target;
    int score;
    int x;
    int y;
    int w;
    int h;
};

/*
   Update shared frame safely.
*/
void updateLatestFrame(const char *new_frame)
{
    portENTER_CRITICAL(&frameMux);

    strncpy(latest_frame, new_frame, FRAME_BUFFER_SIZE - 1);
    latest_frame[FRAME_BUFFER_SIZE - 1] = '\0';
    latest_frame_length = strlen(latest_frame);

    /*
       Reset SPI index when a new frame is ready.
       This makes the next FPGA reads start from the first digit.
    */
    spi_tx_index = 0;

    portEXIT_CRITICAL(&frameMux);
}

/*
   Task 1:
   Run Grove AI inference.
   Collect all digit detections.
   Sort them left-to-right.
   Build ASCII frame.
*/
void aiTask(void *parameter)
{
    Serial.println("Starting Grove AI water meter task...");

    if (!AI.begin()) {
        Serial.println("ERROR: Grove AI begin failed!");
        Serial.println("Check Grove Vision AI connection.");
        while (1) {
            updateLatestFrame("N\n");
            delay(1000);
        }
    }

    Serial.println("Grove AI connected successfully.");

    char previous_frame[FRAME_BUFFER_SIZE] = "";

    while (1) {
        int ret = AI.invoke();

        /*
           In your previous working code:
           ret == 0 means success.
        */
        if (ret != 0) {
            updateLatestFrame("N\n");

            Serial.print("invoke failed, ret = ");
            Serial.println(ret);

            delay(100);
            continue;
        }

        std::vector<DigitBox> digits;

        if (AI.boxes().size() > 0) {

            for (int i = 0; i < AI.boxes().size(); i++) {
                int target = AI.boxes()[i].target;
                int score  = AI.boxes()[i].score;

                if (score < SCORE_THRESHOLD) {
                    continue;
                }

                char digit = targetToDigit(target);

                if (digit == '?') {
                    Serial.print("Unknown target from model: ");
                    Serial.println(target);
                    continue;
                }

                DigitBox d;
                d.target = target;
                d.score  = score;

                /*
                   These field names are commonly used by SSCMA boxes.

                   If your library uses different names, print AI.boxes()[i]
                   fields from your old working examples and adjust here.

                   Common possibilities are:
                   x, y, w, h
                   or x_min, y_min, x_max, y_max.

                   For Seeed_Arduino_SSCMA, x/y/w/h is usually correct.
                */
                d.x = AI.boxes()[i].x;
                d.y = AI.boxes()[i].y;
                d.w = AI.boxes()[i].w;
                d.h = AI.boxes()[i].h;

                digits.push_back(d);
            }

            if (digits.size() > 0) {

                /*
                   Sort detected digits from left to right.

                   This is the key change compared with hand gesture detection.
                */
                std::sort(digits.begin(), digits.end(),
                    [](const DigitBox &a, const DigitBox &b) {
                        return a.x < b.x;
                    }
                );

                char frame[FRAME_BUFFER_SIZE];
                int index = 0;

                for (int i = 0; i < digits.size() && index < FRAME_BUFFER_SIZE - 2; i++) {
                    frame[index++] = targetToDigit(digits[i].target);
                }

                frame[index++] = '\n';
                frame[index] = '\0';

                updateLatestFrame(frame);

                /*
                   Print only when reading changes.
                */
                if (strcmp(frame, previous_frame) != 0) {
                    strcpy(previous_frame, frame);

                    Serial.print("Detected water meter reading: ");
                    Serial.print(frame);

                    Serial.println("Digit details:");

                    for (int i = 0; i < digits.size(); i++) {
                        Serial.print("  digit=");
                        Serial.print(targetToDigit(digits[i].target));
                        Serial.print(" target=");
                        Serial.print(digits[i].target);
                        Serial.print(" name=");
                        Serial.print(targetToName(digits[i].target));
                        Serial.print(" score=");
                        Serial.print(digits[i].score);
                        Serial.print(" x=");
                        Serial.print(digits[i].x);
                        Serial.print(" y=");
                        Serial.print(digits[i].y);
                        Serial.print(" w=");
                        Serial.print(digits[i].w);
                        Serial.print(" h=");
                        Serial.println(digits[i].h);
                    }
                }
            }
            else {
                updateLatestFrame("N\n");

                if (strcmp(previous_frame, "N\n") != 0) {
                    strcpy(previous_frame, "N\n");
                    Serial.println("No valid digit detection | SPI frame=N");
                }
            }
        }
        else {
            updateLatestFrame("N\n");

            if (strcmp(previous_frame, "N\n") != 0) {
                strcpy(previous_frame, "N\n");
                Serial.println("No detection | SPI frame=N");
            }
        }

        delay(50);
    }
}

/*
   Task 2:
   SPI slave task.

   FPGA is master.
   ESP32 is slave.

   FPGA clocks one dummy byte at a time.
   ESP32 returns the next byte of latest_frame.

   Example returned sequence:
   '0', '2', '0', '1', '2', '\n'
*/
void spiTask(void *parameter)
{
    Serial.println("Starting SPI slave task...");

    slave.setDataMode(SPI_MODE);
    slave.setQueueSize(1);

    /*
       Keep the same style as your working SPI example.
    */
    slave.begin();

    Serial.println("SPI slave ready.");
    Serial.println("FPGA can now read water meter frame byte by byte.");

    while (1) {
        uint8_t tx_byte;
        uint8_t rx_byte = 0;

        portENTER_CRITICAL(&frameMux);

        if (spi_tx_index >= latest_frame_length) {
            spi_tx_index = 0;
        }

        tx_byte = latest_frame[spi_tx_index];
        spi_tx_index++;

        portEXIT_CRITICAL(&frameMux);

        slave.queue(&tx_byte, &rx_byte, 1);

        const std::vector<size_t> received_bytes = slave.wait(20);

        delayMicroseconds(200);
    }
}

void setup()
{
    Serial.begin(9600);
    delay(2000);

    Serial.println();
    Serial.println("==========================================");
    Serial.println("Grove AI Water Meter -> ESP32 -> FPGA SPI");
    Serial.println("==========================================");

    Serial.println("Target map:");
    Serial.println("target 0 = ZERO  -> digit 0");
    Serial.println("target 1 = ONE   -> digit 1");
    Serial.println("target 2 = TWO   -> digit 2");
    Serial.println("target 3 = THREE -> digit 3");
    Serial.println("target 4 = FOUR  -> digit 4");
    Serial.println("target 5 = FIVE  -> digit 5");
    Serial.println("target 6 = SIX   -> digit 6");
    Serial.println("target 7 = SEVEN -> digit 7");
    Serial.println("target 8 = EIGHT -> digit 8");
    Serial.println("target 9 = NINE  -> digit 9");
    Serial.println("none/low score   -> frame N");
    Serial.println();

    updateLatestFrame("N\n");

    xTaskCreate(
        aiTask,
        "AI Task",
        10000,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        spiTask,
        "SPI Task",
        4096,
        NULL,
        2,
        NULL
    );
}

void loop()
{
    delay(1000);
}