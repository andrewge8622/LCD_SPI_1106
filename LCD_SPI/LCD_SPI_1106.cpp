/*
 * LCD_SPI_1106.cpp
 *
 * Created: 12/27/2018 7:18:49 PM
 * Author: Andrew Ge
 * Description: A library written to set up a LCD display built on the SSH1106
                controller, operating in SPI 4-wire mode
				For use with ATMega328P, on an Arduino Nano board
 */ 

/*
Notes on writing to LCD:
write to RAM page 0-7 by using 0xB-
write to high byte of column address using 0x1-
write to low byte of column address using 0x0-
*/

 #ifndef F_CPU
 #define F_CPU 16000000UL
 #endif

 #include <avr/io.h>
 #include <util/delay.h>
 #include "LCD_SPI_1106.h"
 #include "SPI_Master.h"
 #include "defaultFont.cpp"

 /*
 Indicate to LCD controller that SPI transmissions are commands
 */
 void LCD_SetCommandMode(void) {
	PORTB &= ~(1<<LCD_DC);
 }

 /*
 Indicate to LCD controller that SPI transmissions are addressed to display
 */
 void LCD_SetDisplayMode(void) {
	PORTB |= 1<<LCD_DC;
 }

 /*
 Set the column address for subsequent display writes
 */
 void LCD_SetColumnStart(int x) {
	if (x >= 0 && x < 132) {
		LCD_SetCommandMode();
		SPI_MasterTransmit(x & 0x0F); // splits into lower and higher 4 nibbles
		SPI_MasterTransmit(((x & 0xF0) >> 4) + 0x10);
	}
 }

 /*
 Set the RAM page for subsequent display writes
 There are 8 pages of height 8
 */
 void LCD_SetPageStart(int x) {
	if (x >= 0 && x < 8) {
		LCD_SetCommandMode();
		SPI_MasterTransmit(x + 0xB0);
	}
 }

 /*
 Set appropriate pins as outputs
 */
 void LCD_InitPins(void) {
	DDRB |= (1<<LCD_RESET) | (1<<LCD_DC) | (1<<LCD_CS);
 }

 /*
 Initialize LCD display
 */
 void LCD_Init (void) {
	PORTB &= ~(1<<LCD_RESET);
	_delay_us(5);
	PORTB ^= (1<<LCD_RESET); // power cycle LCD
	_delay_us(5);
	LCD_SetCommandMode();
	SPI_MasterTransmit(0xAF); // turn on LCD
	_delay_ms(100);
 }

 /*
 Set up LCD for writes beginning at upper-left corner of display
 */
 void LCD_SetupRAM (void) {
	LCD_SetCommandMode();
	SPI_MasterTransmit(0x02); 
	SPI_MasterTransmit(0x10); // set RAM column start to 2, accounting for display's 132 pixel
	SPI_MasterTransmit(0xB0); // set page to 0
 }

 /*
 Fills screen with either white or black pixels
 */
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

 /*
 Writes a single pixel to the LCD. 
 Note that this will also set all pixels in the corresponding byte
 */
 void LCD_WritePixel (int x, int y, bool light) {
	unsigned char fill = (light) ? 0xFF : 0x00; // default is dark pixel. These bits are all flipped later.
	
	if (x >= 0 && x < 128 && y >= 0 && y/8 < 8) { // check that coordinates are in bound, and that character exists
		LCD_SetCommandMode();

		LCD_SetColumnStart(x + 0x2); // RAM column offset of 2 since 1106 has buffer of size 132 centered on screen of width 128
		LCD_SetPageStart(y/8); // set page address Text is aligned with RAM pages

		LCD_SetDisplayMode();
		SPI_MasterTransmit((~fill) ^ ((0x1) << (y % 8))); // set one bit only
	}
 }

 /*
 Write a horizontal or vertical line.
 Note that in both cases, any accessed bytes will have all other bits turned off
 */
 void LCD_WriteLine (int x, int y, int length, bool horizontal, bool light)
 {
	unsigned char fill = (light) ? 0xFF : 0x00; // default is dark pixel. These bits are all flipped later.

	LCD_SetCommandMode();

	if (horizontal) {
		LCD_SetColumnStart(x + 0x02);

		LCD_SetPageStart(y/8); // set page address
		LCD_SetDisplayMode();
		for (int i = 0; i < length; i++)
			SPI_MasterTransmit((~fill) ^ ((0x1) << (y % 8))); // set appropriate bit
	}
	else {
			uint16_t startMask = (1 << y % 8) - 1;
			uint16_t endMask = (1 << (y + length) % 8) - 1;

			LCD_SetColumnStart(0x2 + x);
			LCD_SetPageStart(y/8); // RAM has 8 pages
			LCD_SetDisplayMode();
			SPI_MasterTransmit(startMask ^ (fill)); // set appropriate bits for first page

			for (int i = 0; i < (length - (8 - (y % 8)))/8; i++) { // limited to number of full pages
				LCD_SetCommandMode();
				LCD_SetColumnStart(0x2 + x);
				LCD_SetPageStart(y/8 + i + 1); // RAM has 8 pages
				LCD_SetDisplayMode();
				SPI_MasterTransmit(fill);
			}

			LCD_SetCommandMode();
			LCD_SetColumnStart(0x2 + x);
			LCD_SetPageStart((y + length)/8);
			LCD_SetDisplayMode();

			if ((y % 8) + length == 8) // exactly than 1 page
				return;
			else if ((y % 8) + length > 8) // longer than 1 page
				SPI_MasterTransmit(endMask ^ (~fill));
			else // shorter than one page
				SPI_MasterTransmit(endMask ^ startMask ^ (~fill));
		return;
	}
 }

 /*
 Writes a single character to the specified location. Only ASCII characters supported
 */
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
 Write a string to the display at the desired location. Uses C Strings
 */
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