/* Locks.h
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

void LocksInit(void);

void LocksUnlk(uint8_t slot);

void LocksLock(uint8_t slot);
