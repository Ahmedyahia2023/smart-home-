/*
 * ADC_prv.h
 *
 *  Created on: Jul 24, 2025
 *      Author: Nada Mamdouh
 */

#ifndef MCAL_ADC_ADC_PRV_H_
#define MCAL_ADC_ADC_PRV_H_

#define SFIOR	*((volatile u8*)(0x50))

#define ADMUX	*((volatile u8*)(0x27))
#define ADCSRA	*((volatile u8*)(0x26))
#define ADCH	*((volatile u8*)(0x25))
#define ADCL	*((volatile u8*)(0x24))

#define ADC		*((volatile u16*)(0x24))


#define REFS1	7

#endif /* MCAL_ADC_ADC_PRV_H_ */
