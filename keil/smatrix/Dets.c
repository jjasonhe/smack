/* Dets.c
   Runs on MSP432
   Main file for smack on-rack interface
   Calls peripherals in separate files
   Jiacheng Jason He
   February 9, 2017
*/

/* DETS
	 P6.4  DET1
	 P4.6  DET2
	 P6.5  DET3
	 P7.4  DET4
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

void DetsInitLP(void) {
	P6SEL0 &= ~0x30;
	P6SEL1 &= ~0x30;
	P6DIR  &= ~0x30;
	P4SEL0 &= ~0x40;
	P4SEL1 &= ~0x40;
	P4DIR	 &= ~0x40;
	P7SEL0 &= ~0x10;
	P7SEL1 &= ~0x10;
	P7DIR	 &= ~0x10;
}

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

uint8_t DetsyLP(uint8_t slot) {
	switch(slot) {
		case(1) :
			if (P6IN&0x10) {
				return 0;
			} else {
				return 1;
			}
		case(2) :
			if (P4IN&0x40) {
				return 0;
			} else {
				return 1;
			}
		case(3) :
			if (P6IN&0x20) {
				return 0;
			} else {
				return 1;
			}
		case(4) :
			if (P7IN&0x10) {
				return 0;
			} else {
				return 1;
			}
	}
}

uint8_t Detsy(uint8_t slot) {
	switch(slot) {
		case(1) :
			if (P5IN&0x04) {
				return 0;
			} else {
				return 1;
			}
		case(2) :
			if (P4IN&0x40) {
				return 0;
			} else {
				return 1;
			}
		case(3) :
			if (P4IN&0x08) {
				return 0;
			} else {
				return 1;
			}
		case(4) :
			if (P7IN&0x10) {
				return 0;
			} else {
				return 1;
			}
	}
}
