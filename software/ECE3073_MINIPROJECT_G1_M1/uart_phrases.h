#ifndef UART_PHRASES_H
#define UART_PHRASES_H

#include <stdint.h>

void user_phrases_mode(void);
void user_phrases_reset(void);
void clear_phrases(void);
void read_line(char *buf, int max_len);
void handle_phrase_buttons(void);

#endif // UART_PHRASES_H