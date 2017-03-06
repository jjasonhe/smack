/* Butts.c
   Runs on MSP432
   Main file for smack on-rack interface
   Calls peripherals in separate files
   Jiacheng Jason He
   February 9, 2017
*/

/* TESTING
	 P5.4  S1
	 P6.0  S2
	 P4.5  S3
	 P7.6  S4
*/

/* BUTTS
	 P5.4  S1
	 P5.0  S2
	 P4.5  S3
	 P7.6  S4
*/

#include <stdint.h>
#include "msp432p401r.h"
#include "Butts.h"

void ButtsInitLP(void) {
	P5SEL0 &= ~0x10;
	P5SEL1 &= ~0x10;
	P5DIR  &= ~0x10;
	P6SEL0 &= ~0x01;
	P6SEL1 &= ~0x01;
	P6DIR  &= ~0x01;
	P4SEL0 &= ~0x20;
	P4SEL1 &= ~0x20;
	P4DIR	 &= ~0x20;
	P7SEL0 &= ~0x40;
	P7SEL1 &= ~0x40;
	P7DIR  &= ~0x40;
}

void ButtsInit(void) {
	P5SEL0 &= ~0x11;
	P5SEL1 &= ~0x11;
	P5DIR  &= ~0x11;
	P4SEL0 &= ~0x20;
	P4SEL1 &= ~0x20;
	P4DIR	 &= ~0x20;
	P7SEL0 &= ~0x40;
	P7SEL1 &= ~0x40;
	P7DIR  &= ~0x40;
}

uint8_t ButtsyLP(uint8_t slot) {
	switch(slot) {
		case(1) :
			if (P5IN&0x10) {
				return 0;
			} else {
				return 1;
			}
		case(2) :
			if (P6IN&0x01) {
				return 0;
			} else {
				return 1;
			}
		case(3) :
			if (P4IN&0x20) {
				return 0;
			} else {
				return 1;
			}
		case(4) :
			if (P7IN&0x40) {
				return 0;
			} else {
				return 1;
			}
	}
}

uint8_t Buttsy(uint8_t slot) {
	switch(slot) {
		case(1) :
			if (P5IN&0x10) {
				return 0;
			} else {
				return 1;
			}
		case(2) :
			if (P5IN&0x01) {
				return 0;
			} else {
				return 1;
			}
		case(3) :
			if (P4IN&0x20) {
				return 0;
			} else {
				return 1;
			}
		case(4) :
			if (P7IN&0x40) {
				return 0;
			} else {
				return 1;
			}
	}
}
