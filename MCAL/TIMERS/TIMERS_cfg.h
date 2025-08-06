/*
 * TIMERS_cfg.h
 *
 *  Created on: Jul 31, 2025
 *      Author: pc
 */

#ifndef MCAL_TIMERS_TIMERS_CFG_H_
#define MCAL_TIMERS_TIMERS_CFG_H_

/* Enable and Disable Timers*/
/*Options:
 * ENABLE
 * DISABLE
 * */
#define TIMERID_0  	ENABLE
#define TIMERID_1  	ENABLE
#define TIMERID_2  	DISABLE

/* Configuire Prescaller */
/*Options:
 *  NO_CLK
	CLK_NO_PRESCALLING
 	CLK_8
 	CLK_64
    CLK_256
    CLK_1024
    EXTERNAL_T0_FALLING
    EXTERNAL_T0_RISING
 * */
#define CLK_SELECT_PRESCALER_TIM0	CLK_8
#define CLK_SELECT_PRESCALER_TIM1	CLK_8

/* SELECT MODE */
/*Options:
 * NORMAL_OVERFLOW
   PWM_PHASE_CORRECT
   CTC
   FAST_PWM
 * */
#define TIMERID_0_MODE	FAST_PWM


/*Interrupt enable/ disable */
#define INTERRUPR_CONTROL	ENABLE

#endif /* MCAL_TIMERS_TIMERS_CFG_H_ */
