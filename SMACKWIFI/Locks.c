/* Locks.c
   Runs on MSP432
   Main file for smack on-rack interface
   Calls peripherals in separate files
   Jiacheng Jason He
   February 9, 2017
*/

/* LOCKS
   P5.3  M1
	 P4.7  M2
	 P4.4  M3
	 P7.5  M4
*/

#include <stdint.h>
#include "msp432p401r.h"
#include "Locks.h"

void LocksInit(void) {
	P5SEL0 &= ~0x08;
	P5SEL1 &= ~0x08;
	P5DIR  |=  0x08;
	P4SEL0 &= ~0x90;
	P4SEL1 &= ~0x90;
	P4DIR	 |=  0x90;
	P7SEL0 &= ~0x20;
	P7SEL1 &= ~0x20;
	P7DIR	 |=  0x20;
}

void LocksUnlk(uint8_t slot) {
	switch(slot) {
		case(1) :
			P5OUT |= 0x08;
			break;
		case(2) :
			P4OUT |= 0x80;
			break;
		case(3) :
			P4OUT |= 0x10;
			break;
		case(4) :
			P7OUT |= 0x20;
			break;
	}
}

void LocksLock(uint8_t slot) {
	switch(slot) {
		case(1) :
			P5OUT &= ~0x08;
			break;
		case(2) :
			P4OUT &= ~0x80;
			break;
		case(3) :
			P4OUT &= ~0x10;
			break;
		case(4) :
			P7OUT &= ~0x20;
			break;
	}
}
