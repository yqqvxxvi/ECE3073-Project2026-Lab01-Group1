#include "nios2_ctrl_reg_macros.h"



// function prototypes int main(void);
int main(void);
void interrupt_handler(void);
void the_exception(void);

/* Declare volatile pointers to I/O registers. This will ensure that the resulting code will bypass the cache*/

// Example declarations shown below

volatile int *OutPort_LEDR      = (int *) 0x04009040;  // LEDs
volatile int *OutPort_LED8      = (int *) 0x04009000;  // LED8
volatile int *InPort_KEY1       = (int *) 0x04009010;  // KEY1
volatile int *InPort_COUNTER    = (int *) 0x04009020;
volatile int *Counter_EdgeCap   = (int *) 0x0400902C;
volatile int *Counter_IRQMask   = (int *) 0x04009028;
volatile int *GPIO2_Port       = (int *) 0x04009030;
int main(void)
{
	int sleepy = 0; // 
	int temp_counter = 0;
	// Below 3 lines of code will be needed to set interrupts 
	// 1. set up rising edge triggered interrupts for the counter PIO 
	*Counter_EdgeCap = 0; // clear pending edge
	// 2. set interrupt enable mask bit for counter IRQ level 0
	*Counter_IRQMask = 1;
	//3. enable Nios II interrupts (PIE bit in status register)
	NIOS2_WRITE_IENABLE(0x1); // enable IRQ0
	NIOS2_WRITE_STATUS(0x1);  // global interrupt enable

	int LED_counter = 0;
	int prev_counter;
	int prev_key;
	int LED8_state = 0;

	prev_counter = (*InPort_COUNTER) & 0x01;
	prev_key = (*InPort_KEY1) & 0x01;

	while (1) //normal code infinite loop
	{

		// Write the code for toggling the LED8- same as poling part
		int curr_key = (*InPort_KEY1) & 0x01;

		// Rising edge on KEY1
		if ((prev_key == 0) && (curr_key == 1))
		{
			LED8_state = !LED8_state;
			*OutPort_LED8 = LED8_state;
		}

		prev_key = curr_key;
		// Add codes like below a long for loop that ensures debouncing //of switch not read
		for(sleepy = 0; sleepy < 10000; sleepy++)
		{
			temp_counter++; //for loop needs some code to be run
		}
	}
}
