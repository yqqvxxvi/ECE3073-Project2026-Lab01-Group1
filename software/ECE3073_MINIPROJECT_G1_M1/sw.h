#ifndef SW_H
#define SW_H

extern volatile int *SW;

int get_mode(void);
int is_switch_on(int bit);

#endif
