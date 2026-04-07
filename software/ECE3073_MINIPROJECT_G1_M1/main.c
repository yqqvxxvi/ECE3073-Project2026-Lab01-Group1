#include "system.h"
#include "unistd.h"
#include "hex.h"

volatile int *HEX0 = (int *)0x04001050;
volatile int *HEX1 = (int *)0x04001040;
volatile int *HEX2 = (int *)0x04001030;
volatile int *HEX3 = (int *)0x04001020;
volatile int *HEX4 = (int *)0x04001010;
volatile int *HEX5 = (int *)0x04001000;

int main()
{
    // hex_test();
    // hex_test_count();

    while (1)
    {
    	hex_test_alpha_scrolling("HELLO WORLD");

        usleep(100000); // Sleep for a while to see the result
    }
}
