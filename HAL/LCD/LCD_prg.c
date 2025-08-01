/*
 * LCD_prg.c
 *
 *  Created on: Jul 19, 2025
 *      Author: pc
 */
#include <util/delay.h>
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/DIO/DIO_int.h"
#include "LCD_int.h"

void HLCD_vInit(void)
{
	/* Initialize pins direction */
	MDIO_vSetPinDir(LCD_CTRL_PORT, LCD_RS, DIO_OUTPUT);
	MDIO_vSetPinDir(LCD_CTRL_PORT, LCD_RW, DIO_OUTPUT);
	MDIO_vSetPinDir(LCD_CTRL_PORT, LCD_E, DIO_OUTPUT);
	MDIO_vSetPortDir(LCD_DATA_PORT, DIO_PORT_OUTPUT);

	/* 8-bit initialization sequence*/
	_delay_ms(50);
	HLCD_vSendCmd(0x38);  	// send function set instruction
	_delay_ms(1);
	HLCD_vSendCmd(0x0C);	// send display on/off instruction
	_delay_ms(1);
	HLCD_vSendCmd(0x01);	// send clear instruction
	_delay_ms(2);
	HLCD_vSendCmd(0x06);	// send entry mode set instruction

}
void HLCD_vSendCmd(u8 A_u8cmd)
{
	/* Control RS to Send Command */
	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_RS, DIO_LOW);

	/* Reset RW to Enable Writing */
	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_RW, DIO_LOW);

	/* Write instruction */
	MDIO_vSetPortVal(LCD_DATA_PORT, A_u8cmd);

	/* Pulse on Enable */
	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_E, DIO_HIGH);
	_delay_ms(1);
	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_E, DIO_LOW);
	_delay_ms(1);

}
void HLCD_vSendChar(u8 A_u8Char)
{
	/* Control RS to Send Data */
	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_RS, DIO_HIGH);

	/* Reset RW to Enable Writing */
	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_RW, DIO_LOW);

	/* Write instruction */
	MDIO_vSetPortVal(LCD_DATA_PORT, A_u8Char);

	/* Pulse on Enable */
	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_E, DIO_HIGH);
	_delay_ms(1);
	MDIO_vSetPinVal(LCD_CTRL_PORT, LCD_E, DIO_LOW);
	_delay_ms(1);
}

void HLCD_vDisplayString(s8 A_s8Str[])
{
	for(u8 L_u8index=0 ;A_s8Str[L_u8index] != '\0'; L_u8index++)
	{
		HLCD_vSendChar(A_s8Str[L_u8index]);
	}
}
void HLCD_vPrintNumber(s32 A_s32Number)
{
	u8 L_u8index = 0;
	u8 L_u8digits[10] = {0};

	/* Extract digits */
	while(A_s32Number > 0)
	{
		L_u8digits[L_u8index] = A_s32Number%10 + '0';
		A_s32Number/=10;
		L_u8index++;
	}
	for(s8 L_s8index = 9; L_s8index >= 0; L_s8index--)
	{
		if(L_u8digits[L_s8index] == 0) continue;
		HLCD_vSendChar(L_u8digits[L_s8index]);
	}

}
void HLCD_vSetCursorPosition(u8 A_u8Row, u8 A_u8Col)
{
	switch(A_u8Row){
	case 0:
		// DDRAM base address + col number
		HLCD_vSendCmd(DDRAM_BASE_ADDR_R0 + A_u8Col);
		break;
	case 1:
		HLCD_vSendCmd(DDRAM_BASE_ADDR_R1 + A_u8Col);
		break;
	}
}

void HLCD_vClearScreen(void) {
	HLCD_vSendCmd(0x01);
}
