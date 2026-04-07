#include "system.h"
#include "unistd.h"
#include <stdint.h>
#include <string.h>
#include "hex.h"
#include "utils.h"

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

// Common readable letters for 7-seg display
uint8_t alpha_lut[26] = {
    0x88, // A
    0x83, // B  (looks like b)
    0xC6, // C
    0xA1, // D  (looks like d)
    0x86, // E
    0x8E, // F
    0xC2, // G
    0x89, // H
    0xCF, // I
    0xE1, // J
    0x8A, // K  (approximation)
    0xC7, // L
    0xAA, // M  (approximation)
    0xAB, // N  (approximation)
    0xC0, // O
    0x8C, // P
    0x98, // Q  (approximation)
    0xAF, // R  (approximation)
    0x92, // S
    0x87, // T
    0xC1, // U
    0xE3, // V  (approximation)
    0x81, // W  (approximation)
    0x89, // X  (same-ish as H)
    0x91, // Y
    0xA4  // Z  (same-ish as 2)
};

int decode_alpha_to_7seg(char c)
{
    if (c >= 'A' && c <= 'Z') {
        return alpha_lut[c - 'A'];
    } else if (c >= 'a' && c <= 'z') {
        return alpha_lut[c - 'a'];
    } else {
        return 0xFF; // Blank for unsupported characters
    }
}

void hex_write_all(int h5, int h4, int h3, int h2, int h1, int h0)
{
    *HEX5 = h5;
    *HEX4 = h4;
    *HEX3 = h3;
    *HEX2 = h2;
    *HEX1 = h1;
    *HEX0 = h0;
}

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

        delay(50000); // Adjust delay for visible counting speed
    }

    return 0;
}

// ALPHABET

int hex_test_alpha_scrolling(char *str)
{
    int len = strlen(str);

    uint8_t d5 = 0xFF, d4 = 0xFF, d3 = 0xFF, d2 = 0xFF, d1 = 0xFF, d0 = 0xFF;

    for (int i = 0; i < len + 6; i++) {
        char c;

        if (i < len)
            c = str[i];
        else
            c = ' ';   // push blanks after string ends

        uint8_t new_char = decode_alpha_to_7seg(c);

        // shift to the left so new characters enter from the right
        d5 = d4;
        d4 = d3;
        d3 = d2;
        d2 = d1;
        d1 = d0;
        d0 = new_char;

        hex_write_all(d5, d4, d3, d2, d1, d0);

        delay(100000);   // 0.2s, easier to see
    }

    return 0;
}