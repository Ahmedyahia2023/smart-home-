/*
 * TIMERS_prg.c
 *
 *  Created on: Jul 27, 2025
 *      Author: Nada Mamdouh
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "TIMERS_int.h"
#include "TIMERS_prv.h"
#include "TIMERS_cfg.h"

static void (*G_TIMER_OVF_CB)(void) = NULL;
static void(*G_TIMER_CTC_CB)(void) = NULL;
static void(*G_TIMER1_ICU_CB)(void) = NULL;

static u32 G_u32T_required=0;

void MTIMERS_vInit(void)
{
	//	/* Waveform generation (normal OF mode)*/
	//	/* Stop timer */
	//	TCCR0 = 0x00;
	//
	//	/*Enable interrupt OF mode */
	//	SET_BIT(TIMSK,0);

	//	TCCR0 = 0b00001000;
	//	/*Enable interrupt CTC mode */
	//	SET_BIT(TIMSK,1);

	// Fast PWM mode/ non-inverting /clk_8
	//	TCCR0 = 0b01101010;
#if TIMERID_0 == ENABLE

#if TIMERID_0_MODE == NORMAL_OVERFLOW
	CLR_BIT(TCCR0, WGM01);
	CLR_BIT(TCCR0, WGM00);
#elif TIMERID_0_MODE == PWM_PHASE_CORRECT
	CLR_BIT(TCCR0, WGM01);
	SET_BIT(TCCR0, WGM00);

#elif TIMERID_0_MODE == CTC
	SET_BIT(TCCR0, WGM01);
	CLR_BIT(TCCR0, WGM00);

#elif TIMERID_0_MODE == FAST_PWM
	SET_BIT(TCCR0, WGM01);
	SET_BIT(TCCR0, WGM00);

#else
#error "Incorrect mode"

#endif

#if INTERRUPR_CONTROL == ENABLE
	MTIMERS_vEnableInterrupt(TIM_0, TIMERID_0_MODE);
#elif INTERRUPR_CONTROL == DISABLE
	MTIMERS_vDisableInterrupt(TIM_0, TIMERID_0_MODE);
#else
#error "Incorrect value"
#endif

	// Stop bit
	CLR_BIT(TCCR0,CS02);
	CLR_BIT(TCCR0,CS01);
	CLR_BIT(TCCR0,CS00);

#endif

#if TIMERID_1 == ENABLE
	//	/* Fast PWM mode */
	//	// non inverting mode
	//	SET_BIT(TCCR1A, 7);
	//	CLR_BIT(TCCR1A, 6);
	//	// waveform
	//	SET_BIT(TCCR1A,1);
	//	CLR_BIT(TCCR1A,0);
	//	SET_BIT(TCCR1B,3);
	//	SET_BIT(TCCR1B,4);
	//	// prescaler (64)
	//	SET_BIT(TCCR1B, 0);
	//	SET_BIT(TCCR1B, 1);
	//	CLR_BIT(TCCR1B, 2);
	//	// value ICR1
	//	ICR1 = 2499;

	TCCR1A = 0b00000000;
	TCCR1B = 0b01000010;



#endif

#if TIMERID_2 == ENABLE

#endif
}
void MTIMERS_vSetIntervalAsych_CB(void (*Fptr)(void), u32 A_u32T_required)
{
	G_u32T_required = A_u32T_required;
	G_TIMER_OVF_CB = Fptr;
//	MTIMERS_vStartTimer();
}
void MTIMERS_vSetInterval_CTC(void (*Fptr)(void),u32 A_u32T_required, u8 A_u8OCR_val)
{
	G_TIMER_CTC_CB = Fptr;
	G_u32T_required = A_u32T_required;
	OCR0 = A_u8OCR_val;
//	MTIMERS_vStartTimer();
}
void MTIMERS_vStartTimer(u8 A_u8TimerID)
{
	if(A_u8TimerID == TIM_0)
	{
		TCCR0 = (TCCR0 & 0xF8) | (0x07 & CLK_SELECT_PRESCALER_TIM0);
	}
	if(A_u8TimerID == TIM_1)
	{
		TCCR1B = (TCCR1B & 0xF8) | (0x07 & CLK_SELECT_PRESCALER_TIM1);
	}



}
void MTIMERS_vStopTimer(u8 A_u8TimerID)
{
	if(A_u8TimerID == TIM_0)
	{
		TCCR0 = (TCCR0 & 0xF8) | (0x07 & 0x00);
	}
	if(A_u8TimerID == TIM_1)
	{
		TCCR1B = (TCCR1B & 0xF8) | (0x07 & 0x00);
	}

}

void MTIMERS_vSetCompareMatch(u8 A_u8TimerID, u16 A_16OCR_val)
{
	switch(A_u8TimerID)
	{
	case TIM_0:
		OCR0 = A_16OCR_val;
		break;
	case TIM_1_A:
		OCR1A = A_16OCR_val;
		break;
	}

}

void MTIMERS_vSetICU_CB(void (*Fptr)(void))
{
	G_TIMER1_ICU_CB = Fptr;
}

u16 MTIMERS_u16GetCapturedValue(void)
{
	return ICR1;
}
void MTIMERS_SetTrigger(u8 A_Trigger_Type)
{
	switch (A_Trigger_Type)
	{
	case TRIG_TYPE_RISING:
		SET_BIT(TCCR1B, 6);
		break;
	case TRIG_TYPE_FALLING:
		CLR_BIT(TCCR1B, 6);
		break;
	}
}
void MTIMERS_vEnableInterrupt(u8 A_u8TimerID, u8 A_u8TimerMode)
{
	if(A_u8TimerID == TIM_0)
	{
		switch(A_u8TimerMode)
		{
		case NORMAL_OVERFLOW:
			SET_BIT(TIMSK, 0);
			break;
		case CTC:
			SET_BIT(TIMSK, 1);
			break;
		}
	}
	if(A_u8TimerID == TIM_1)
	{
		switch(A_u8TimerMode)
		{
		case ICU_MODE:
			SET_BIT(TIMSK, 5);
			break;
		}
	}
}
void MTIMERS_vDisableInterrupt(u8 A_u8TimerID, u8 A_u8TimerMode)
{
	if(A_u8TimerID == TIM_0)
	{
		switch(A_u8TimerMode)
		{
		case NORMAL_OVERFLOW:
			CLR_BIT(TIMSK, 0);
			break;
		case CTC:
			CLR_BIT(TIMSK, 1);
			break;
		}
	}
	if(A_u8TimerID == TIM_1)
	{
		switch(A_u8TimerMode)
		{
		case ICU_MODE:
			CLR_BIT(TIMSK, 5);
			break;
		}
	}
}



void __vector_11(void) __attribute__((signal));
void __vector_11(void)
{
	static u32 LS_u32T_OVF = 0;
	LS_u32T_OVF++;
	if(LS_u32T_OVF == G_u32T_required)
	{
		if(G_TIMER_OVF_CB != NULL)
		{
			G_TIMER_OVF_CB();
			LS_u32T_OVF = 0;
		}
	}

}

void __vector_10(void) __attribute__((signal));
void __vector_10(void)
{
	static u32 LS_u32Counter = 0;
	LS_u32Counter++;
	if(LS_u32Counter == G_u32T_required)
	{
		if(G_TIMER_CTC_CB != NULL)
		{
			G_TIMER_CTC_CB();
			LS_u32Counter = 0;
		}
	}
}

void __vector_6(void) __attribute__((signal));
void __vector_6(void)
{
	if(	G_TIMER1_ICU_CB != NULL)
	{
		G_TIMER1_ICU_CB();
	}
}
