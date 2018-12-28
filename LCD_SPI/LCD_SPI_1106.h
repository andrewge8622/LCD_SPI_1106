/*
 * LCD_SPI_1106.h
 *
 * Created: 12/27/2018 7:19:58 PM
 *  Author: Andrew Ge
 *
 * Currently written for use with an Arduino Nano board
 */ 


#ifndef LCD_SPI_1106_H_
#define LCD_SPI_1106_H_

// PORTB
#define LCD_RESET 0
#define LCD_DC 1
#define LCD_CS 2

void LCD_InitPins(void);
void LCD_Init(void);
void LCD_WriteChar(int x, int y, char c);


#endif /* LCD_SPI_1106_H_ */