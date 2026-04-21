#ifndef SDRAM_H
#define SDRAM_H

#include <stdint.h>

#define SDRAM_TEST_SIZE_BYTES 4096
#define SDRAM_TEST_OFFSET     0x00100000

#define SDRAM_PATTERN_A 0xAA
#define SDRAM_PATTERN_B 0x55

int sdram_test_memory(volatile uint8_t *start, volatile uint8_t *end, uint8_t test_val);
int sdram_run_test(void);
void sdram_quick_probe(void);

#endif