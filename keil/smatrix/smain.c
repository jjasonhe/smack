/* smain.c
   Runs on MSP432
   Main file for smack on-rack interface
   Calls peripherals in separate files
   Jiacheng Jason He
   February 9, 2017
*/

/* LCD
	 GND	 GND
	 +3V3	 VCC
	 P9.3	 RESET
	 P9.2	 C/D
	 X		 CARD_CS
	 P9.4	 TFT_CS
	 P9.7	 MOSI
	 P9.5	 SCLK
	 X		 MISO
	 +3V3	 LIGHT
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

/* LOCKS
   P5.3  M1
	 P4.7  M2
	 P4.4  M3
	 P7.5  M4
*/

/* BUTTS
	 P5.4  S1
	 P5.0  S2
	 P4.5  S3
	 P7.6  S4
*/

/* DETS
	 P5.2  DET1
	 P4.6  DET2
	 P4.3  DET3
	 P7.4  DET4
*/

#include <stdint.h>
#include "msp432p401r.h"
#include "Keypad.h"
#include "LCD.h"
#include "Locks.h"
#include "Butts.h"
#include "Dets.h"
#include "ClockSystem.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

#define SLOTS 4
#define RACKNUM 1
#define SEC_TEN 240000000			// assuming 24MHz clock? SMCLK?
#define SEC_TWO  48000000			// assuming 24MHz clock? SMCLK?
#define SEC_30  720000000			// assuming 24MHz clock? SMCLK?

volatile uint8_t key;
uint8_t keyInt;
uint8_t slot;
uint8_t localStatus[SLOTS];			// 0 = free, 1 = taken
uint8_t localPins[SLOTS][4];
uint8_t timesUp;

void OneShot(uint32_t period){long sr;
  sr = StartCritical();
  TIMER32_LOAD1 = period-1;    // timer reload value
  TIMER32_INTCLR1 = 0x00000001;    // clear Timer32 Timer 1 interrupt
  // bits31-8=X...X,   reserved
  // bit7=1,           timer enable
  // bit6=1,           timer in periodic mode
  // bit5=1,           interrupt enable
  // bit4=X,           reserved
  // bits3-2=??,       input clock divider according to parameter
  // bit1=1,           32-bit counter
  // bit0=0,           one-shot mode
  TIMER32_CONTROL1 = 0x000000E3;
// interrupts enabled in the main program after all devices initialized
  NVIC_IPR6 = (NVIC_IPR6&0xFFFF00FF)|0x00004000; // priority 2
  NVIC_ISER0 = 0x02000000;         // enable interrupt 25 in NVIC
  EndCritical(sr);
}

void T32_INT1_IRQHandler(void){
  TIMER32_INTCLR1 = 0x00000001;    // acknowledge Timer32 Timer 1 interrupt
	timesUp = 1;
}

int main(void) {
	//Clock_Init48MHz();					// doesn't work with Keypad yet
	KeypadInit();
	LocksInit();
	ButtsInit();
	DetsInit();
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_RED);
	ST7735_SetTextColor(ST7735_WHITE);
	//WiFiInit();
	
	while(1) {
		//localStatus = WiFiFetchStatus();
		//localPins = WiFiFetchPins();
		
		ST7735_FillScreen(ST7735_RED);
		ST7735_OutString("Hi! Press # to begin.");
		key = KeypadGet();
		if (key != '#') {
			continue;
		}
		
		while(1) {
			//localStatus = WiFiFetchStatus();
			//localPins = WiFiFetchPins();
			// update localStatus based on ButtsYes + DetsYes
			// if diff, WiFiPushStatus();
			//          WiFiPushPins();
			
			ST7735_FillScreen(ST7735_RED);
			ST7735_OutString("Enter the slot number you want to access.");
			key = KeypadGet();
			if ((key!='1')&&(key!='2')&&(key!='3')&&(key!='4')) {
				ST7735_FillScreen(ST7735_RED);
				ST7735_OutString("Invalid slot number!");
				continue;
			}
			
			slot = KeypadInt(key);
			
			if (!localStatus[keyInt]) {
				localStatus[slot] = 1;
				//WiFiPushStatus();			  // push "taken" so web users can't also claim
				timesUp = 0;
				OneShot(SEC_TEN);					// start 10 second timer (one-shot)
				while(1) {
					if (timesUp) {
						break;
					}
					ST7735_FillScreen(ST7735_RED);
					ST7735_OutString("Enter your desired pin.");
					
					key = KeypadGet();
					if ((key!='1')&&(key!='2')&&(key!='3')&&(key!='4')&&(key!='5')&&(key!='6')&&(key!='7')&&(key!='8')&&(key!='9')&&(key!='0')) {
						ST7735_FillScreen(ST7735_RED);
						ST7735_OutString("Invalid key! Try again.");
						continue;
					}
					localPins[slot][0] = KeypadInt(key);
					ST7735_SetCursor(0,1);
					ST7735_OutString("*");
					
					key = KeypadGet();
					if ((key!='1')&&(key!='2')&&(key!='3')&&(key!='4')&&(key!='5')&&(key!='6')&&(key!='7')&&(key!='8')&&(key!='9')&&(key!='0')) {
						ST7735_FillScreen(ST7735_RED);
						ST7735_OutString("Invalid key! Try again.");
						continue;
					}
					localPins[slot][1] = KeypadInt(key);
					ST7735_SetCursor(0,3);
					ST7735_OutString("*");
					
					key = KeypadGet();
					if ((key!='1')&&(key!='2')&&(key!='3')&&(key!='4')&&(key!='5')&&(key!='6')&&(key!='7')&&(key!='8')&&(key!='9')&&(key!='0')) {
						ST7735_FillScreen(ST7735_RED);
						ST7735_OutString("Invalid key! Try again.");
						continue;
					}
					localPins[slot][2] = KeypadInt(key);
					ST7735_SetCursor(0,5);
					ST7735_OutString("*");
					
					key = KeypadGet();
					if ((key!='1')&&(key!='2')&&(key!='3')&&(key!='4')&&(key!='5')&&(key!='6')&&(key!='7')&&(key!='8')&&(key!='9')&&(key!='0')) {
						ST7735_FillScreen(ST7735_RED);
						ST7735_OutString("Invalid key! Try again.");
						continue;
					}
					localPins[slot][3] = KeypadInt(key);
					ST7735_SetCursor(0,7);
					ST7735_OutString("*");
					break;
				}
				if (timesUp) {
					localStatus[slot] = 0;
					//WiFiPushStatus();
					timesUp = 0;
					ST7735_FillScreen(ST7735_RED);
					ST7735_OutString("Took too long! Start over.");
					OneShot(SEC_TWO);
					while(!timesUp) {}
					continue;
				} else {
					TIMER32_INTCLR1 = 0x00000001;    // acknowledge Timer32 Timer 1 interrupt
					TIMER32_CONTROL1 = 0x00000043;	 // disable timer, disable interrupt
					timesUp = 0;
					
					//WiFiPushPins();
					ST7735_FillScreen(ST7735_RED);
					ST7735_OutString("Sweet! Go lock your bike!");
					LocksUnlk(slot);
					OneShot(SEC_30);
					while(!(ButtsYes(slot)&&DetsYes(slot))) {
						if (timesUp) {
							break;
						}
					}
					if (timesUp) {
						localStatus[slot] = 0;
						localPins[slot][0] = 0;
						localPins[slot][1] = 0;
						localPins[slot][2] = 0;
						localPins[slot][3] = 0;
						//WiFiPushStatus();
						//WiFiPushPins();
						timesUp = 0;
						ST7735_FillScreen(ST7735_RED);
						ST7735_OutString("Took too long! Start over.");
						OneShot(SEC_TWO);
						while(!timesUp) {}
						continue;
					} else {
						TIMER32_INTCLR1 = 0x00000001;    // acknowledge Timer32 Timer 1 interrupt
						TIMER32_CONTROL1 = 0x00000043;	 // disable timer, disable interrupt
						timesUp = 0;
						
						LocksLock(slot);
						ST7735_FillScreen(ST7735_RED);
						ST7735_OutString("See ya later, alligator!");
					}
				}
			}
		}
	}
}

void mainLP(void) {
	//Clock_Init48MHz();					// doesn't work with Keypad yet
	KeypadInitLP();
	LocksInit();
	ButtsInit();
	DetsInit();
	ST7735_InitR(INITR_REDTAB);
	ST7735_OutString("Graphics test\n");
}
