#ifndef SDRAM_TEST_H
#define SDRAM_TEST_H

#include <stdint.h>

int sdram_test_memory(volatile uint8_t *start, volatile uint8_t *end, uint8_t test_val);
int sdram_run_default_test(void);

#endif