/*
 * EXTI_int.h
 *
 *  Created on: Jul 23, 2025
 *      Author: Nada Mamdouh
 */

#ifndef MCAL_EXTI_EXTI_INT_H_
#define MCAL_EXTI_EXTI_INT_H_


#define ENABLE			1
#define DISABLE			0

#define EXTI_INT0         ENABLE
#define EXTI_INT1         DISABLE
#define EXTI_INT2         DISABLE

#define EXTI_FALLING	0
#define EXTI_RISING		1
#define EXTI_LOW_LVL	2
#define EXTI_ONCHANGE	3
/*
 * EXTI_INT0_SENSE_CTRL , EXTI_INT1_SENSE_CTRL
 * Options:
 * EXTI_FALLING ,
 * EXTI_RISING ,
 * EXTI_LOW_LVL ,
 * EXTI_ONCHANGE
 *       ----------------------------
 * EXTI_INT2_SENSE_CTRL
 * Options:
 * EXTI_FALLING ,
 * EXTI_RISING
 *
 **/
#define EXTI_INT0_SENSE_CTRL  EXTI_RISING
#define EXTI_INT1_SENSE_CTRL  EXTI_FALLING
#define EXTI_INT2_SENSE_CTRL  EXTI_FALLING

void MEXTI_vInit(void);
void MEXTI_vCallBackFunction(void (*Fptr)(void), u8 A_u8InterruptNo);


#endif /* MCAL_EXTI_EXTI_INT_H_ */
