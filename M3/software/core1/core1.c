#include "nios2_ctrl_reg_macros.h"
//#include "altera_up_avalon_accelerometer_spi.h"
//#include "system.h"
//#include "hex.h"
//#include "accelerometer.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <string.h>
#include <system.h>
#include <io.h>
#include <sys/alt_irq.h>
#include "sys/alt_irq.h"
#include <sys/alt_alarm.h>
#include "sys/alt_alarm.h"
#include <sys/alt_cache.h>
#include <sys/alt_stdio.h>

#include "nios2_ctrl_reg_macros.h"
#include "system.h"
#include "altera_avalon_mutex.h"
#include <altera_avalon_jtag_uart_regs.h>

#include <stddef.h>
#include <math.h>
#include <string.h>
#include <system.h>
#include <io.h>
#include "sys/alt_stdio.h"
#include <stdio.h>
#include "stdio.h"

#include <stdlib.h>
#include "stdlib.h"

#include "io.h"
#include <io.h>

#include "system.h"
#include <stdio.h>

#include <stdint.h>
#include "altera_avalon_mutex.h"
#include <altera_avalon_mutex.h>

#include <sys/alt_cache.h>
#include <altera_avalon_jtag_uart_regs.h>

#include <unistd.h>

//#include "includes.h"
/* Or specifically: */
//#include "os_cfg.h"
//#include "ucos_ii.h"

#define SDRAM_1       0x01100000
#define SDRAM_2       0x01200000
#define RED_LED_ADDR  LED_RED_BASE
#define YELLOW_LED_ADDR LED_YELLOW_BASE
#define GREEN_LED_ADDR LED_GREEN_BASE
//BUZZER_BASE

volatile int *speaker      = (int *)BUZZER_BASE;
volatile int *Mutex      = (int *)MUTEX_BASE;
volatile int *Shared_Sdram_Flag      = (int *)SHARED_SDRAM_BASE;
volatile int *Shared_Sdram_1      = (int *)SDRAM_1;
volatile int *Shared_Sdram_2      = (int *)SDRAM_2;
volatile int *InPort_SW      = (int *)SW_BASE;
volatile int *green_LED      = (int *)LED_GREEN_BASE;
volatile int *yellow_LED      = (int *)LED_YELLOW_BASE;
volatile int *red_LED      = (int *)LED_RED_BASE;
volatile int *timer      = (int *)US_COUNTER_BASE;
volatile int *keys      = (int *)KEYS_BASE;
volatile int *LED9_0      = (int *)LEDR_BASE;


/* ---------------- Speaker / Morse tuning ---------------- */
#define SPEAKER_ON_VALUE      1
#define SPEAKER_OFF_VALUE     0

#define LED_ON_VALUE          1
#define LED_OFF_VALUE         0

#define MORSE_UNIT_DELAY      150000
#define MORSE_DOT_TOGGLES     300
#define TONE_HALF_PERIOD      500

/* ---------------- Helper delays ---------------- */
void short_delay(volatile int count)
{
    for (volatile int i = 0; i < count; i++);
}

void tone_delay(volatile int count)
{
    for (volatile int i = 0; i < count; i++);
}

/* ---------------- Safe Nios II/f I/O access macros ---------------- */
#define READ_REG(addr)         IORD(addr, 0)
#define WRITE_REG(addr, val)   IOWR(addr, 0, val)

/* ---------------- Speaker helpers ---------------- */

void speaker_on(void)
{
    WRITE_REG(BUZZER_BASE, SPEAKER_ON_VALUE);
}

void speaker_off(void)
{
    WRITE_REG(BUZZER_BASE, SPEAKER_OFF_VALUE);
}

/* ---------------- LED helpers ---------------- */
void all_morse_leds_off(void)
{
    WRITE_REG(RED_LED_ADDR, LED_OFF_VALUE);
    WRITE_REG(YELLOW_LED_ADDR, LED_OFF_VALUE);
    WRITE_REG(GREEN_LED_ADDR, LED_OFF_VALUE);
}

void red_on_only(void)
{
    WRITE_REG(RED_LED_ADDR, LED_ON_VALUE);
    WRITE_REG(YELLOW_LED_ADDR, LED_OFF_VALUE);
    WRITE_REG(GREEN_LED_ADDR, LED_OFF_VALUE);
}

void yellow_on_only(void)
{
    WRITE_REG(RED_LED_ADDR, LED_OFF_VALUE);
    WRITE_REG(YELLOW_LED_ADDR, LED_ON_VALUE);
    WRITE_REG(GREEN_LED_ADDR, LED_OFF_VALUE);
}

void green_on_only(void)
{
    WRITE_REG(RED_LED_ADDR, LED_OFF_VALUE);
    WRITE_REG(YELLOW_LED_ADDR, LED_OFF_VALUE);
    WRITE_REG(GREEN_LED_ADDR, LED_ON_VALUE);
}

void beep_short(void)
{
    for (int i = 0; i < 15; i++) {
        speaker_on();
        tone_delay(50);
        speaker_off();
        tone_delay(50);
    }

    speaker_off();
}

/* ---------------- Display reading on output LEDs ---------------- */
//void display_reading_on_leds(const char *reading)
//{
//    if (reading[0] == NO_READING_BYTE || reading[0] == '\0') {
//        WRITE_REG(OUTPORT_LED_ADDR, 0x00);
//        return;
//    }
//
//    int len = strlen(reading);
//    char last_char = reading[len - 1];
//
//    if (last_char >= '0' && last_char <= '9') {
//        int last_digit = last_char - '0';
//        WRITE_REG(OUTPORT_LED_ADDR, last_digit & 0x0F);
//    }
//    else {
//        WRITE_REG(OUTPORT_LED_ADDR, 0x00);
//    }
//}
/* ---------------- Morse buzzer functions ---------------- */

/*
 * led_type:
 * 0 = no special LED
 * 1 = red LED for dot
 * 2 = yellow LED for dash
 */
void morse_tone_for_units(int units, int led_type)
{
    int total_toggles = units * MORSE_DOT_TOGGLES;

    if (led_type == 1) {
        red_on_only();
    }
    else if (led_type == 2) {
        yellow_on_only();
    }
    else {
        all_morse_leds_off();
    }

    for (int i = 0; i < total_toggles; i++) {
        speaker_on();
        tone_delay(TONE_HALF_PERIOD);

        speaker_off();
        tone_delay(TONE_HALF_PERIOD);
    }

    speaker_off();
    all_morse_leds_off();
}

void morse_dot(void)
{
    morse_tone_for_units(1, 1);
    short_delay(MORSE_UNIT_DELAY);
}

void morse_dash(void)
{
    morse_tone_for_units(3, 2);
    short_delay(MORSE_UNIT_DELAY);
}

const char *morse_lookup(char c)
{
    switch (c) {
        case 'A': case 'a': return ".-";
        case 'B': case 'b': return "-...";
        case 'C': case 'c': return "-.-.";
        case 'D': case 'd': return "-..";
        case 'E': case 'e': return ".";
        case 'F': case 'f': return "..-.";
        case 'G': case 'g': return "--.";
        case 'H': case 'h': return "....";
        case 'I': case 'i': return "..";
        case 'J': case 'j': return ".---";
        case 'K': case 'k': return "-.-";
        case 'L': case 'l': return ".-..";
        case 'M': case 'm': return "--";
        case 'N': case 'n': return "-.";
        case 'O': case 'o': return "---";
        case 'P': case 'p': return ".--.";
        case 'Q': case 'q': return "--.-";
        case 'R': case 'r': return ".-.";
        case 'S': case 's': return "...";
        case 'T': case 't': return "-";
        case 'U': case 'u': return "..-";
        case 'V': case 'v': return "...-";
        case 'W': case 'w': return ".--";
        case 'X': case 'x': return "-..-";
        case 'Y': case 'y': return "-.--";
        case 'Z': case 'z': return "--..";

        case '0': return "-----";
        case '1': return ".----";
        case '2': return "..---";
        case '3': return "...--";
        case '4': return "....-";
        case '5': return ".....";
        case '6': return "-....";
        case '7': return "--...";
        case '8': return "---..";
        case '9': return "----.";

        default: return "";
    }
}

void play_morse_char(char c)
{
    const char *code = morse_lookup(c);

    for (int i = 0; code[i] != '\0'; i++) {
        if (code[i] == '.') {
            morse_dot();
        }
        else if (code[i] == '-') {
            morse_dash();
        }
    }

    short_delay(MORSE_UNIT_DELAY * 2);
}

void play_morse_string(const char *msg)
{
    printf("Playing Morse for reading: %s\n", msg);
    printf("Morse: ");

    all_morse_leds_off();

    for (int i = 0; msg[i] != '\0'; i++) {
        const char *code = morse_lookup(msg[i]);

        if (msg[i] == ' ') {
            printf(" / ");
            short_delay(MORSE_UNIT_DELAY * 7);
        }
        else {
            printf("%c=%s ", msg[i], code);
            play_morse_char(msg[i]);
        }
    }

    printf("\n");

    speaker_off();

    /*
     * Finished Morse playback = green LED.
     */
    green_on_only();
    short_delay(MORSE_UNIT_DELAY * 8);
    all_morse_leds_off();
}

void handle_key0_morse(int current_5digit_reading)
{
    printf("\nKEY0 PRESSED\n");

//    if (sizeof(current_5digit_reading))!= {
//        printf("No valid 5-digit reading available for Morse playback.\n");
//        printf("-------------------------\n");
//        return;
//    }


    printf("Morse playback selected for locked reading: %d\n", current_5digit_reading);
    printf("Dot = red LED, Dash = yellow LED, Finished = green LED\n");

    WRITE_REG(LEDR_BASE, 0x01);



//    play_morse_string(current_5digit_reading);

    speaker_off();
    WRITE_REG(LEDR_BASE, 0x00);

    /*
     * New behavior:
     * Do NOT resume scanning after KEY0.
     * Wait for KEY1.
     */
//    morse_done_waiting_for_key1 = 1;

    printf("Morse playback complete.\n");
    printf("System is still paused.\n");
    printf("Press KEY1 to store this reading to SDRAM and continue scanning.\n");
    printf("-------------------------\n");
}



int main()
{ 

for (int j = 0; j < 0x40; j++)
  {
	usleep(1000);
    IOWR_16DIRECT(Shared_Sdram_Flag,j,0);
  }

alt_mutex_dev* mutex=altera_avalon_mutex_open("/dev/mutex");

*red_LED = 0x00;
*yellow_LED =0x00;
*green_LED = 0x00;   // OFF initially (active-high)
*speaker   = 0x00;   // OFF initially
*LED9_0=0;

int count=0;
int reading=0x00;
int beep=0x00;
char c=0x00;
int swBase=0;
int keyBaseraw=0;
int keyBase=0;
uint16_t stateBuffer1 = 0;
uint16_t stateBuffer2 = 0;

  while (1){
	  *LED9_0=count;
	  count=count+1;
	  swBase = IORD_8DIRECT(InPort_SW, 0);
	  keyBaseraw = IORD_8DIRECT(keys, 0);
//	  printf("1:%u\n",!(keyBaseraw & 0x01));
//	  usleep(5000);
//	  printf("2:%u\n",!(keyBaseraw & 0x02));
//	  usleep(5000);
	  stateBuffer1 = (stateBuffer1 << 1) | !(keyBaseraw & 0x1);

	  stateBuffer2 = (stateBuffer2 << 1) | !((keyBaseraw & 0x02) >> 1);


	  if (stateBuffer1 == 0xFFFF) {
		  // Button has been stable HIGH for 16 consecutive samples
		  keyBase |= 0x01;
	  } else if (stateBuffer1 == 0x0000) {
		  keyBase &= ~0x01;
		  // Button has been stable LOW for 16 consecutive samples
	  }

	  if (stateBuffer2 == 0xFFFF) {
	  		  // Button has been stable HIGH for 16 consecutive samples
		  keyBase |= 0x02;
	  	  } else if (stateBuffer2 == 0x0000) {
	  	  // Button has been stable LOW for 16 consecutive samples
	  		keyBase &= ~0x02;
	  	  }

	  altera_avalon_mutex_lock(mutex, 1);

	  IOWR_16DIRECT(Shared_Sdram_Flag,0x10,swBase);
	  IOWR_16DIRECT(Shared_Sdram_Flag,0x14,keyBase);

	  *red_LED = IORD_16DIRECT(Shared_Sdram_Flag,0x18);
	  *yellow_LED = IORD_16DIRECT(Shared_Sdram_Flag, 0x1C);
	  *green_LED = IORD_16DIRECT(Shared_Sdram_Flag, 0x20);
	  *speaker = IORD_16DIRECT(Shared_Sdram_Flag, 0x24);
	  beep = IORD_16DIRECT(Shared_Sdram_Flag, 0x28);
	  c = IORD_16DIRECT(Shared_Sdram_Flag, 0x2C);
	  reading = IORD_16DIRECT(Shared_Sdram_Flag, 0x30);

	  altera_avalon_mutex_unlock(mutex);

	  if ((beep&0x01)!=0x00){
		  alt_putstr("1.5!\n");
		  beep_short();
		  beep=0x00;
	  }
	  if ((c&0x01)!=0x00){
		  alt_putstr("1.4!\n");
		  play_morse_char(c);
		  c=0x00;
	  }

	  if ((reading)!=0x00){
		  	  alt_putstr("1.3!\n");
		  	  handle_key0_morse(reading);
		  	  reading=0x00;
	  	  }

	  usleep(5000);

  }

  return 0;
}
