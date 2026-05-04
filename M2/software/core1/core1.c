#include "nios2_ctrl_reg_macros.h"
//#include "altera_up_avalon_accelerometer_spi.h"
//#include "system.h"
//#include "hex.h"
//#include "accelerometer.h"

#include <stddef.h>
#include <math.h>
#include <string.h>
#include <system.h>
#include <io.h>
#include "sys/alt_stdio.h"
#include <stdlib.h>
#include "io.h"
#include "system.h"
#include <stdio.h>
#include <stdint.h>
#include "altera_avalon_mutex.h"
#include <sys/alt_cache.h>

#define BUZZER_BASE 0x2021030
#define SDRAM_MUTEX_BASE 0x2021080
#define SHARED_SDRAM_BASE 0x1000000
#define SDRAM_1       0x01100000
#define SDRAM_2       0x01200000
#define SWITCH_BASE 0x2021060
#define GREEN_LIGHT_BASE 0x2021020
#define RED_LIGHT_BASE 0x2021000
#define US_COUNTER_BASE 0x2021070
#define YELLOW_LIGHT_BASE 0x2021010

//BUZZER_BASE

volatile int *speaker      = (int *)BUZZER_BASE;
volatile int *Mutex      = (int *)SDRAM_MUTEX_BASE;
volatile int *Shared_Sdram_Flag      = (int *)SHARED_SDRAM_BASE;
volatile int *Shared_Sdram_1      = (int *)SDRAM_1;
volatile int *Shared_Sdram_2      = (int *)SDRAM_2;
volatile int *InPort_SW      = (int *)SWITCH_BASE;
volatile int *green_LED      = (int *)GREEN_LIGHT_BASE;
volatile int *yellow_LED      = (int *)YELLOW_LIGHT_BASE;
volatile int *red_LED      = (int *)RED_LIGHT_BASE;
volatile int *timer      = (int *)US_COUNTER_BASE;

/* ---------------- Morse buzzer tuning ---------------- */

/*
 * If your speaker turns ON when writing 1, keep this.
 *
 * If your speaker logic is inverted, change this:
 *
 * #define SPEAKER_ON_VALUE   0
 * #define SPEAKER_OFF_VALUE  1
 */
#define SPEAKER_ON_VALUE   1
#define SPEAKER_OFF_VALUE  0

/*
 * Controls silent gap length between dots/dashes/letters.
 *
 * Increase = slower Morse gaps.
 * Decrease = faster Morse gaps.
 */
#define MORSE_UNIT_DELAY   60000

/*
 * Controls beep duration.
 *
 * Dot  = 1 x MORSE_DOT_TOGGLES
 * Dash = 3 x MORSE_DOT_TOGGLES
 *
 * If H is still too long, reduce this to 60.
 */
#define MORSE_DOT_TOGGLES  60

/*
 * Controls pitch of buzzer tone.
 *
 * Smaller = higher pitch.
 * Larger = lower pitch.
 */
#define TONE_HALF_PERIOD   120

/* ---------------- Helper delays ---------------- */
void short_delay(volatile int count)
{
    for (volatile int i = 0; i < count; i++);
}

void tone_delay(volatile int count)
{
    for (volatile int i = 0; i < count; i++);
}

/* ---------------- Speaker helpers ---------------- */

void speaker_on(void)
{
    *speaker = SPEAKER_ON_VALUE;
}

void speaker_off(void)
{
    *speaker = SPEAKER_OFF_VALUE;
}

void beep_short(void)
{
    for (int i = 0; i < 30; i++) {
        speaker_on();
        tone_delay(60);
        speaker_off();
        tone_delay(60);
    }

    speaker_off();
}

/* ---------------- Morse buzzer functions ---------------- */

/*
 * This creates the actual buzzing sound.
 *
 * Old problem:
 * The previous code used MORSE_UNIT_TIME = 60000 as the number of tone cycles.
 * That made one dot extremely long.
 *
 * New version:
 * A dot only uses MORSE_DOT_TOGGLES cycles.
 */
void morse_tone_for_units(int units)
{
    int total_toggles = units * MORSE_DOT_TOGGLES;

    for (int i = 0; i < total_toggles; i++) {
        speaker_on();
        tone_delay(TONE_HALF_PERIOD);

        speaker_off();
        tone_delay(TONE_HALF_PERIOD);
    }

    speaker_off();
}

/*
 * Morse timing:
 *
 * dot  = 1 unit
 * dash = 3 units
 *
 * gap between dots/dashes inside a letter = 1 unit
 * gap between letters = 3 units
 * gap between words = 7 units
 */

void morse_dot(void)
{
    morse_tone_for_units(1);

    /*
     * 1 unit gap after each dot/dash.
     */
    short_delay(MORSE_UNIT_DELAY);
}

void morse_dash(void)
{
    morse_tone_for_units(3);

    /*
     * 1 unit gap after each dot/dash.
     */
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
        } else if (code[i] == '-') {
            morse_dash();
        }
    }

    /*
     * We already waited 1 unit after the final dot/dash.
     * Letter gap should be 3 units total.
     * So add 2 more units.
     */
    short_delay(MORSE_UNIT_DELAY * 2);
}

void play_morse_string(const char *msg)
{
    printf("Playing Morse: ");

    for (int i = 0; msg[i] != '\0'; i++) {
        const char *code = morse_lookup(msg[i]);

        if (msg[i] == ' ') {
            printf(" / ");

            /*
             * Word gap should be 7 units.
             */
            short_delay(MORSE_UNIT_DELAY * 7);
        } else {
            printf("%c=%s ", msg[i], code);
            play_morse_char(msg[i]);
        }
    }

    printf("\n");
    speaker_off();
}






int main()
{ 
for (int j = 0; j < 0x10; j++)
  {
      IOWR_16DIRECT(Shared_Sdram_Flag,j,0);
  }

uint32_t start_time, end_time;
start_time = IORD_32DIRECT(timer, 0);
alt_putstr("Hello from core 1!\n");
end_time = IORD_32DIRECT(timer, 0);
printf("Time(us): %u\n\n",(unsigned int)(end_time-start_time));

alt_mutex_dev* mutex=altera_avalon_mutex_open("/dev/SHARED_SDRAM");
int LENGTH=IORD_32DIRECT(InPort_SW, 0) & 0x7F; //SW[6:0]

altera_avalon_mutex_lock(mutex, 0);

for (int i = 0; i < LENGTH; i++)
{
	for (int j = 0; j < LENGTH; j++)
	{
	//int hold=rand()-(32767/2);
		int hold=2;
		IOWR_16DIRECT(Shared_Sdram_1,(
		((i*LENGTH)+j)
		*sizeof(int)),hold);

	}
}

IOWR_16DIRECT(Shared_Sdram_Flag,0x10,LENGTH);
alt_dcache_flush(Shared_Sdram_Flag,((LENGTH*LENGTH)));

altera_avalon_mutex_unlock(mutex);


while((IORD_16DIRECT(Shared_Sdram_Flag,6)&0x01)==0);
altera_avalon_mutex_lock(mutex, 0);


start_time = IORD_32DIRECT(timer, 0);
for (int i = 0; i < LENGTH/2; i++)
{
for (int j = 0; j < LENGTH; j++)
 {
for (int k = 0; k < LENGTH; k++){
    int num1=IORD_16DIRECT(Shared_Sdram_1,(
            ((i*LENGTH)+k)
            *sizeof(int)));
    int num2=IORD_16DIRECT(Shared_Sdram_2,(
            ((k*LENGTH)+j)
            *sizeof(int)));
    int num3=IORD_16DIRECT(Shared_Sdram_2,(
            ((LENGTH*LENGTH)+(i*LENGTH)+j)
            *sizeof(int)));
    int hold3 =num3+(num1*num2);
    IOWR_16DIRECT(Shared_Sdram_2,(
          ((LENGTH*LENGTH)+(i*LENGTH)+j)
          *sizeof(int)),hold3);
         }
     }
}
            end_time = IORD_32DIRECT(timer, 0);

            IOWR_16DIRECT(Shared_Sdram_Flag,8,1);
            altera_avalon_mutex_unlock(mutex);

            printf("Time(us): %u\n\n",(unsigned int)(end_time-start_time));

*red_LED = 0x00;
*green_LED = 0x00;   // OFF initially (active-high)
*speaker   = 0x00;   // OFF initially


void short_delay(volatile int count)
{
    for (volatile int i = 0; i < count; i++);
}

void beep_short(void)
{
    for (int i = 0; i < 30; i++) {
        *speaker = 0x01;
        short_delay(60);
        *speaker = 0x00;
        short_delay(60);
    }
}

  while (1){

	  int swBase = IORD_32DIRECT(InPort_SW, 0);
	  altera_avalon_mutex_lock(mutex, 0);
	  printf("%u",swBase);
	  IOWR_16DIRECT(Shared_Sdram_Flag,0x10,swBase);
//	  IOWR_16DIRECT(Shared_Sdram_Flag,0x20,keyBase);
	  altera_avalon_mutex_unlock(mutex);

  }

  return 0;
}


