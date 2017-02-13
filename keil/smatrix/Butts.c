/* Butts.c
   Runs on MSP432
   Main file for smack on-rack interface
   Calls peripherals in separate files
   Jiacheng Jason He
   February 9, 2017
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

uint8_t ButtsYes(uint8_t slot) {
	switch(slot) {
		case(1) :
			if (P5IN&0x10) {
				return 1;
			} else {
				return 0;
			}
		case(2) :
			if (P5IN&0x01) {
				return 1;
			} else {
				return 0;
			}
		case(3) :
			if (P4IN&0x20) {
				return 1;
			} else {
				return 0;
			}
		case(4) :
			if (P7IN&0x40) {
				return 1;
			} else {
				return 0;
			}
	}
}
