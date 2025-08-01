/*
 * USART_prg.c
 *
 *  Created on: Jul 30, 2025
 *      Author: pc
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "USART_int.h"
#include "USART_prv.h"


void MUSART_vInit(void)
{
	// enable receiver & transmitter
	UCSRB = 0b00011000;
	// enable UCSRC reg | asynch | no parity | 1 stop bit | 8 bit data | no clk polarity
	UCSRC = 0b10000110;
	UBRRL = 51;
	UBRRH = 0;


}
void MUSART_vTransmit(u8 A_u8Byte)
{
	/* Wait for empty transmit buffer */
	while(!GET_BIT(UCSRA, UDRE));
//	while ( !( UCSRA & (1<<UDRE)) )
//	;
	/* Put data into buffer, sends the data */
	UDR = A_u8Byte;
}

u8 MUSART_vReceive(void)
{
	/* Wait for data to be received */
	while(!GET_BIT(UCSRA, RXC));
//	while ( !(UCSRA & (1<<RXC)) )
//	;
	/* Get and return received data from buffer */
	return UDR;
}
