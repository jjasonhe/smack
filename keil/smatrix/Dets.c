/* Dets.c
   Runs on MSP432
   Main file for smack on-rack interface
   Calls peripherals in separate files
   Jiacheng Jason He
   February 9, 2017
*/

/* DETS
	 P5.2  DET1
	 P4.6  DET2
	 P4.3  DET3
	 P7.4  DET4
*/

#include <stdint.h>
#include "msp432p401r.h"
#include "Dets.h"

void DetsInit(void) {
	P5SEL0 &= ~0x04;
	P5SEL1 &= ~0x04;
	P5DIR  &= ~0x04;
	P4SEL0 &= ~0x48;
	P4SEL1 &= ~0x48;
	P4DIR	 &= ~0x48;
	P7SEL0 &= ~0x10;
	P7SEL1 &= ~0x10;
	P7DIR	 &= ~0x10;
}

uint8_t DetsYes(uint8_t slot) {
	switch(slot) {
		case(1) :
			if (P5IN&0x04) {
				return 1;
			} else {
				return 0;
			}
		case(2) :
			if (P4IN&0x40) {
				return 1;
			} else {
				return 0;
			}
		case(3) :
			if (P4IN&0x08) {
				return 1;
			} else {
				return 0;
			}
		case(4) :
			if (P7IN&0x10) {
				return 1;
			} else {
				return 0;
			}
	}
}
