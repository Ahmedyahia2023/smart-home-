/*
 * WDT_prg.c
 *
 *  Created on: Jul 30, 2025
 *      Author: pc
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "WDT_int.h"
#include "WDT_prv.h"

void MWDT_vEnable(u8 A_u8TimeOut)
{
	/* Set timeout*/
	WDTCR = (WDTCR & 0xF8) | (A_u8TimeOut & 0x07);

	/* Enable */
	SET_BIT(WDTCR, 3);

}
void MWDT_vDisable(void)
{
	WDTCR = 0b00011000;
	WDTCR = 0;
}

