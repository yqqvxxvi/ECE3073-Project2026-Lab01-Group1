#include "nios2_ctrl_reg_macros.h"


// function prototypes int main(void);
void interrupt_handler(void); void the_exception(void);

/* Declare volatile pointers to I/O registers. This will ensure that the resulting code will bypass the cache*/

// Example declarations shown below

volatile int * OutPort_LEDR = (int *) 0x08009040; // 8 Led
volatile int * InPort_COUNTER = (int *) 0x08009020; // MSB
volatile int * Dram = (int *) 0x04000000; //
volatile int * Key1 = (int *) 0x08009010; //
volatile int * LED8 = (int *) 0x08009000; //
volatile int * GPIO = (int *) 0x08009030; //


int main(void)
{
	int sleepy = 0; // 
	int temp_counter = 0;
	// Below 3 lines of code will be needed to set interrupts 
	// 1. set up rising edge triggered interrupts for the counter PIO 
	// 2. set interrupt enable mask bit for counter IRQ level 0
	//3. enable Nios II interrupts (PIE bit in status register)
	while (1) //normal code infinite loop
	{

		// Write the code for toggling the LED8- same as poling part
		// Add codes like below a long for loop that ensures debouncing //of switch not read
		/*for(sleepy = 0; sleepy < 10000; sleepy++)
		{
		temp_counter++; //for loop needs some code to be run
		}*/
	}
}
