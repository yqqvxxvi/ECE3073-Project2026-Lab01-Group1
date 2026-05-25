#include <string.h>
#include <unistd.h>
#include "hex.h"

/* Your packed HEX ports */
extern volatile int *OutPort_HEX012;   // HEX0, HEX1, HEX2
extern volatile int *OutPort_HEX345;   // HEX3, HEX4, HEX5

/* 7-seg codes
   Adjust these if your board is active-low instead of active-high.
   These values assume:
   bit0=a, bit1=b, bit2=c, bit3=d, bit4=e, bit5=f, bit6=g
*/
#define SEG_BLANK 0x00
#define SEG_DASH  0x40

unsigned char char_to_7seg(char c)
{
    switch (c)
    {
        /* digits */
        case '0': return 0xC0;
        case '1': return 0xF9;
        case '2': return 0xA4;
        case '3': return 0xB0;
        case '4': return 0x99;
        case '5': return 0x92;
        case '6': return 0x82;
        case '7': return 0xF8;
        case '8': return 0x80;
        case '9': return 0x90;

        /* uppercase / lowercase letters */
        case 'A': case 'a': return 0x88;
        case 'B': case 'b': return 0x83;
        case 'C': case 'c': return 0xC6;
        case 'D': case 'd': return 0xA1;
        case 'E': case 'e': return 0x86;
        case 'F': case 'f': return 0x8E;
        case 'G': case 'g': return 0x90;   /* looks similar to 9 */
        case 'H': case 'h': return 0x89;
        case 'I': case 'i': return 0xF9;   /* like 1 */
        case 'J': case 'j': return 0xE1;
        case 'K': case 'k': return 0x89;   /* approximate as H */
        case 'L': case 'l': return 0xC7;
        case 'M': case 'm': return 0xAA;   /* rough approximation */
        case 'N': case 'n': return 0xAB;   /* rough approximation */
        case 'O': case 'o': return 0xA3;
        case 'P': case 'p': return 0x8C;
        case 'Q': case 'q': return 0x98;
        case 'R': case 'r': return 0xAF;
        case 'S': case 's': return 0x92;
        case 'T': case 't': return 0x87;
        case 'U': case 'u': return 0xC1;
        case 'V': case 'v': return 0xE3;   /* rough approximation */
        case 'W': case 'w': return 0xD5;   /* rough approximation */
        case 'X': case 'x': return 0x89;   /* approximate as H */
        case 'Y': case 'y': return 0x91;
        case 'Z': case 'z': return 0xA4;   /* like 2 */

        /* symbols */
        case '-': return 0xBF;
        case '_': return 0xF7;
        case ' ': return 0xFF;

        default:  return 0xFF;
    }
}


void hex_refresh(void)
{
	*OutPort_HEX345 =
	    ((int)hex_buf[2] << 16) |
	    ((int)hex_buf[1] << 8)  |
	    ((int)hex_buf[0]);

	*OutPort_HEX012 =
	    ((int)hex_buf[5] << 16) |
	    ((int)hex_buf[4] << 8)  |
	    ((int)hex_buf[3]);;
}

void hex_scroll_in_char(char c)
{
    int i;
    unsigned char seg = char_to_7seg(c);

    for (i = 5; i > 0; i--)
    {
        hex_buf[i] = hex_buf[i - 1];
    }

    hex_buf[0] = seg;
    hex_refresh();
}

void hex_scroll_string(const char *str)
{
    int i;

    for (i = 0; str[i] != '\0'; i++)
    {
        hex_scroll_in_char(str[i]);
        usleep(100000);
    }

    // push remaining characters out of the 6 HEX displays
    for (i = 0; i < 6; i++)
    {
        hex_scroll_in_char(' ');
        usleep(100000);
    }
}

unsigned char seg_dp_on(unsigned char seg)
{
    return seg & 0x7F;     // clear bit7
}

unsigned char seg_dp_off(unsigned char seg)
{
    return seg | 0x80;     // set bit7
}

int two_digit_value(int v)
{
    if (v < 0)
        v = -v;

    return v % 100;
}

void hex_show_xyz_2digit_signed(int x, int y, int z)
{
    int x_neg = (x < 0);
    int y_neg = (y < 0);
    int z_neg = (z < 0);

    x = two_digit_value(x);
    y = two_digit_value(y);
    z = two_digit_value(z);

    hex_buf[5] = char_to_7seg('0' + (x / 10));
    hex_buf[4] = char_to_7seg('0' + (x % 10));

    hex_buf[3] = char_to_7seg('0' + (y / 10));
    hex_buf[2] = char_to_7seg('0' + (y % 10));

    hex_buf[1] = char_to_7seg('0' + (z / 10));
    hex_buf[0] = char_to_7seg('0' + (z % 10));

    if (x_neg) hex_buf[5] = seg_dp_on(hex_buf[5]);
    else       hex_buf[5] = seg_dp_off(hex_buf[5]);

    if (y_neg) hex_buf[3] = seg_dp_on(hex_buf[3]);
    else       hex_buf[3] = seg_dp_off(hex_buf[3]);

    if (z_neg) hex_buf[1] = seg_dp_on(hex_buf[1]);
    else       hex_buf[1] = seg_dp_off(hex_buf[1]);

    hex_refresh();
}

int two_digit_value_signed(int v)
{
    if (v < 0)
        v = -v;

    return v % 100;
}

void hex_show_3_signed_values(int a, int b, int c)
{
    int a_neg = (a < 0);
    int b_neg = (b < 0);
    int c_neg = (c < 0);

    a = two_digit_value_signed(a);
    b = two_digit_value_signed(b);
    c = two_digit_value_signed(c);

    hex_buf[5] = char_to_7seg('0' + (a / 10));
    hex_buf[4] = char_to_7seg('0' + (a % 10));

    hex_buf[3] = char_to_7seg('0' + (b / 10));
    hex_buf[2] = char_to_7seg('0' + (b % 10));

    hex_buf[1] = char_to_7seg('0' + (c / 10));
    hex_buf[0] = char_to_7seg('0' + (c % 10));

    if (a_neg) hex_buf[5] = seg_dp_on(hex_buf[5]);
    else       hex_buf[5] = seg_dp_off(hex_buf[5]);

    if (b_neg) hex_buf[3] = seg_dp_on(hex_buf[3]);
    else       hex_buf[3] = seg_dp_off(hex_buf[3]);

    if (c_neg) hex_buf[1] = seg_dp_on(hex_buf[1]);
    else       hex_buf[1] = seg_dp_off(hex_buf[1]);

    hex_refresh();
}
