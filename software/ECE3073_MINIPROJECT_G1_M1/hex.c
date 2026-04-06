#include "system.h"
#include "unistd.h"
#include "hex.h"

extern volatile int *HEX0;
extern volatile int *HEX1;
extern volatile int *HEX2;
extern volatile int *HEX3;
extern volatile int *HEX4;
extern volatile int *HEX5;

// Common active-low 7-seg codes for digits 0-9 in the order of a,b,c,d,e,f,g (g is the middle segment)
unsigned char seg_code[10] = {
    0xC0, // 0 (bin = 11000000)
    0xF9, // 1 (bin = 11111001)
    0xA4, // 2 (bin = 10100100)
    0xB0, // 3 (bin = 10110000)
    0x99, // 4 (bin = 10011001)
    0x92, // 5 (bin = 10010010)
    0x82, // 6 (bin = 10000010)
    0xF8, // 7  (bin = 11111000)
    0x80, // 8 (bin = 10000000)
    0x90  // 9 (bin = 10010000)
};

int hex_test_show(void)
{
    // Show 123456 on HEX5 ... HEX0
    *HEX5 = seg_code[1];
    *HEX4 = seg_code[2];
    *HEX3 = seg_code[3];
    *HEX2 = seg_code[4];
    *HEX1 = seg_code[5];
    *HEX0 = seg_code[6];

    while (1);

    return 0;
}

int hex_test_count(void)
{
    // Count from 0 to 999999 on HEX5 ... HEX0
    for (int i = 0; i <= 999999; i++) {
        int num = i;
        *HEX5 = seg_code[num / 100000]; // Get the hundred-thousands digit
        num %= 100000;
        *HEX4 = seg_code[num / 10000]; // Get the ten-thousands digit
        num %= 10000;
        *HEX3 = seg_code[num / 1000]; // Get the thousands digit
        num %= 1000;
        *HEX2 = seg_code[num / 100]; // Get the hundreds digit
        num %= 100;
        *HEX1 = seg_code[num / 10]; // Get the tens digit
        *HEX0 = seg_code[num % 10]; // Get the ones digit

        usleep(10000); // Sleep
    }

    return 0;
}
