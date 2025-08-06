/*
 * TWI_prg.c
 *
 *  Created on: Aug 3, 2025
 *      Author: Nada Mamdouh
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "TWI_int.h"
#include "TWI_prv.h"
#include "TWI_cfg.h"

void MTWI_vInit(void)
{
	/* Enable/Disable Interrupt (disable) */
	CLR_BIT(TWCR, TWEN);

	/*Set bit rate speed*/
	TWBR = 10;

}
u8 MTWI_u8SendStartCondition(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)| (1<<TWEN);

	while (!(TWCR & (1<<TWINT)))
		;

	/* Status code */
	return (TWSR & 0xF8);

}
u8 MTWI_u8ResendStartCondition(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)| (1<<TWEN);

	while (!(TWCR & (1<<TWINT)))
		;

	/* Status code */
	return (TWSR & 0xF8);
}
u8 MTWI_u8SendSLA_RW(u8 A_u8SLA, u8 A_u8RW)
{
	TWDR = (A_u8SLA<<1) | A_u8RW;

	TWCR = (1<<TWINT) | (1<<TWEN);

	/* Clear start condition bit (must be cleared by SW)*/
	CLR_BIT(TWCR, TWSTA);

	while (!(TWCR & (1<<TWINT)))
		;

	/* Status code */
	return (TWSR & 0xF8);
}
u8 MTWI_u8SendByte(u8 A_u8Byte)
{
	TWDR = A_u8Byte;
	TWCR = (1<<TWINT) | (1<<TWEN);

	while (!(TWCR & (1<<TWINT)))
		;

	/* Status code */
	return (TWSR & 0xF8);
}
u8 MTWI_u8ReceiveByte(u8* PtrByte, u8 A_u8Ack_NAck)
{
	switch(A_u8Ack_NAck)
	{
	case TWI_ACK:
		TWCR = (1<<TWINT) | (1<<TWEN) ;
		SET_BIT(TWCR, TWEA);
		break;
	case TWI_NOT_ACK:
		TWCR = (1<<TWINT) | (1<<TWEN) ;
		CLR_BIT(TWCR, TWEA);
		break;
	}


	while (!(TWCR & (1<<TWINT)))
		;

	*PtrByte = TWDR;
	/* Status code */
	return (TWSR & 0xF8);
}
u8 MTWI_u8SentStopCondition(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

	/* Status code */
	return (TWSR & 0xF8);
}
u8 MTWI_u8GetStatusCode(void)
{
	/* Status code */
	return (TWSR & 0xF8);
}
