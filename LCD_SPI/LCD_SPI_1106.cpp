/*
 * LCD_SPI_1106.cpp
 *
 * Created: 12/27/2018 7:18:49 PM
 *  Author: Andrew Ge
 */ 

 #ifndef F_CPU
 #define F_CPU 16000000UL
 #endif

 #include <avr/io.h>
 #include <util/delay.h>
 #include "LCD_SPI_1106.h"
 #include "SPI_Master.h"
 #include "defaultFont.cpp"

 /* void LCD_Init (void) {
	 DDRB |= (1<<LCD_RESET) | (1<<LCD_DC) | (1<<LCD_CS);

	 PORTB &= ~(1<<LCD_RESET);
	 _delay_us(5);
	 PORTB ^= (1<<LCD_RESET); // power cycle LCD
	 _delay_us(5);
	 PORTB &= ~(1<<LCD_DC); // set in command mode
	 SPI_MasterTransmit(0xAF); // turn on LCD
	 _delay_ms(100);
 } */

 void LCD_InitPins(void) {
	DDRB |= (1<<LCD_RESET) | (1<<LCD_DC) | (1<<LCD_CS);
 }

 void LCD_Init (void) {
	PORTB &= ~(1<<LCD_RESET);
	_delay_us(5);
	PORTB ^= (1<<LCD_RESET); // power cycle LCD
	_delay_us(5);

	PORTB &= ~(1<<LCD_DC); // set in command mode
	SPI_MasterTransmit(0xAF); // turn on LCD
	_delay_ms(100);
 }

 void LCD_SetupRAM (void) {
	PORTB &= ~(1<<LCD_DC); // set in command mode

	SPI_MasterTransmit(0x20); // set addressing mode
	SPI_MasterTransmit(0x00); // 0b00 = horizontal
	
	SPI_MasterTransmit(0x21); // set column start  and end address
	SPI_MasterTransmit(0x02); // 2
	SPI_MasterTransmit(0x82); // 130

	SPI_MasterTransmit(0x22); // set page start and end address
	SPI_MasterTransmit(0x00);
	SPI_MasterTransmit(0x07); 
 }

 void LCD_WriteChar (int x, int y, char c) {
	if (x >= 0 && x < 128 && y >= 0 && y < 64 - 5 && c < 255) { // check that coordinates are in bound, and that character exists
		PORTB &= ~(1<<LCD_DC); // set in command mode

		SPI_MasterTransmit(0x21);
		SPI_MasterTransmit(x + 2);  // RAM column offset of 2 since 1106 has buffer of size 132 centered on screen of width 128
		SPI_MasterTransmit(0x82);

		SPI_MasterTransmit(0x22);
		SPI_MasterTransmit(y/8); // this should round down to nearest page
		SPI_MasterTransmit(0x07);

		PORTB |= 1<<LCD_DC;

		for (int i = 0; i < 5; i++)
			SPI_MasterTransmit(font[i + c]);
	}
 }

 /*
 Notes on writing to LCD:
 write to RAM page 0-7 by using 0xB-
 write to high byte of column address using 0x1-
 write to low byte of column address using 0x0-
 */