/*
 * KPD_int.h
 *
 *  Created on: Jul 22, 2025
 *      Author: pc
 */

#ifndef HAL_KPD_KPD_INT_H_
#define HAL_KPD_KPD_INT_H_

#define KPD_COL_PORT 	DIO_PORTB
#define KPD_ROW_PORT 	DIO_PORTB

#define KPD_ROWS	4
#define KPD_COLS	3

#define COL_INIT    0
#define COL_END     3
#define ROW_INIT    4
#define ROW_END     8

#define NO_KEY		0xFF

/* ----- Key mapping ----- */
/*
Write your keyMap in main like this , you can change size

u8 KeyMap[KPD_ROWS][KPD_COLS] =
{
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
*/
void HKPD_vInit(void);
u8 HKPD_u8GetPressedKey(const u8 KeyMap[KPD_ROWS][KPD_COLS]);

#endif /* HAL_KPD_KPD_INT_H_ */
