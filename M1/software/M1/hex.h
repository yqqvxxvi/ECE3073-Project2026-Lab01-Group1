#ifndef HEX_H
#define HEX_H

/* Buffer holding 7-seg codes for HEX0–HEX5 */
extern unsigned char hex_buf[6];

/* Convert a character to its 7-segment code */
unsigned char char_to_7seg(char c);

/* Write hex_buf contents to HEX012 and HEX345 output ports */
void hex_refresh(void);

/* Scroll a new character in from the right (HEX0 side) */
void hex_scroll_in_char(char c);
void hex_show_xyz_2digit_signed(int x, int y, int z);
void hex_scroll_string(const char *str);

int two_digit_value_signed(int v);
int two_digit_value(int v);

void hex_show_3_signed_values(int a, int b, int c);


#endif /* HEX_H */
