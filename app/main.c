#define F_CPU 8000000UL
#include <util/delay.h>

#include "../LIB/BIT_MATH.h"
#include "../LIB/STD_TYPES.h"

#include "../MCAL/ADC/ADC_int.h"
#include "../MCAL/DIO/DIO_int.h"
#include "../MCAL/EXTI/EXTI_int.h"
#include "../MCAL/GIE/GIE_int.h"
#include "../MCAL/USART/USART_int.h"
#include "../MCAL/TIMERS/TIMERS_cfg.h"
#include "../MCAL/TIMERS/TIMERS_int.h"

/////////////////////////////////////////////////
#define LDRPIN     0   // ADC Channel 0
#define LM35PIN    1   // ADC Channel 1
#define MOTORPIN1  2   // PA2
#define MOTORPIN2  3   // PA3
#define TEMP_LED   4   // PA4

/////////////////////////////////////////////////
u8 motorState = 0;
u8 overheatFlag = 0;

u16 LM35_reading = 0;
u16 LDR_reading = 0;
u8 temp = 0;

/////////////////////////////////////////////////
void LDR_func(void);
void LM35_FUNC(void);
void MotorUp_Func(void);
void MotorDown_Func(void);
void motorstop(void);
void ALARM_FUNC(void);

/////////////////////////////////////////////////
void main(void)
{
    MGIE_vEnableGlobalInterrupt();
    MEXTI_vInit();
    MTIMERS_vInit();
    MADC_vInit();
    MUSART_vInit();

    // Input Pins
    MDIO_vSetPinDir(DIO_PORTA, LM35PIN, DIO_INPUT);
    MDIO_vSetPinDir(DIO_PORTA, LDRPIN, DIO_INPUT);

    // Motor Control Output Pins
    MDIO_vSetPinDir(DIO_PORTA, MOTORPIN1, DIO_OUTPUT);
    MDIO_vSetPinDir(DIO_PORTA, MOTORPIN2, DIO_OUTPUT);

    // LED or test pin output
    MDIO_vSetPinDir(DIO_PORTA, TEMP_LED, DIO_OUTPUT);

    // Set temperature alarm check every X ticks (adjust internally)
    MTIMERS_vSetIntervalAsych_CB(ALARM_FUNC, 2000);
    MTIMERS_vStartTimer(TIM_0);

    while (1)
    {
        // Skip all logic if overheat occurred
        if (overheatFlag)
        {
            motorstop();
            continue;
        }

        // You may uncomment this if you want LDR-based control too
        // LDR_func();

        _delay_ms(500);  // Short delay to allow USART output readability
    }
}

/////////////////////////////////////////////////
void LDR_func(void)
{
    LDR_reading = MADC_u16AnalogRead(LDRPIN);

    if (LDR_reading >= 800 && motorState == 0)
    {
        MotorUp_Func();
        motorstop();
        motorState = 1;
    }
    else if (LDR_reading < 700 && motorState == 1)
    {
        MotorDown_Func();
        motorstop();
        motorState = 0;
    }
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
void MotorUp_Func(void)
{
    MDIO_vSetPinVal(DIO_PORTA, MOTORPIN1, DIO_HIGH);
    MDIO_vSetPinVal(DIO_PORTA, MOTORPIN2, DIO_LOW);
}

void MotorDown_Func(void)
{
    MDIO_vSetPinVal(DIO_PORTA, MOTORPIN1, DIO_LOW);
    MDIO_vSetPinVal(DIO_PORTA, MOTORPIN2, DIO_HIGH);
}

void motorstop(void)
{
    MDIO_vSetPinVal(DIO_PORTA, MOTORPIN1, DIO_LOW);
    MDIO_vSetPinVal(DIO_PORTA, MOTORPIN2, DIO_LOW);
}

/////////////////////////////////////////////////
void ALARM_FUNC(void)
{	static u8 tempState=0;
    u32 reading = MADC_u16AnalogRead(CHANNEL_1);
    u16 millivolts = (reading * 5000UL) / 1024;
    u16 local_temp = millivolts / 10;
    // USART Test Output
       MUSART_vTransmit((local_temp / 10) + '0');
       MUSART_vTransmit((local_temp % 10) + '0');
       MUSART_vTransmit('\n');

      if (local_temp >= 30 && local_temp < 60 && tempState == 0)
       {
           MotorUp_Func();
           tempState = 1;
       }
       else if (local_temp < 30)
       {
           tempState = 0;
           motorstop();
       }
    if (local_temp > 60)
    {
        overheatFlag = 1;
        MDIO_vSetPinVal(DIO_PORTA, TEMP_LED, DIO_HIGH);  // Overheat LED ON
        motorstop();  // Stop motor immediately
    }
    else
        MDIO_vSetPinVal(DIO_PORTA, TEMP_LED, DIO_LOW);  // Overheat LED ON


}
