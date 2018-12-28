/*
 * SPI_Master.h
 *
 * Created: 12/27/2018 7:24:50 PM
 *  Author: Andrew Ge
 */ 


#ifndef SPI_MASTER_H_
#define SPI_MASTER_H_

// PORTB
#define MOSI 3
#define SCK 5

void SPI_InitMaster (void);
void SPI_SetMode3 (void);
void SPI_MasterTransmit (uint8_t data);

#endif /* SPI_MASTER_H_ */