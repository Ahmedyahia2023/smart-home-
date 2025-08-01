/*
 * ADC_prg.c
 *
 *  Created on: Jul 24, 2025
 *      Author: Nada Mamdouh
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "ADC_int.h"
#include "ADC_prv.h"


void MADC_vInit(void)
{
	/* Reference voltage (AVCC)*/
	CLR_BIT(ADMUX, REFS1);
	SET_BIT(ADMUX, 6);

	/* Data adjustment (right) */
	CLR_BIT(ADMUX, 5);

	/* Select prescaler 64 */
//	ADCSRA = (ADCSRA & (0xF8)) | ((0x07)&(prescaller));
	SET_BIT(ADCSRA,2);
	SET_BIT(ADCSRA,1);
	CLR_BIT(ADCSRA,0);

	/* Auto trigger or manual */
	CLR_BIT(ADCSRA, 5);

	/* Interrupt enable/disable */
	CLR_BIT(ADCSRA, 3);

	/* Enable ADC */
	SET_BIT(ADCSRA, 7);

}

u16  MADC_u16AnalogRead(u8 A_u8Channel)
{
	/* Select channel in Single Ended Input mode */
	ADMUX = (ADMUX & (0xE0)) | (A_u8Channel & 0x07);

	/* Start conversion */
	SET_BIT(ADCSRA, 6);

	/* Wait for flag to be raised */
	while(GET_BIT(ADCSRA, 4) == 0){}

	/* Clear flag */
	SET_BIT(ADCSRA, 4);

	return ADC;
}
