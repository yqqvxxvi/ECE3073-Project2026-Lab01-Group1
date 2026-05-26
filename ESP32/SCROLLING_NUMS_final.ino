// #include <Arduino.h>
// #include <Seeed_Arduino_SSCMA.h>
// #include <ESP32SPISlave.h>
// #include <vector>
// #include <algorithm>

// #define SPI_MODE SPI_MODE0

// SSCMA AI;
// ESP32SPISlave slave;

// /*
//    Water meter digit detector.

//    Expected classes:
//    target 0 -> zero
//    target 1 -> one
//    target 2 -> two
//    target 3 -> three
//    target 4 -> four
//    target 5 -> five
//    target 6 -> six
//    target 7 -> seven
//    target 8 -> eight
//    target 9 -> nine
// */

// #define SCORE_THRESHOLD 75
// #define REQUIRED_DIGITS 5

// /*
//    Nios expects newline-terminated frames.

//    Valid frame:
//        1 2 3 4 5 \n

//    So SPI_FRAME_LEN = 6.
// */
// #define SPI_FRAME_LEN 6
// #define IDLE_SPI_BYTE 0xFF

// /*
//    Shared SPI frame.

//    This is exactly what the ESP32 cycles through:
//        latest_frame[0]
//        latest_frame[1]
//        latest_frame[2]
//        latest_frame[3]
//        latest_frame[4]
//        latest_frame[5]

//    Example:
//        "12345\n"

//    Before first valid detection, this sends 0xFF bytes.
// */
// char latest_frame[SPI_FRAME_LEN] = {
//     IDLE_SPI_BYTE,
//     IDLE_SPI_BYTE,
//     IDLE_SPI_BYTE,
//     IDLE_SPI_BYTE,
//     IDLE_SPI_BYTE,
//     IDLE_SPI_BYTE
// };

// volatile int latest_frame_length = SPI_FRAME_LEN;
// volatile bool valid_frame_available = false;

// portMUX_TYPE frameMux = portMUX_INITIALIZER_UNLOCKED;

// int spi_tx_index = 0;

// char targetToDigit(int target)
// {
//     if (target == 0) return '0';
//     if (target == 1) return '1';
//     if (target == 2) return '2';
//     if (target == 3) return '3';
//     if (target == 4) return '4';
//     if (target == 5) return '5';
//     if (target == 6) return '6';
//     if (target == 7) return '7';
//     if (target == 8) return '8';
//     if (target == 9) return '9';

//     return '?';
// }

// const char* targetToName(int target)
// {
//     if (target == 0) return "ZERO";
//     if (target == 1) return "ONE";
//     if (target == 2) return "TWO";
//     if (target == 3) return "THREE";
//     if (target == 4) return "FOUR";
//     if (target == 5) return "FIVE";
//     if (target == 6) return "SIX";
//     if (target == 7) return "SEVEN";
//     if (target == 8) return "EIGHT";
//     if (target == 9) return "NINE";

//     return "UNKNOWN";
// }

// struct DigitBox {
//     int target;
//     int score;
//     int x;
//     int y;
//     int w;
//     int h;
// };

// /*
//    Set idle frame before first valid detection.

//    Your Nios code ignores 0xFF:
//        #define IDLE_SPI_BYTE 0xFF
// */
// void setIdleFrame()
// {
//     portENTER_CRITICAL(&frameMux);

//     for (int i = 0; i < SPI_FRAME_LEN; i++) {
//         latest_frame[i] = IDLE_SPI_BYTE;
//     }

//     latest_frame_length = SPI_FRAME_LEN;
//     valid_frame_available = false;

//     /*
//        This reset is okay only during startup/idle setup.
//        Do not repeatedly call this once a valid frame exists.
//     */
//     spi_tx_index = 0;

//     portEXIT_CRITICAL(&frameMux);
// }

// /*
//    Update frame only when a new valid 5-digit number is available.

//    IMPORTANT:
//    This function does NOT reset spi_tx_index.

//    Reason:
//    If we reset spi_tx_index every AI frame, Nios may never receive '\n',
//    causing reading_buffer overflow.
// */
// void updateLatestValidFrameNoReset(const char *five_digits)
// {
//     portENTER_CRITICAL(&frameMux);

//     for (int i = 0; i < REQUIRED_DIGITS; i++) {
//         latest_frame[i] = five_digits[i];
//     }

//     latest_frame[REQUIRED_DIGITS] = '\n';

//     latest_frame_length = SPI_FRAME_LEN;
//     valid_frame_available = true;

//     /*
//        DO NOT DO THIS HERE:
//            spi_tx_index = 0;

//        Let the SPI task keep cycling naturally through:
//            digit digit digit digit digit newline
//     */

//     portEXIT_CRITICAL(&frameMux);
// }

// void printFiveDigitReading(const char *reading)
// {
//     Serial.println();
//     Serial.println("===== EXACTLY 5 DIGITS DETECTED =====");
//     Serial.print("Reading: ");
//     Serial.println(reading);
//     Serial.println("SPI frame: 5 digits + newline");
//     Serial.println("=====================================");
//     Serial.println();
// }

// void aiTask(void *parameter)
// {
//     Serial.println("Starting Grove AI water meter task...");

//     if (!AI.begin()) {
//         Serial.println("ERROR: Grove AI begin failed!");
//         Serial.println("Check Grove Vision AI connection.");

//         while (1) {
//             /*
//                Before the first valid detection, keep idle.
//             */
//             if (!valid_frame_available) {
//                 setIdleFrame();
//             }

//             delay(1000);
//         }
//     }

//     Serial.println("Grove AI connected successfully.");
//     Serial.print("Confidence threshold = ");
//     Serial.println(SCORE_THRESHOLD);
//     Serial.print("Required digit count = ");
//     Serial.println(REQUIRED_DIGITS);

//     char previous_good_frame[REQUIRED_DIGITS + 1] = "";

//     while (1) {
//         int ret = AI.invoke();

//         if (ret != 0) {
//             Serial.print("invoke failed, ret = ");
//             Serial.println(ret);

//             /*
//                Do not modify SPI frame on invoke failure.
//                Keep previous valid reading, or idle if none exists.
//             */
//             delay(100);
//             continue;
//         }

//         std::vector<DigitBox> digits;

//         if (AI.boxes().size() > 0) {
//             for (int i = 0; i < AI.boxes().size(); i++) {
//                 int target = AI.boxes()[i].target;
//                 int score  = AI.boxes()[i].score;

//                 if (score < SCORE_THRESHOLD) {
//                     continue;
//                 }

//                 char digit = targetToDigit(target);

//                 if (digit == '?') {
//                     Serial.print("Unknown target from model: ");
//                     Serial.println(target);
//                     continue;
//                 }

//                 DigitBox d;
//                 d.target = target;
//                 d.score  = score;

//                 d.x = AI.boxes()[i].x;
//                 d.y = AI.boxes()[i].y;
//                 d.w = AI.boxes()[i].w;
//                 d.h = AI.boxes()[i].h;

//                 digits.push_back(d);
//             }

//             if (digits.size() == REQUIRED_DIGITS) {
//                 std::sort(digits.begin(), digits.end(),
//                     [](const DigitBox &a, const DigitBox &b) {
//                         return a.x < b.x;
//                     }
//                 );

//                 char five_digit_frame[REQUIRED_DIGITS + 1];

//                 for (int i = 0; i < REQUIRED_DIGITS; i++) {
//                     five_digit_frame[i] = targetToDigit(digits[i].target);
//                 }

//                 five_digit_frame[REQUIRED_DIGITS] = '\0';

//                 /*
//                    Only update the SPI frame if the number changed.

//                    This prevents the AI task from constantly rewriting the frame
//                    while Nios is trying to read it.
//                 */
//                 if (strcmp(five_digit_frame, previous_good_frame) != 0) {
//                     strcpy(previous_good_frame, five_digit_frame);

//                     updateLatestValidFrameNoReset(five_digit_frame);

//                     printFiveDigitReading(five_digit_frame);

//                     Serial.println("Digit details:");

//                     for (int i = 0; i < digits.size(); i++) {
//                         Serial.print("  digit=");
//                         Serial.print(targetToDigit(digits[i].target));
//                         Serial.print(" target=");
//                         Serial.print(digits[i].target);
//                         Serial.print(" name=");
//                         Serial.print(targetToName(digits[i].target));
//                         Serial.print(" score=");
//                         Serial.print(digits[i].score);
//                         Serial.print(" x=");
//                         Serial.print(digits[i].x);
//                         Serial.print(" y=");
//                         Serial.print(digits[i].y);
//                         Serial.print(" w=");
//                         Serial.print(digits[i].w);
//                         Serial.print(" h=");
//                         Serial.println(digits[i].h);
//                     }

//                     Serial.println();
//                 }
//                 else {
//                     /*
//                        Same reading as before.
//                        Do not rewrite the SPI frame.
//                        SPI task keeps cycling through existing frame.
//                     */
//                     Serial.print("Same valid reading still detected: ");
//                     Serial.println(five_digit_frame);
//                 }
//             }
//             else {
//                 /*
//                    Not exactly 5 digits.
//                    Do not send partial data.
//                    Do not overwrite previous valid frame.
//                 */
//                 Serial.print("Scanning... valid high-confidence digits = ");
//                 Serial.print(digits.size());
//                 Serial.print(" / ");
//                 Serial.print(REQUIRED_DIGITS);
//                 Serial.print(" | threshold = ");
//                 Serial.println(SCORE_THRESHOLD);
//             }
//         }
//         else {
//             /*
//                No detection.
//                Do not overwrite previous valid frame.
//             */
//             Serial.print("Scanning... no boxes detected | threshold = ");
//             Serial.println(SCORE_THRESHOLD);
//         }

//         delay(50);
//     }
// }

// void spiTask(void *parameter)
// {
//     Serial.println("Starting SPI slave task...");

//     slave.setDataMode(SPI_MODE);
//     slave.setQueueSize(1);

//     slave.begin();

//     Serial.println("SPI slave ready.");
//     Serial.println("Sending repeating frame: 5 digits + newline.");
//     Serial.println("Before first valid detection, sending 0xFF idle bytes.");

//     static uint8_t tx_byte = IDLE_SPI_BYTE;
//     static uint8_t rx_byte = 0;

//     while (1) {
//         portENTER_CRITICAL(&frameMux);

//         if (spi_tx_index >= latest_frame_length) {
//             spi_tx_index = 0;
//         }

//         tx_byte = latest_frame[spi_tx_index];
//         spi_tx_index++;

//         portEXIT_CRITICAL(&frameMux);

//         slave.queue(&tx_byte, &rx_byte, 1);

//         const std::vector<size_t> received_bytes = slave.wait(20);

//         delayMicroseconds(200);
//     }
// }

// void setup()
// {
//     Serial.begin(9600);
//     delay(2000);

//     Serial.println();
//     Serial.println("==========================================");
//     Serial.println("Grove AI Water Meter -> ESP32 -> FPGA SPI");
//     Serial.println("Nios-compatible 5-digit frame mode");
//     Serial.println("==========================================");

//     Serial.print("Confidence threshold: ");
//     Serial.println(SCORE_THRESHOLD);

//     Serial.print("Required digits: ");
//     Serial.println(REQUIRED_DIGITS);

//     Serial.println();
//     Serial.println("SPI output format:");
//     Serial.println("Valid frame: 5 ASCII digits followed by newline");
//     Serial.println("Example: 02012\\n");
//     Serial.println("Invalid/no detection: keep previous valid frame");
//     Serial.println("Before first valid frame: 0xFF idle bytes");
//     Serial.println();

//     setIdleFrame();

//     xTaskCreate(
//         aiTask,
//         "AI Task",
//         10000,
//         NULL,
//         1,
//         NULL
//     );

//     xTaskCreate(
//         spiTask,
//         "SPI Task",
//         4096,
//         NULL,
//         2,
//         NULL
//     );
// }

// void loop()
// {
//     delay(1000);
// }

#include <Arduino.h>
#include <Seeed_Arduino_SSCMA.h>
#include <ESP32SPISlave.h>
#include <vector>
#include <algorithm>
#include <cstring>

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
*/

#define SCORE_THRESHOLD 75
#define REQUIRED_DIGITS 5

/*
   Nios expects newline-terminated frames.

   Valid frame:
       1 2 3 4 5 \n

   So SPI_FRAME_LEN = 6.
*/
#define SPI_FRAME_LEN 6
#define IDLE_SPI_BYTE 0xFF

/*
   SPI command from Nios/FPGA.

   When ESP32 receives this byte from SPI master,
   it runs exactly one AI inference.
*/
#define SPI_INFERENCE_REQUEST_BYTE 'R'

char latest_frame[SPI_FRAME_LEN] = {
    IDLE_SPI_BYTE,
    IDLE_SPI_BYTE,
    IDLE_SPI_BYTE,
    IDLE_SPI_BYTE,
    IDLE_SPI_BYTE,
    IDLE_SPI_BYTE
};

volatile int latest_frame_length = SPI_FRAME_LEN;
volatile bool valid_frame_available = false;

portMUX_TYPE frameMux = portMUX_INITIALIZER_UNLOCKED;

int spi_tx_index = 0;

/*
   Semaphore used to trigger AI inference only when requested.
*/
SemaphoreHandle_t inferenceRequestSemaphore;

/*
   Prevent multiple queued inference requests while one is already running.
*/
volatile bool inference_busy = false;
volatile bool inference_request_pending = false;

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

struct DigitBox {
    int target;
    int score;
    int x;
    int y;
    int w;
    int h;
};

void setIdleFrame()
{
    portENTER_CRITICAL(&frameMux);

    for (int i = 0; i < SPI_FRAME_LEN; i++) {
        latest_frame[i] = IDLE_SPI_BYTE;
    }

    latest_frame_length = SPI_FRAME_LEN;
    valid_frame_available = false;
    spi_tx_index = 0;

    portEXIT_CRITICAL(&frameMux);
}

void updateLatestValidFrameNoReset(const char *five_digits)
{
    portENTER_CRITICAL(&frameMux);

    for (int i = 0; i < REQUIRED_DIGITS; i++) {
        latest_frame[i] = five_digits[i];
    }

    latest_frame[REQUIRED_DIGITS] = '\n';

    latest_frame_length = SPI_FRAME_LEN;
    valid_frame_available = true;

    portEXIT_CRITICAL(&frameMux);
}

void printFiveDigitReading(const char *reading)
{
    Serial.println();
    Serial.println("===== EXACTLY 5 DIGITS DETECTED =====");
    Serial.print("Reading: ");
    Serial.println(reading);
    Serial.println("SPI frame: 5 digits + newline");
    Serial.println("=====================================");
    Serial.println();
}

void runSingleInference(char *previous_good_frame)
{
    int ret = AI.invoke();

    if (ret != 0) {
        Serial.print("invoke failed, ret = ");
        Serial.println(ret);
        return;
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
            d.x = AI.boxes()[i].x;
            d.y = AI.boxes()[i].y;
            d.w = AI.boxes()[i].w;
            d.h = AI.boxes()[i].h;

            digits.push_back(d);
        }

        if (digits.size() == REQUIRED_DIGITS) {
            std::sort(digits.begin(), digits.end(),
                [](const DigitBox &a, const DigitBox &b) {
                    return a.x < b.x;
                }
            );

            char five_digit_frame[REQUIRED_DIGITS + 1];

            for (int i = 0; i < REQUIRED_DIGITS; i++) {
                five_digit_frame[i] = targetToDigit(digits[i].target);
            }

            five_digit_frame[REQUIRED_DIGITS] = '\0';

            if (strcmp(five_digit_frame, previous_good_frame) != 0) {
                strcpy(previous_good_frame, five_digit_frame);

                updateLatestValidFrameNoReset(five_digit_frame);

                printFiveDigitReading(five_digit_frame);

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

                Serial.println();
            }
            else {
                Serial.print("Same valid reading detected again: ");
                Serial.println(five_digit_frame);
            }
        }
        else {
            Serial.print("Inference complete, but valid high-confidence digits = ");
            Serial.print(digits.size());
            Serial.print(" / ");
            Serial.print(REQUIRED_DIGITS);
            Serial.print(" | threshold = ");
            Serial.println(SCORE_THRESHOLD);
        }
    }
    else {
        Serial.print("Inference complete, no boxes detected | threshold = ");
        Serial.println(SCORE_THRESHOLD);
    }
}

void aiTask(void *parameter)
{
    Serial.println("Starting Grove AI water meter task...");

    if (!AI.begin()) {
        Serial.println("ERROR: Grove AI begin failed!");
        Serial.println("Check Grove Vision AI connection.");

        while (1) {
            delay(1000);
        }
    }

    Serial.println("Grove AI connected successfully.");
    Serial.println("AI inference mode: continuous while SPI receives 'R'");
    Serial.print("Trigger byte from SPI master = ");
    Serial.println(SPI_INFERENCE_REQUEST_BYTE);
    Serial.print("Confidence threshold = ");
    Serial.println(SCORE_THRESHOLD);
    Serial.print("Required digit count = ");
    Serial.println(REQUIRED_DIGITS);

    char previous_good_frame[REQUIRED_DIGITS + 1] = "";

    while (1) {
        /*
           Wait until Nios sends at least one 'R'.
        */
        xSemaphoreTake(inferenceRequestSemaphore, portMAX_DELAY);

        /*
           Run one inference.
           If Nios keeps sending 'R', spiTask will queue another request
           after this one finishes.
        */
        inference_busy = true;
        inference_request_pending = false;

        Serial.println();
        Serial.println("SPI inference request received.");
        Serial.println("Running one AI inference...");

        runSingleInference(previous_good_frame);

        Serial.println("Inference finished.");
        Serial.println();

        inference_busy = false;

        /*
           If another 'R' arrived while AI was busy,
           immediately run another inference.
        */
        if (inference_request_pending) {
            xSemaphoreGive(inferenceRequestSemaphore);
        }
    }
}

void spiTask(void *parameter)
{
    Serial.println("Starting SPI slave task...");

    slave.setDataMode(SPI_MODE);
    slave.setQueueSize(1);

    slave.begin();

    Serial.println("SPI slave ready.");
    Serial.println("Sending repeating frame: 5 digits + newline.");
    Serial.println("Before first valid detection, sending 0xFF idle bytes.");
    Serial.println("Send byte 'R' from Nios/FPGA to trigger one inference.");

    static uint8_t tx_byte = IDLE_SPI_BYTE;
    static uint8_t rx_byte = 0;

    while (1) {
        portENTER_CRITICAL(&frameMux);

        if (spi_tx_index >= latest_frame_length) {
            spi_tx_index = 0;
        }

        tx_byte = latest_frame[spi_tx_index];
        spi_tx_index++;

        portEXIT_CRITICAL(&frameMux);

        slave.queue(&tx_byte, &rx_byte, 1);

        const std::vector<size_t> received_bytes = slave.wait(20);

        /*
           If Nios/FPGA sends 'R', trigger exactly one inference.
           If inference is already running, ignore repeated requests.
        */
        if (received_bytes.size() > 0) {
                if (rx_byte == SPI_INFERENCE_REQUEST_BYTE) {

                    if (!inference_busy && !inference_request_pending) {
                        inference_request_pending = true;
                        xSemaphoreGive(inferenceRequestSemaphore);
                        Serial.println("Inference request queued from SPI.");
                    }
                    else {
                        /*
                        AI is already running or one request is already waiting.
                        Do not print repeatedly.
                        Do not queue unlimited requests.
                        */
                        inference_request_pending = true;
                    }
                }

                rx_byte = 0;
            }
        }

        delayMicroseconds(200);
    }


void setup()
{
    Serial.begin(9600);
    delay(2000);

    Serial.println();
    Serial.println("==========================================");
    Serial.println("Grove AI Water Meter -> ESP32 -> FPGA SPI");
    Serial.println("Nios-compatible 5-digit frame mode");
    Serial.println("Triggered inference mode");
    Serial.println("==========================================");

    Serial.print("Confidence threshold: ");
    Serial.println(SCORE_THRESHOLD);

    Serial.print("Required digits: ");
    Serial.println(REQUIRED_DIGITS);

    Serial.println();
    Serial.println("SPI output format:");
    Serial.println("Valid frame: 5 ASCII digits followed by newline");
    Serial.println("Example: 02012\\n");
    Serial.println("Invalid/no detection: keep previous valid frame");
    Serial.println("Before first valid frame: 0xFF idle bytes");
    Serial.println();
    Serial.println("Inference control:");
    Serial.println("Nios/FPGA must send ASCII 'R' over SPI to run one inference.");
    Serial.println();

    setIdleFrame();

    inferenceRequestSemaphore = xSemaphoreCreateBinary();

    if (inferenceRequestSemaphore == NULL) {
        Serial.println("ERROR: Failed to create inference semaphore.");
        while (1) {
            delay(1000);
        }
    }

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
    delay(2500);
}