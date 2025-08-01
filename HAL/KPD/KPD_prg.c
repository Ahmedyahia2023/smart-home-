/*
 * KPD_prg.c
 *
 *  Created on: Jul 22, 2025
 *      Author: pc
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include <util/delay.h>
#include "../../MCAL/DIO/DIO_int.h"
#include "KPD_int.h"


void HKPD_vInit(void)
{
	/* Configure columns as output */
	for(u8 L_u8ColIndex = COL_INIT; L_u8ColIndex < COL_END; L_u8ColIndex++)
	{
		MDIO_vSetPinDir(KPD_COL_PORT, L_u8ColIndex, DIO_OUTPUT);
	}

	/* Configure rows as input */
	for(u8 L_u8RowIndex = ROW_INIT; L_u8RowIndex < ROW_END; L_u8RowIndex++)
	{
		MDIO_vSetPinDir(KPD_ROW_PORT, L_u8RowIndex, DIO_INPUT);
	}

	/* Set columns val to high */
	for(u8 L_u8ColIndex = COL_INIT; L_u8ColIndex < COL_END; L_u8ColIndex++)
	{
		MDIO_vSetPinVal(KPD_COL_PORT, L_u8ColIndex, DIO_HIGH);
	}

	/* Activate pull-up resistor on input pins */
	for(u8 L_u8RowIndex = ROW_INIT; L_u8RowIndex < ROW_END; L_u8RowIndex++)
	{
		MDIO_vSetPinVal(KPD_ROW_PORT, L_u8RowIndex, DIO_HIGH);
	}
}

u8 HKPD_u8GetPressedKey(const u8 KeyMap[KPD_ROWS][KPD_COLS])
{
	u8 L_u8PressedKey = NO_KEY;

	for(u8 col = COL_INIT; col < COL_END; col++)
	{
		/* Activate current Column */
		MDIO_vSetPinVal(KPD_COL_PORT, col, DIO_LOW);

		/* Check all rows */
		for(u8 row = ROW_INIT; row < ROW_END; row++)
		{
			/* if switch is pressed */
			if(MDIO_u8GETPinVal(KPD_ROW_PORT, row) == 0)
			{
				/* if switch is still pressed (wait for release) */
				while(MDIO_u8GETPinVal(KPD_ROW_PORT, row) == 0);

				/* Debouncing */
				_delay_ms(10);

				/* Action */
				L_u8PressedKey = KeyMap[row-ROW_INIT][col];

			}
		}
		/* Deactivate current Column */
		MDIO_vSetPinVal(KPD_COL_PORT, col, DIO_HIGH);
	}
	return L_u8PressedKey;

}
