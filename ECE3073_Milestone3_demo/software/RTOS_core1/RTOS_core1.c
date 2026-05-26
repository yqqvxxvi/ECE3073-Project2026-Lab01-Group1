#ifndef MAIN_HEADERS_H
#define MAIN_HEADERS_H

/* Standard C Libraries */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <unistd.h>

/* Intel/Altera Nios II HAL Framework */
#include "system.h"
#include <io.h>
#include <sys/alt_stdio.h>
#include <sys/alt_cache.h>
#include <sys/alt_irq.h>
#include <sys/alt_alarm.h>

/* Hardware Peripheral Drivers */
#include <altera_avalon_mutex.h>
#include <altera_avalon_jtag_uart_regs.h>
#include "nios2_ctrl_reg_macros.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_timer.h"
#include <io.h>
#include <altera_avalon_timer_regs.h>

/* MicroC/OS-II Real-Time Operating System Core Headers */
#include "includes.h"
#include "os_cfg.h"
#include "ucos_ii.h"

#endif /* MAIN_HEADERS_H */



#define SDRAM_1       0x01100000
#define SDRAM_2       0x01200000

//BUZZER_BASE

//volatile int *speaker      = (int *)BUZZER_BASE;
//volatile int *Mutex      = (int *)MUTEX_BASE;
//volatile int *Shared_Sdram_Flag      = (int *)SHARED_SDRAM_BASE;
//volatile int *Shared_Sdram_1      = (int *)SDRAM_1;
//volatile int *Shared_Sdram_2      = (int *)SDRAM_2;
//volatile int *InPort_SW      = (int *)SW_BASE;
//volatile int *green_LED      = (int *)LED_GREEN_BASE;
//volatile int *yellow_LED      = (int *)LED_YELLOW_BASE;
//volatile int *red_LED      = (int *)LED_RED_BASE;
//volatile int *timer      = (int *)US_COUNTER_BASE;
//volatile int *keys      = (int *)KEYS_BASE;
//volatile int *LED9_0      = (int *)LEDR_BASE;

volatile int *speaker             = (volatile int *) (BUZZER_BASE | 0x80000000);
volatile int *Mutex               = (volatile int *) (MUTEX_BASE | 0x80000000);
volatile int *Shared_Sdram_Flag   = (volatile int *) (SHARED_SDRAM_BASE | 0x80000000);
volatile int *Shared_Sdram_1       = (volatile int *) (SDRAM_1 | 0x80000000);
volatile int *Shared_Sdram_2       = (volatile int *) (SDRAM_2 | 0x80000000);
volatile int *InPort_SW           = (volatile int *) (SW_BASE | 0x80000000);
volatile int *green_LED           = (volatile int *) (LED_GREEN_BASE | 0x80000000);
volatile int *yellow_LED          = (volatile int *) (LED_YELLOW_BASE | 0x80000000);
volatile int *red_LED             = (volatile int *) (LED_RED_BASE | 0x80000000);
volatile int *timer               = (volatile int *) (US_COUNTER_BASE | 0x80000000);
volatile int *keys                = (volatile int *) (KEYS_BASE | 0x80000000);
volatile int* LED9_0 = (volatile int*) (LEDR_BASE | 0x80000000);




/* ---------------- Speaker / Morse tuning ---------------- */
#define SPEAKER_ON_VALUE      1
#define SPEAKER_OFF_VALUE     0

#define LED_ON_VALUE          1
#define LED_OFF_VALUE         0

#define MORSE_UNIT_DELAY      15000
#define MORSE_DOT_TOGGLES     75
#define TONE_HALF_PERIOD      500

/* ---------------- Helper delays ---------------- */
void delay(volatile int count)
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
    WRITE_REG(LED_RED_BASE, LED_OFF_VALUE);
    WRITE_REG(LED_YELLOW_BASE, LED_OFF_VALUE);
    WRITE_REG(LED_GREEN_BASE, LED_OFF_VALUE);
}

void red_on_only(void)
{
    WRITE_REG(LED_RED_BASE, LED_ON_VALUE);
    WRITE_REG(LED_YELLOW_BASE, LED_OFF_VALUE);
    WRITE_REG(LED_GREEN_BASE, LED_OFF_VALUE);
}

void yellow_on_only(void)
{
    WRITE_REG(LED_RED_BASE, LED_OFF_VALUE);
    WRITE_REG(LED_YELLOW_BASE, LED_ON_VALUE);
    WRITE_REG(LED_GREEN_BASE, LED_OFF_VALUE);
}

void green_on_only(void)
{
    WRITE_REG(LED_RED_BASE, LED_OFF_VALUE);
    WRITE_REG(LED_YELLOW_BASE, LED_OFF_VALUE);
    WRITE_REG(LED_GREEN_BASE, LED_ON_VALUE);
}

void beep_short(void)
{
    for (int i = 0; i < 15; i++) {
        speaker_on();
        delay(50);
        speaker_off();
        delay(50);
    }

    speaker_off();
}

/* ---------------- Display reading on output LEDs ---------------- */
void display_reading_on_leds(const char *reading)
{
    if (reading[0] == '\0') {
        WRITE_REG(LED9_0, 0x00);
        return;
    }

    int len = strlen(reading);
    char last_char = reading[len - 1];

    if (last_char >= '0' && last_char <= '9') {
        int last_digit = last_char - '0';
        WRITE_REG(LED9_0, last_digit & 0x0F);
    }
    else {
        WRITE_REG(LED9_0, 0x00);
    }
}
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
        delay(TONE_HALF_PERIOD);

        speaker_off();
        delay(TONE_HALF_PERIOD);
    }

    speaker_off();
    all_morse_leds_off();
}

void morse_dot(void)
{
    morse_tone_for_units(1, 1);
    delay(MORSE_UNIT_DELAY);
}

void morse_dash(void)
{
    morse_tone_for_units(2, 2);
    delay(MORSE_UNIT_DELAY);
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

    delay(MORSE_UNIT_DELAY * 2);
}

void play_morse_string(const char *msg)
{
//    printf("Playing Morse for reading: %s\n", msg);
//    printf("Morse: ");

    all_morse_leds_off();

    for (int i = 0; msg[i] != '\0'; i++) {
        char code = msg[i];
        play_morse_char(code);
//        morse_tone_for_units(1, 1);
    }

    speaker_off();

    /*
     * Finished Morse playback = green LED.
     */
    green_on_only();
    delay(MORSE_UNIT_DELAY * 8);
    all_morse_leds_off();
}

void handle_key0_morse(const char* current_5digit_reading)
{
    printf("\nKEY0 PRESSED\n");

//    if (sizeof(current_5digit_reading)<=5) {
//        printf("No valid 5-digit reading available for Morse playback.\n");
//        printf("-------------------------\n");
//        return;
//    }


//    printf("Morse playback selected for locked reading: %d\n", current_5digit_reading);
    printf("Dot = red LED, Dash = yellow LED, Finished = green LED\n");

//    WRITE_REG(LEDR_BASE, 0x01);



    play_morse_string(current_5digit_reading);

    speaker_off();

//    WRITE_REG(LEDR_BASE, 0x00);

    /*
     * New behavior:
     * Do NOT resume scanning after KEY0.
     * Wait for KEY1.
     */
//    morse_done_waiting_for_key1 = 1;

//    printf("Morse playback complete.\n");
//    printf("System is still paused.\n");
//    printf("Press KEY1 to store this reading to SDRAM and continue scanning.\n");
//    printf("-------------------------\n");
}

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       1024

// Force 8-byte alignment using GCC attributes
__attribute__((aligned(8))) OS_STK task1_stk[TASK_STACKSIZE];
__attribute__((aligned(8))) OS_STK task2_stk[TASK_STACKSIZE];

OS_EVENT *Semaphore;

/* Definition of Task Priorities */

#define TASK1_PRIORITY      2
#define TASK2_PRIORITY      1


volatile int count=0;
volatile char* reading;
volatile int received_value=0;
volatile int beep=0x00;
volatile char c=0x00;
volatile int swBase=0;
volatile int keyBaseraw=0;
volatile int keyBase=0;
volatile int speakerOff = 0;
uint16_t stateBuffer1 = 0;
uint16_t stateBuffer2 = 0;

/* Prints "Hello World" and sleeps for three seconds */
void task1(void* pdata)
{
  usleep(5000);
  INT8U err;
  alt_mutex_dev* mutex=altera_avalon_mutex_open("/dev/mutex");

  while (1)
  {
	  OSSemPend(Semaphore, 0, &err);
	  IOWR(LED9_0,0,count);

	  count=count+1;
	  swBase = IORD_8DIRECT(InPort_SW, 0);

	  uint8_t keyBaseraw = (uint8_t)(*keys & 0x03);

	  // 2. Shift and sample button state (Invert if keys are active-low, standard on DE-boards)
//	   A pressed button adds a '1' bit to the right side of the history buffer
	  stateBuffer1 = ((stateBuffer1 << 1) | ((keyBaseraw & 0x01) ? 0 : 1)) & 0x0FFF;
	  stateBuffer2 = ((stateBuffer2 << 1) | ((keyBaseraw & 0x02) ? 0 : 1)) & 0x0FFF;

	  if (stateBuffer1 == 0x0FF) {
	      keyBase |= 0x01;
	  } else if (stateBuffer1 == 0x0000) {
	      keyBase &= ~0x01;
	  }

	  if (stateBuffer2 == 0x0FF) {
	      keyBase |= 0x02;
	  } else if (stateBuffer2 == 0x0000) {
	      keyBase &= ~0x02;
	  }

	  altera_avalon_mutex_lock(mutex, 1);

	  IOWR_16DIRECT(Shared_Sdram_Flag,0x10,swBase);
	  IOWR_16DIRECT(Shared_Sdram_Flag,0x14,keyBase);

	  *red_LED = IORD_16DIRECT(Shared_Sdram_Flag,0x18);
	  *yellow_LED = IORD_16DIRECT(Shared_Sdram_Flag, 0x1C);
	  *green_LED = IORD_16DIRECT(Shared_Sdram_Flag, 0x20);

	  speakerOff = IORD_16DIRECT(Shared_Sdram_Flag, 0x24);

	  beep = IORD_16DIRECT(Shared_Sdram_Flag, 0x28);
	  c = IORD_16DIRECT(Shared_Sdram_Flag, 0x2C);
	  received_value = IORD_16DIRECT(Shared_Sdram_Flag, 0x30);
	  IOWR_16DIRECT(Shared_Sdram_Flag,0x30,0);
	  altera_avalon_mutex_unlock(mutex);
	  OSSemPost(Semaphore);

//	  OSTimeDlyHMSM(0, 0, 0, 900);
	  OSTimeDly(2);

  }
}

void task2(void* pdata)
{
INT8U err;
int local_c = 0;
int local_beep = 0;
int local_received = 0;
char reading_buf[6];
while (1)
{
	IOWR(green_LED,0,1);
	IOWR(yellow_LED,0,1);
	IOWR(red_LED,0,1);

	OSSemPend(Semaphore, 0, &err);

	local_beep = beep;
	local_c = c;
	local_received = received_value;

	// 2. IMMEDIATELY clear the global variables so task1 doesn't re-trigger them
	beep = 0;
	c = 0;
	received_value = 0;

	OSSemPost(Semaphore);

	if (speakerOff==0)
	{
		if (local_beep==0x01)
		{
			beep_short();
			beep=0x00;
		}
		if ((local_c&0x01)!=0x00)
		{
			play_morse_char(c);
			c=0x00;
		}
		if ((local_received)!=0)
		{
			snprintf(reading_buf, sizeof(reading_buf), "%u", local_received);
			reading_buf[5] = '\0';

			handle_key0_morse(reading_buf);
			received_value=0;
		}
	}
	OSTimeDly(2);
	}
}

static void sys_tick_isr(void* context)
{
    IOWR_ALTERA_AVALON_TIMER_STATUS(
    		TIMER_10MS_BASE,
        0
    );

    OSTimeTick();
}
/* The main function creates two task and starts multi-tasking */
int main()
{
	int i;
    for(i = 0; i < 500000; i++) { __asm__("nop"); }

    for (int j = 0; j < 0x40; j++)
    	  {
    		usleep(1000);
    	    IOWR_16DIRECT(Shared_Sdram_Flag,j,0);
    	  }

	count=0;

	IOWR(LED9_0,0,count);
	IOWR(green_LED,0,1);
	IOWR(yellow_LED,0,1);
	IOWR(red_LED,0,1);

	beep_short();

	// Stop timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(
			TIMER_10MS_BASE,
	    0
	);

	// Load 10ms period manually
	// Example assumes 50 MHz clock:
	// 50,000,000 * 0.01 = 500000 = 0x7A120

	#define TICKS_10MS 500000

	IOWR_ALTERA_AVALON_TIMER_PERIODL(
			TIMER_10MS_BASE,
	    (TICKS_10MS & 0xFFFF)
	);

	IOWR_ALTERA_AVALON_TIMER_PERIODH(
			TIMER_10MS_BASE,
	    (TICKS_10MS >> 16)
	);

	// Clear timeout flag
	IOWR_ALTERA_AVALON_TIMER_STATUS(
			TIMER_10MS_BASE,
	    0
	);

	// Register ISR
	alt_ic_isr_register(
		TIMER_10MS_IRQ_INTERRUPT_CONTROLLER_ID,
		TIMER_10MS_IRQ,
	    sys_tick_isr,
	    NULL,
	    NULL
	);

	// Start continuous IRQ timer
	IOWR_ALTERA_AVALON_TIMER_CONTROL(
			TIMER_10MS_BASE,
	    ALTERA_AVALON_TIMER_CONTROL_ITO_MSK  |
	    ALTERA_AVALON_TIMER_CONTROL_CONT_MSK |
	    ALTERA_AVALON_TIMER_CONTROL_START_MSK
	);


	OSInit();

    Semaphore = OSSemCreate(1);

  OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
				  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

  OSTaskCreateExt(task2,
                  NULL,
                  (void *)&task2_stk[TASK_STACKSIZE-1],
                  TASK2_PRIORITY,
                  TASK2_PRIORITY,
                  task2_stk,
                  TASK_STACKSIZE,
                  NULL,
				  OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR );

  OSStart();


  return 0;
}
