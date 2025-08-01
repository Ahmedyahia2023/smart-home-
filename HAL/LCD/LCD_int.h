/*
 * LCD_int.h
 *
 *  Created on: Jul 19, 2025
 *      Author: pc
 */

#ifndef HAL_LCD_LCD_INT_H_
#define HAL_LCD_LCD_INT_H_


#define LCD_CTRL_PORT 	DIO_PORTA
#define LCD_DATA_PORT 	DIO_PORTD
#define LCD_RS			DIO_PIN0
#define LCD_RW			DIO_PIN1
#define LCD_E			DIO_PIN2

#define DDRAM_BASE_ADDR_R0  0x80
#define DDRAM_BASE_ADDR_R1  0xC0


void HLCD_vInit(void);
void HLCD_vSendCmd(u8 A_u8cmd);
void HLCD_vSendChar(u8 A_u8Char);
void HLCD_vDisplayString(s8 A_s8Str[]);
void HLCD_vPrintNumber(s32 A_s32Number);
void HLCD_vSetCursorPosition(u8 A_u8Row, u8 A_u8Col);
void HLCD_vClearScreen(void);

#endif /* HAL_LCD_LCD_INT_H_ */
