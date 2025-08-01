/*
 * TIMERS_int.h
 *
 *  Created on: Jul 27, 2025
 *      Author: Nada Mamdouh
 */

#ifndef MCAL_TIMERS_TIMERS_INT_H_
#define MCAL_TIMERS_TIMERS_INT_H_


#define TIM_0	 0
#define TIM_1_A	 1
#define TIM_1	 2

#define TRIG_TYPE_RISING	0
#define TRIG_TYPE_FALLING	1


#define ICU_MODE	0

void MTIMERS_vInit(void);
void MTIMERS_vSetIntervalAsych_CB(void (*Fptr)(void), u32 A_u32T_required);

// CTC
void MTIMERS_vSetInterval_CTC(void (*Fptr)(void),u32 A_u32T_required, u8 A_u8OCR_val);
void MTIMERS_vSetCompareMatch(u8 A_u8TimerID, u16 A_16OCR_val);

void MTIMERS_vSetICU_CB(void (*Fptr)(void));
u16 MTIMERS_u16GetCapturedValue(void);
void MTIMERS_SetTrigger(u8 A_Trigger_Type);
void MTIMERS_vEnableInterrupt(u8 A_u8TimerID, u8 A_u8TimerMode);
void MTIMERS_vDisableInterrupt(u8 A_u8TimerID, u8 A_u8TimerMode);


void MTIMERS_vStartTimer(u8 A_u8TimerID);
void MTIMERS_vStopTimer(u8 A_u8TimerID);

#endif /* MCAL_TIMERS_TIMERS_INT_H_ */
