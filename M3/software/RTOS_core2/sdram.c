#include "sdram.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "system.h"


#define NEW_SDRAM_CONTROLLER_0_BASE (SHARED_SDRAM_BASE+0XF0)
/*
 * Assumes your SDRAM base symbol in system.h is:
 * NEW_SDRAM_CONTROLLER_0_BASE
 */

void sdram_write_array(uint32_t offset, const uint8_t *src, uint32_t len)
{
    volatile uint8_t *mem =
        (volatile uint8_t *)(NEW_SDRAM_CONTROLLER_0_BASE + offset);

    uint32_t i;
    for (i = 0; i < len; i++) {
        mem[i] = src[i];
    }

    printf("SDRAM write: %lu bytes at offset 0x%08X\n",
           (unsigned long)len, (unsigned int)offset);
}

void sdram_read_array(uint32_t offset, uint8_t *dst, uint32_t len)
{
    volatile uint8_t *mem =
        (volatile uint8_t *)(NEW_SDRAM_CONTROLLER_0_BASE + offset);

    uint32_t i;
    for (i = 0; i < len; i++) {
        dst[i] = mem[i];
    }

    printf("SDRAM read: %lu bytes from offset 0x%08X\n",
           (unsigned long)len, (unsigned int)offset);
}

void sdram_write_string(uint32_t offset, const char *str)
{
    sdram_write_array(offset, (const uint8_t *)str, strlen(str) + 1);
}

void sdram_read_string(uint32_t offset, char *buf, uint32_t bufsize)
{
    volatile uint8_t *mem =
        (volatile uint8_t *)(NEW_SDRAM_CONTROLLER_0_BASE + offset);

    uint32_t i;
    for (i = 0; i < bufsize - 1; i++)
    {
        buf[i] = (char)mem[i];
        if (buf[i] == '\0') {
            return;
        }
    }

    buf[bufsize - 1] = '\0';
}
