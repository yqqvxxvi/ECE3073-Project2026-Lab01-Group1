#ifndef SDRAM_H
#define SDRAM_H

#include <stdint.h>

/* ---- test config ---- */
#define SDRAM_TEST_OFFSET     0x0000000
#define SDRAM_TEST_SIZE_BYTES 1024
#define SDRAM_PATTERN_A       0xAA
#define SDRAM_PATTERN_B       0x55

/* ---- existing API ---- */
int  sdram_run_test(void);
void sdram_quick_probe(void);
int  sdram_test_memory(volatile uint8_t *start, volatile uint8_t *end, uint8_t test_val);

/* ---- array store/load API ---- */

/**
 * sdram_write_array
 * Writes 'len' bytes from 'src' into SDRAM starting at 'offset'.
 *
 * @param offset  Byte offset from SDRAM base
 * @param src     Source buffer
 * @param len     Number of bytes to write
 */
void sdram_write_array(uint32_t offset, const uint8_t *src, uint32_t len);

/**
 * sdram_read_array
 * Reads 'len' bytes from SDRAM starting at 'offset' into 'dst'.
 *
 * @param offset  Byte offset from SDRAM base
 * @param dst     Destination buffer (must be at least 'len' bytes)
 * @param len     Number of bytes to read
 */
void sdram_read_array(uint32_t offset, uint8_t *dst, uint32_t len);

/**
 * sdram_write_string
 * Writes a null-terminated string into SDRAM at 'offset'.
 * The null terminator is also written.
 *
 * @param offset  Byte offset from SDRAM base
 * @param str     Source string
 */
void sdram_write_string(uint32_t offset, const char *str);

/**
 * sdram_read_string
 * Reads a null-terminated string from SDRAM at 'offset' into 'buf'.
 *
 * @param offset   Byte offset from SDRAM base
 * @param buf      Destination buffer
 * @param bufsize  Max bytes to read including null terminator
 */
void sdram_read_string(uint32_t offset, char *buf, uint32_t bufsize);

#endif /* SDRAM_H */
