/* Butts.h
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

void ButtsInitLP(void);

void ButtsInit(void);

uint8_t ButtsyLP(uint8_t slot);

uint8_t Buttsy(uint8_t slot);
