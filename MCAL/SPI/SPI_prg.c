/*
 * SPI_prg.c
 *
 *  Created on: Jul 31, 2025
 *      Author: pc
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../DIO/DIO_int.h"

#include "SPI_int.h"
#include "SPI_prv.h"
#include "SPI_cfg.h"

void MSPI_vInit(void)
{
#if SPI_MODE == SPI_MASTER
	MDIO_vSetPinDir(DIO_PORTB, DIO_PIN5, DIO_OUTPUT);          // MOSI
	MDIO_vSetPinDir(DIO_PORTB, DIO_PIN7, DIO_OUTPUT);          // sck
	MDIO_vSetPinDir(DIO_PORTB, DIO_PIN6, DIO_INPUT);          // MISO
	MDIO_vSetPinVal(DIO_PORTB, DIO_PIN6, DIO_OUTPUT);			// pullup resistor

#endif
#if SPI_MODE == SPI_SLAVE
	MDIO_vSetPinDir(DIO_PORTB, DIO_PIN5, DIO_INPUT);          // MOSI
	MDIO_vSetPinDir(DIO_PORTB, DIO_PIN7, DIO_INPUT);          // sck
	MDIO_vSetPinDir(DIO_PORTB, DIO_PIN6, DIO_OUTPUT);          // MISO
#endif


	// SPI enable | Master | pre_64
	SPCR = 0b01011010;
}
u8 MSPI_vTransceive(u8 A_u8Byte)
{
	/* Start transmission */
	SPDR = A_u8Byte;

	/* Wait for transmission complete */
	while(!GET_BIT(SPSR, SPIF))
		;

	/* Return data register */
	return SPDR;

}

void MSPI_vTransmit(u8 A_u8Byte)
{
	/* Start transmission */
	SPDR = A_u8Byte;

	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
		;
}
u8 MSPI_u8Receive(void)
{
	/* Wait for reception complete */
	while(!GET_BIT(SPSR, SPIF))
		;
	/* Return data register */
	return SPDR;
}

