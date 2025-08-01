/*
 * SPI_int.h
 *
 *  Created on: Jul 31, 2025
 *      Author: pc
 */

#ifndef MCAL_SPI_SPI_INT_H_
#define MCAL_SPI_SPI_INT_H_


void MSPI_vInit(void);
u8 MSPI_vTransceive(u8 A_u8Byte);

void MSPI_vTransmit(u8 A_u8Byte);
u8 MSPI_u8Receive(void);

#endif /* MCAL_SPI_SPI_INT_H_ */
