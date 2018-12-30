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

 void LCD_SetCommandMode(void) {
	PORTB &= ~(1<<LCD_DC);
 }

 void LCD_SetDisplayMode(void) {
	PORTB |= 1<<LCD_DC;
 }

 void LCD_SetColumnStart(int x) {
	if (x >= 0 && x < 132) {
		LCD_SetCommandMode();
		SPI_MasterTransmit(x & 0x0F); // splits into lower and higher 4 nibbles
		SPI_MasterTransmit(((x & 0xF0) >> 4) + 0x10);
	}
 }

 void LCD_SetPageStart(int x) {
	if (x >= 0 && x < 8) {
		LCD_SetCommandMode();
		SPI_MasterTransmit(x + 0xB0);
	}
 }

 void LCD_InitPins(void) {
	DDRB |= (1<<LCD_RESET) | (1<<LCD_DC) | (1<<LCD_CS);
 }

 void LCD_Init (void) {
	PORTB &= ~(1<<LCD_RESET);
	_delay_us(5);
	PORTB ^= (1<<LCD_RESET); // power cycle LCD
	_delay_us(5);
	LCD_SetCommandMode();
	SPI_MasterTransmit(0xAF); // turn on LCD
	_delay_ms(100);
 }

 void LCD_SetupRAM (void) {
	LCD_SetCommandMode();
	SPI_MasterTransmit(0x02); 
	SPI_MasterTransmit(0x10); // set RAM column start to 2, accounting for display's 132 pixel
	SPI_MasterTransmit(0xB0); // set page to 0
 }

 void LCD_ClearScreen (bool on) {
	unsigned char fill = (on) ? 0xFF : 0x00; // fill screen with either white (on) or black (off)
	LCD_SetCommandMode(); // set column to 0
	SPI_MasterTransmit(0x00);
	SPI_MasterTransmit(0x10);

	for (int x = 0; x < 8; x++) {
		SPI_MasterTransmit(0xB0 + x);
		LCD_SetDisplayMode();
		for (int i = 0; i < 132; i++)
			SPI_MasterTransmit(fill);
		LCD_SetCommandMode();
	}
 }

 void LCD_WriteWord (int x, int y, int length, char* word, bool align) {
	int charStart = x + 2; // tracks starting index of current char
	int pageIndex = y/8;

	// check that coordinates are in bound, and at least one character can be written at point
	if (x >= 0 && x < 128 - 5 && y >= 0 && y/8 < 8) { 
		LCD_SetColumnStart(x + 0x2); // RAM column offset of 2 since 1106 has buffer of size 132 centered on screen of width 128
		LCD_SetPageStart(y/8); // set page address

		LCD_SetDisplayMode();
		for (int j = 0; j < length; j++) {
			if (charStart > 130 - 5) { // check if there is room for next char
				LCD_SetCommandMode();
				LCD_SetPageStart((++pageIndex) % 8); // increment page, with wraparound
				if (align) {
					LCD_SetColumnStart(x + 0x2); // align wrapped text with initial start point
					charStart = x + 0x2;
				}
				else {
					LCD_SetColumnStart(0x2); // align wrapped text with screen
					charStart = 0x2;
				}
				LCD_SetDisplayMode();
			}
			charStart += 6;

			for (int i = 0; i < 5; i++)
				SPI_MasterTransmit(~font[i + word[j]*5]);
			SPI_MasterTransmit(0xFF);
		}
	}
 }

 void LCD_WriteChar (int x, int y, char c) {
	if (x >= 0 && x < 128 - 5 && y >= 0 && y/8 < 8 && c < 255) { // check that coordinates are in bound, and that character exists
		LCD_SetCommandMode();

		LCD_SetColumnStart(x + 0x2); // RAM column offset of 2 since 1106 has buffer of size 132 centered on screen of width 128
		LCD_SetPageStart(y/8); // set page address Text is aligned with RAM pages

		LCD_SetDisplayMode();
		for (int i = 0; i < 5; i++)
			SPI_MasterTransmit(~font[i + c*5]);
		SPI_MasterTransmit(0xFF);
	}
 }

 /*
 Notes on writing to LCD:
 write to RAM page 0-7 by using 0xB-
 write to high byte of column address using 0x1-
 write to low byte of column address using 0x0-
 */