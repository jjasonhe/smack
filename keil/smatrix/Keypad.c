/* Keypad.c
   Runs on MSP432
   Main file for smack on-rack interface
   Calls peripherals in separate files
   Jiacheng Jason He
   February 9, 2017
	 Based on MatrixKeypadPeriodic by Jon Valvano
*/

// [1] [2] [3]
// [4] [5] [6]
// [7] [8] [9]
// [*] [0] [#]
/* TESTING
	 P4.3 COL1
	 P4.2 COL2
	 P4.1 COL3
	 P4.0 ROW1
	 P5.2 ROW2
	 P5.1 ROW3
	 P5.0 ROW4
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
#include "msp432p401r.h"
#include "Keypad.h"
#include "FIFO.h"
//#include "driverlib.h"
#include "sl_common.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

#define FIFOSIZE   16         // size of the FIFOs (must be power of 2)
#define FIFOSUCCESS 1         // return value on success
#define FIFOFAIL    0         // return value on failure
AddIndexFifo(Matrix, FIFOSIZE, uint8_t, FIFOSUCCESS, FIFOFAIL) // create a FIFO
volatile uint32_t HeartBeat;  // incremented every 25 ms
static uint8_t LastKey;

void TimerA_Init(uint32_t period) {
	long sr;
	sr = StartCritical();
	TIMER_A0->CTL &= ~0x0030; // stop mode
	TIMER_A0->CTL = 0x0202; // SMCLK, enable int
	TIMER_A0->CCTL[0] = 0x0010; // enable CC int
	TIMER_A0->CCR[0] = period - 1;
	TIMER_A0->EX0 &= ~0x0007; // div /1
	NVIC->IP[2] = (NVIC->IP[2]&0xFFFFFF00)|0x00000040; // priority 2
	NVIC->ISER[0] = 0x00000100;
	TIMER_A0->CTL |= 0x0014;
	EndCritical(sr);
}

void KeypadInitLP(void) {
	long sr;
	sr = StartCritical();
  // **** general initialization ****
  MatrixFifo_Init();               // initialize empty FIFOs
  HeartBeat = 0;
  LastKey = 0;
  P4DIR &= ~0x0F;                  // make P7.3-P7.0 in (P7.3-P7.0 rows)
  P5DIR &= ~0x07;                  // make P4.3-P4.0 in (P4.3-P4.0 columns)
	TimerA_Init(1200000);							 // 25 ms polling
  EndCritical(sr);
}

void KeypadInit(void) {
	long sr;
	sr = StartCritical();
  MatrixFifo_Init();               // initialize empty FIFOs
  HeartBeat = 0;
  LastKey = 0;
	P1DIR  &= ~0x0F;
	P10DIR &= ~0x0E;
	TimerA_Init(1200000);							 // 25 ms polling
  //SysTick_Init(1200000);             // Program 5.12, 25 ms polling
  EndCritical(sr);
}

struct Row{
  uint8_t direction;// direction register mask
  uint8_t keycode[3];};
typedef const struct Row RowType;
RowType ScanTab[5]={
{   0x01, "123" }, // row 0
{   0x02, "456" }, // row 1
{   0x04, "789" }, // row 2
{   0x08, "*0#" }, // row 3
{   0x00, "   " }};

uint8_t MatrixKeypad_ScanLP(int16_t *Num){
  RowType *pt;
  uint8_t key;
  uint8_t column;
  uint16_t j;
  (*Num) = 0;
  key = 0;    // default values
  pt = &ScanTab[0];
  while(pt->direction){
    P4DIR = ((pt->direction)&0x01);
		P5DIR = (((pt->direction)&0x08)>>3)|(((pt->direction)&0x04)>>1)|(((pt->direction)&0x02)<<1);
		P4OUT &= ~0x01;                // DIRn=0, OUTn=HiZ; DIRn=1, OUTn=0
		P5OUT &= ~0x07;                // DIRn=0, OUTn=HiZ; DIRn=1, OUTn=0
    for(j=1; j<=10; j++);          // very short delay
    column = ((P4IN&0x02)<<1)|((P4IN&0x04)>>1)|((P4IN&0x08)>>3);
    for(j=0; j<=2; j++){
      if((column&0x01)==0){
        key = pt->keycode[j];
        (*Num)++;
      }
      column>>=1;  // shift into position
    }
    pt++;
  }
  return key;
}

uint8_t MatrixKeypad_Scan(int16_t *Num){
  RowType *pt;
  uint8_t key;
  uint8_t column;
  uint16_t j;
  (*Num) = 0;
  key = 0;    // default values
  pt = &ScanTab[0];
  while(pt->direction){
    P1DIR = ((pt->direction)&0x01);
		P10DIR = (((pt->direction)&0x08)>>2)|(((pt->direction)&0x04)>>1)|(((pt->direction)&0x02)<<2);
		P1OUT &= ~0x01;                // DIRn=0, OUTn=HiZ; DIRn=1, OUTn=0
		P10OUT &= ~0x0E;                // DIRn=0, OUTn=HiZ; DIRn=1, OUTn=0
    for(j=1; j<=10; j++);          // very short delay
    column = ((P1IN&0x02)<<1)|((P1IN&0x04)>>1)|((P1IN&0x08)>>3);
    for(j=0; j<=2; j++){
      if((column&0x01)==0){
        key = pt->keycode[j];
        (*Num)++;
      }
      column>>=1;  // shift into position
    }
    pt++;
  }
  return key;
}

void TA0_0_IRQHandler(void) {
	TIMER_A0->CCTL[0] &= ~0x0001;
	uint8_t thisKey;
	int16_t n;
  thisKey = MatrixKeypad_Scan(&n); // scan
  if((thisKey != LastKey) && (n == 1)){
    MatrixFifo_Put(thisKey);
    LastKey = thisKey;
  } else if(n == 0){
    LastKey = 0; // invalid
  }
  HeartBeat++;
}

uint8_t KeypadGet(void){
	uint8_t letter;
  while(MatrixFifo_Get(&letter) == FIFOFAIL){
		if (timesUp) {
			letter = 0;
			break;
		}
    WaitForInterrupt();            // there will not be any characters until at least the next interrupt
  }
  return(letter);
}

uint8_t KeypadInt(uint8_t c){
	uint8_t ret;
	switch(c) {
		case '0' :
			ret = 0;
			break;
		case '1' :
			ret = 1;
			break;
		case '2' :
			ret = 2;
			break;
		case '3' :
			ret = 3;
			break;
		case '4' :
			ret = 4;
			break;
		case '5' :
			ret = 5;
			break;
		case '6' :
			ret = 6;
			break;
		case '7' :
			ret = 7;
			break;
		case '8' :
			ret = 8;
			break;
		case '9' :
			ret = 9;
			break;
	}
	return ret;
}
