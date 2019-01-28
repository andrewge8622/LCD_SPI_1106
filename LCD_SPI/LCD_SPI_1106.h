/*
 * LCD_SPI_1106.h
 *
 * Created: 12/27/2018 7:19:58 PM
 * Author: Andrew Ge
 */ 


#ifndef LCD_SPI_1106_H_
#define LCD_SPI_1106_H_

// PORTB
#define LCD_RESET 0
#define LCD_DC 1
#define LCD_CS 2

void LCD_InitPins(void);
void LCD_Init(void);
void LCD_SetupRAM(void);
void LCD_ClearScreen(bool on = true);
void LCD_WritePixel(int x, int y, bool light = false);
void LCD_WriteLine(int x, int y, int length, bool horizontal = true, bool light = false);
void LCD_WriteChar(int x, int y, char c);
void LCD_WriteWord(int x, int y, int length, char* word, bool align = true);



#endif /* LCD_SPI_1106_H_ */