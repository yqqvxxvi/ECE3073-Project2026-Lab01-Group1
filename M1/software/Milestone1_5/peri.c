//#include "peri.h"
//
//void tone_delay(volatile int count)
//{
//    for (volatile int i = 0; i < count; i++);
//}
//
//void long_delay(volatile int count)
//{
//    for (volatile int i = 0; i < count; i++);
//}
//
//void beep_cycles(int cycles, int delay_count)
//{
//    int i;
//    for (i = 0; i < cycles; i++)
//    {
//        *speaker = 0x01;
//        tone_delay(delay_count);
//        *speaker = 0x00;
//        tone_delay(delay_count);
//    }
//}
//
//void speaker_beep(int duration_cycles)
//{
//    *speaker = 0x01;
//    volatile int i;
//    for (i = 0; i < duration_cycles; i++);
//    *speaker = 0x00;
//}
//
//void led_alert(int duration_cycles)
//{
//    *OutPort_LED8 = 0x01;
//    *green_LED    = 0x01;
//    volatile int i;
//    for (i = 0; i < duration_cycles; i++);
//    *OutPort_LED8 = 0x00;
//    *green_LED    = 0x00;
//}
//
//void signal_alert(int duration_cycles)
//{
//    *speaker      = 0x01;
//    *OutPort_LED8 = 0x01;
//    *green_LED    = 0x01;
//    volatile int i;
//    for (i = 0; i < duration_cycles; i++);
//    *speaker      = 0x00;
//    *OutPort_LED8 = 0x00;
//    *green_LED    = 0x00;
//}
//
//void signal_test(void)
//{
//    volatile int i;
//    #define DELAY 3000000
//
//    speaker_beep(DELAY);
//    for (i = 0; i < DELAY; i++);
//
//    led_alert(DELAY);
//    for (i = 0; i < DELAY; i++);
//
//    *green_LED = 0x01;
//    for (i = 0; i < DELAY; i++);
//    *green_LED = 0x00;
//    for (i = 0; i < DELAY; i++);
//
//    signal_alert(DELAY);
//}
