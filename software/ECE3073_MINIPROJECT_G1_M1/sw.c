#include "sw.h"
#include "system.h"
#include "unistd.h"
#include <stdint.h>
#include <string.h>

extern volatile int *SW;

int get_mode(void)
{
    return (*SW) & 0x7;   // SW0-SW2 choose mode 7 mode in total
}

int is_switch_on(int bit)
{
    return (((*SW) & (1 << bit)) != 0);
}