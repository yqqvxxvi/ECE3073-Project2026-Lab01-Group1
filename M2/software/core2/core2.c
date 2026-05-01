#include "sys/alt_stdio.h"
#include <stdlib.h>
#include "io.h"
#include "system.h"
#include <stdio.h>
#include <stdint.h>
#include "altera_avalon_mutex.h"
#include <sys/alt_cache.h>

#define HEX012_BASE 				0x00021030
#define HEX345_BASE 				0x00021020
#define KEY01_BASE 					0x00021080
#define SDRAM_MUTEX_BASE 			0x02021080
#define SHARED_SDRAM_BASE 			0x01000000
#define SDRAM_1       				0x01100000
#define SDRAM_2       				0x01200000
#define ACCELEROMETER_SPI_0_BASE 	0x00021098
#define SPI_0_BASE 					0x00021000
#define SPI_SS_BASE 				0x00021050
#define US_COUNTER_BASE 			0x02021070


volatile int *timer      = (int *)US_COUNTER_BASE;
volatile int *HEX012      = (int *)HEX012_BASE;
volatile int *HEX345      = (int *)HEX345_BASE;
volatile int *ACCEL      = (int *)ACCELEROMETER_SPI_0_BASE;
volatile int *SPI      = (int *)SPI_0_BASE;
volatile int *SPI_SS      = (int *)SPI_SS_BASE;


volatile int *Key1       = (int *)KEY01_BASE;
volatile int *Mutex      = (int *)SDRAM_MUTEX_BASE;
volatile int *Shared_Sdram_Flag      = (int *)SHARED_SDRAM_BASE;
volatile int *Shared_Sdram_1      = (int *)SDRAM_1;
volatile int *Shared_Sdram_2      = (int *)SDRAM_2;

int main()
{ 
  uint32_t start_time, end_time;
  start_time = IORD(timer, 0);
  alt_putstr("Hello from core 2!\n");
  for (int i = 0; i < 200000; i++){i++;};
  end_time = IORD(timer, 0);

  printf("Time(us): %u\n\n",(unsigned int)(end_time-start_time));
  alt_mutex_dev* mutex=altera_avalon_mutex_open("/dev/SHARED_SDRAM");

  int LENGTH;
  alt_dcache_flush_all();
//  LENGTH=(IORD_16DIRECT(Shared_Sdram_Flag,0x10));
  while((IORD_16DIRECT(Shared_Sdram_Flag,0x10))==0x0);

  LENGTH=(IORD_16DIRECT(Shared_Sdram_Flag,0x10));
  printf("Length: %u\n\n",(int)(IORD_16DIRECT(Shared_Sdram_Flag,0x10)));

  altera_avalon_mutex_lock(mutex, 1);

    for (int i = 0; i < LENGTH; i++)
        {
        for (int j = 0; j < LENGTH; j++)
        {
//                int hold2=rand()-(32767/2);
          int hold2=3;
//          printf("index: %u\n\n",(
//                  ((i*LENGTH)+j)
//                  *sizeof(int))
//				  );

          IOWR_16DIRECT(Shared_Sdram_2,(
                  ((i*LENGTH)+j)
                  *sizeof(int)),hold2);
          IOWR_16DIRECT(Shared_Sdram_2,(
                  ((LENGTH*LENGTH)+(i*LENGTH)+j)
                  *sizeof(int)),0);
        }
    }

    IOWR_16DIRECT(Shared_Sdram_Flag,6,1);
    altera_avalon_mutex_unlock(mutex);

while((IORD_16DIRECT(Shared_Sdram_Flag,8)&0x01) == 0);

altera_avalon_mutex_lock(mutex, 1);

start_time = IORD_32DIRECT(timer, 0);
for (int i = LENGTH/2; i < LENGTH; i++)
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

//altera_avalon_mutex_unlock(mutex);

  printf("Time(us): %u\n\n",(unsigned int)(end_time-start_time));
  printf("A:\n");
    for (int i = 0; i < LENGTH; i++)
    {
        for (int j = 0; j < LENGTH; j++)
                     {

            unsigned int print=IORD_16DIRECT(Shared_Sdram_1,(
                        ((i*LENGTH)+j)
                        *sizeof(int)));
            printf("%u ",print);
                 }
        printf("\n");
    }

  printf("B:\n");
        for (int i = 0; i < LENGTH; i++)
             {
            for (int j = 0; j < LENGTH; j++)
                 {

                unsigned int print=IORD_16DIRECT(Shared_Sdram_2,(
                            ((i*LENGTH)+j)
                            *sizeof(int)));
                printf("%u ",print);
                     }
            printf("\n");
            }

  printf("AxB:\n");
  for (int i = 0; i < LENGTH; i++)
         {
        for (int j = 0; j < LENGTH; j++)
                 {

            unsigned int print=IORD_16DIRECT(Shared_Sdram_2,(
                            ((LENGTH*LENGTH)+(i*LENGTH)+j)
                            *sizeof(int)));
            printf("%u ",print);
             }
        printf("\n");
        }

  /* Event loop never exits. */
  while (1);

  return 0;
}
