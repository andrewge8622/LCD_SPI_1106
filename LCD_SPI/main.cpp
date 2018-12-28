/*
 * LCD_SPI.cpp
 *
 * Created: 12/26/2018 12:04:32 PM
 * Author : Andrew Ge
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "LCD_SPI_1106.h"
#include "SPI_Master.h"

// PORTC
// PC4 SDA
// PC5 SCL

#define LEDEXT 7

void statusBlink (void) {
	PORTD |= 1<<LEDEXT;
	_delay_ms(1000);
	PORTD ^= 1<<LEDEXT;
	_delay_ms(1000);
}

int main(void)
{
	DDRD = 1<<LEDEXT;
	LCD_InitPins();

	SPI_InitMaster(); // initialize SPI pins and set for communication with LCD
	SPI_SetMode3();

	LCD_Init(); // reset and turn on display

	PORTB &= ~(1<<LCD_DC); // set in command mode
	SPI_MasterTransmit(0b10100101);
	statusBlink();
	SPI_MasterTransmit(0xA4);
	statusBlink();
	SPI_MasterTransmit(0x02); // reset column start address to 2 - temporary measure for testing

	PORTB |= 1<<LCD_DC; // set in transmission mode

	for (int i = 0; i < 128; i++)
		SPI_MasterTransmit(0xAA);

	statusBlink();
    /* Replace with your application code */
    while (1) 
    {
    }
}
