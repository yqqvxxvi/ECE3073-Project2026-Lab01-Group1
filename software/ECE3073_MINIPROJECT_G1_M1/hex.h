#ifndef HEX_H
#define HEX_H

extern volatile int *HEX0;
extern volatile int *HEX1;
extern volatile int *HEX2;
extern volatile int *HEX3;
extern volatile int *HEX4;
extern volatile int *HEX5;

int hex_test_show(void);
int hex_test_count(void);
int hex_test_alpha_scrolling(char *str);
int decode_alpha_to_7seg(char c);
void hex_write_all(int h5, int h4, int h3, int h2, int h1, int h0);

#endif
