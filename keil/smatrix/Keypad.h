/* Keypad.h
   Runs on MSP432
   Main file for smack on-rack interface
   Calls peripherals in separate files
   Jiacheng Jason He
   February 9, 2017
*/

// [1] [2] [3]
// [4] [5] [6]
// [7] [8] [9]
// [*] [0] [#]
/* TESTING
   P5.0	 COL1
	 P5.1	 COL2
	 P5.2  COL3
	 P4.0  ROW1
	 P4.1  ROW2
	 P4.2  ROW3
	 P4.3  ROW4
*/

/* KEYPAD
	 P1.3  COL1
	 P1.2  COL2
	 P1.1  COL3
	 P1.0  ROW1
	 P10.3 ROW2
	 P10.2 ROW3
	 P10.1 ROW4
*/

#include <stdint.h>

void SysTick_Init(uint32_t period);

void KeypadInitLP(void);

void KeypadInit(void);

uint8_t MatrixKeypad_ScanLP(int16_t *Num);

uint8_t MatrixKeypad_Scan(int16_t *Num);

void SysTick_Handler(void);

uint8_t KeypadGet(void);

uint8_t KeypadInt(uint8_t c);
