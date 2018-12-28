/*
 * SPI_Master.cpp
 *
 * Created: 12/27/2018 7:25:08 PM
 *  Author: Andrew Ge
 */ 

 #ifndef F_CPU
 #define F_CPU 16000000UL
 #endif

 #include <avr/io.h>
 #include <util/delay.h>
 #include "SPI_Master.h"

 void SPI_InitMaster (void) {
	 // Set MOSI and SCK as outputs
	 DDRB |= (1<<MOSI) | (1<<SCK);

	 // Enable SPI as master, set clock rate to fck/16
	 SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); // strange note to investigate: if I attempt to change DDRB after this line, it seems to fail
 }

 void SPI_SetMode3 (void) {
	 SPCR |= (1<<CPHA)|(1<<CPOL);
 }

 void SPI_MasterTransmit (uint8_t data) {
	 SPDR = data;
	 while(!(SPSR & (1<<SPIF)))
	 ; // wait until transmission is completed
 }
