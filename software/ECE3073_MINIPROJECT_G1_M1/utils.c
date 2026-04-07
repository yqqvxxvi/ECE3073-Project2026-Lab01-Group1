#include "system.h"
#include "unistd.h"
#include <stdint.h>
#include <string.h>


void delay(int count)
{
    for (int i = 0; i < count; i++) {
        // 50Mhz clock, 20ns per cycle, so 50 million cycles is 1 second
        // do something that takes 20ns, like a volatile read
        // this is to avoid interrupt dies on usleep
        volatile int *dummy = (int *)0x0; // read from an arbitrary address
        
    }
}

