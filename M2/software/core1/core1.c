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

volatile int *buzzer      = (int *)BUZZER_BASE;
volatile int *Mutex      = (int *)SDRAM_MUTEX_BASE;
volatile int *Shared_Sdram_Flag      = (int *)SHARED_SDRAM_BASE;
volatile int *Shared_Sdram_1      = (int *)SDRAM_1;
volatile int *Shared_Sdram_2      = (int *)SDRAM_2;
volatile int *Switch9_0      = (int *)SWITCH_BASE;
volatile int *green_light      = (int *)GREEN_LIGHT_BASE;
volatile int *yellow_light      = (int *)YELLOW_LIGHT_BASE;
volatile int *red_light      = (int *)RED_LIGHT_BASE;
volatile int *timer      = (int *)US_COUNTER_BASE;

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
int LENGTH=IORD_32DIRECT(Switch9_0, 0) & 0x7F; //SW[6:0]

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

//printf("length: %u\n\n",(unsigned int)LENGTH);
IOWR_16DIRECT(Shared_Sdram_Flag,0x10,LENGTH);
//printf("Length: %u\n\n",(unsigned int)(IORD_16DIRECT(Shared_Sdram_Flag,0)&0x0F));
alt_dcache_flush(Shared_Sdram_Flag,((LENGTH*LENGTH)));
//printf("Length: %u\n\n",(unsigned int)(IORD_16DIRECT(Shared_Sdram_Flag,0)&0x0F));

altera_avalon_mutex_unlock(mutex);
//IOWR_16DIRECT(Shared_Sdram_Flag,0x10,LENGTH);
//printf("Length: %u\n\n",(unsigned int)(IORD_16DIRECT(Shared_Sdram_Flag,0)&0x0F));


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


  while (1);

  return 0;
}


