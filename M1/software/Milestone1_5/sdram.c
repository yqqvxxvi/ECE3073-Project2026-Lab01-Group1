#include <stdio.h>
#include <stdint.h>

#include "../Dram_2_bsp/system.h"
#include "../Dram_2_bsp/HAL/inc/sys/alt_stdio.h"
#include "sdram.h"

int sdram_test_memory(volatile uint8_t *start, volatile uint8_t *end, uint8_t test_val)
{
    volatile uint8_t *iter;
    int errors = 0;
    int bytes_accessed = 0;

    /* Write phase */
    for (iter = start; iter <= end; iter++)
    {
        *iter = test_val;
        bytes_accessed++;
    }

    /* Read / verify phase */
    for (iter = start; iter <= end; iter++)
    {
        uint8_t read_val = *iter;
        bytes_accessed++;

        if (read_val != test_val)
        {
            printf("ERROR at address %p: wrote 0x%02X, read 0x%02X\n",
                   (void *)iter, test_val, read_val);
            errors++;
        }
    }

    printf("Bytes accessed: %d (wrote %d + read %d)\n",
           bytes_accessed,
           (int)(end - start + 1),
           (int)(end - start + 1));

    return errors;
}

int sdram_run_test(void)
{
    volatile uint8_t *mem_start =
        (volatile uint8_t *)(NEW_SDRAM_CONTROLLER_0_BASE + SDRAM_TEST_OFFSET);

    volatile uint8_t *mem_end =
        (volatile uint8_t *)(NEW_SDRAM_CONTROLLER_0_BASE +
                             SDRAM_TEST_OFFSET +
                             SDRAM_TEST_SIZE_BYTES - 1);

    int total_errors = 0;

    printf("SDRAM Memory Tester\n");
    printf("Testing range: 0x%08X to 0x%08X\n",
           (unsigned int)mem_start, (unsigned int)mem_end);

    printf("Pass 1: Writing pattern 0x%02X...\n", SDRAM_PATTERN_A);
    total_errors += sdram_test_memory(mem_start, mem_end, SDRAM_PATTERN_A);

    printf("Pass 2: Writing pattern 0x%02X...\n", SDRAM_PATTERN_B);
    total_errors += sdram_test_memory(mem_start, mem_end, SDRAM_PATTERN_B);

    if (total_errors == 0)
    {
        printf("MEMORY TEST PASSED: all locations verified.\n");
        return 0;
    }
    else
    {
        printf("MEMORY TEST FAILED: %d errors detected.\n", total_errors);
        return total_errors;
    }
}

void sdram_quick_probe(void)
{
    volatile uint8_t *mem =
        (volatile uint8_t *)(NEW_SDRAM_CONTROLLER_0_BASE + SDRAM_TEST_OFFSET);

    alt_putstr("Start\n");
    alt_putstr("About to write SDRAM\n");

    mem[0] = 0xAA;

    alt_putstr("First write done\n");

    if (mem[0] == 0xAA)
        alt_putstr("Readback pass\n");
    else
        alt_putstr("Readback fail\n");
}
