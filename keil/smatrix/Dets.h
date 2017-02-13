/* Dets.h
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

void DetsInit(void);

uint8_t DetsYes(uint8_t slot);
