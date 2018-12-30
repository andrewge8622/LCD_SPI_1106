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
	char word[] = "testedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtestedtested";
	DDRD = 1<<LEDEXT;
	LCD_InitPins();

	SPI_InitMaster(); // initialize SPI pins and set for communication with LCD
	SPI_SetMode3();

	LCD_Init(); // reset and turn on display

	LCD_ClearScreen();



	LCD_WriteWord(117, 32, 150, word, false);
	LCD_WriteChar(1, 18, 'h');
	LCD_WriteChar(64, 27, 'a');
	LCD_WriteChar(117, 32, 'm');


	statusBlink();
    /* Replace with your application code */
    while (1) 
    {
    }
}
