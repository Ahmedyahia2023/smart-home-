/*
 * SPI_prv.h
 *
 *  Created on: Jul 31, 2025
 *      Author: pc
 */

#ifndef MCAL_SPI_SPI_PRV_H_
#define MCAL_SPI_SPI_PRV_H_

#define SPCR		*((volatile u8*)(0x2D))
#define SPSR		*((volatile u8*)(0x2E))
#define SPDR		*((volatile u8*)(0x2F))

/* --- SPI Configuration --- */
#define SPI_MASTER	0
#define SPI_SLAVE	1

/* SPI SPSF Register */
#define SPIF	7

#endif /* MCAL_SPI_SPI_PRV_H_ */
