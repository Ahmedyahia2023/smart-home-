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
#include "../HAL/KPD/KPD_int.h"
#include "../HAL/LCD/LCD_int.h"

#define LDRPIN     0
#define LM35PIN    1
#define WINDOW_LED  2
#define TEMP_LED   4
#define fanPIn1    6
#define SERVOPIN   5
#define BuzzerPin  7

u8 motorState = 0;
u8 y = 0;
u8 flage = 0;

u8 fanMode = 0;         // 0: Auto, 1: Manual
u8 fanControlFlag = 0;  // Fan control in manual mode

u8 doorMode = 0;        // 0: Auto, 1: Manual
u8 doorControlFlag = 0; // Door control in manual mode

u8 windMode = 0;        // 0: Auto, 1: Manual
u8 windControlFlag = 0;

u8 KeyPap[KPD_ROWS][KPD_COLS] = {
    {'7', '8', '9'},
    {'4', '5', '6'},
    {'1', '2', '3'},
    {'*', '0', '#'}
};
u8 key_enter[5];

void LDR_func(void);
void MotorUp_Func(u8 port, u8 pin1);
void motorstop(u8 port, u8 pin1);
void LM35_func(void);
void Welcome_Screen(void);
void ServoOn_func(u8 angle);
void ServoOff_func(void);
void BuzzerOn_Func(void);
void BuzzerOff_Func(void);
void LCD_STATE(void);
void alarm_func(void);
void main(void)
{

    MDIO_vSetPinVal(DIO_PORTA, WINDOW_LED, DIO_LOW);  // in main()


    // Pin Direction Setup
    MDIO_vSetPinDir(DIO_PORTA, LM35PIN, DIO_INPUT);
    MDIO_vSetPinDir(DIO_PORTA, LDRPIN, DIO_INPUT);
    MDIO_vSetPinDir(DIO_PORTA, 5, DIO_INPUT);
    MDIO_vSetPinDir(DIO_PORTA, WINDOW_LED, DIO_OUTPUT);
    MDIO_vSetPinDir(DIO_PORTA, fanPIn1, DIO_OUTPUT);
    MDIO_vSetPinDir(DIO_PORTA, TEMP_LED, DIO_OUTPUT);
    MDIO_vSetPinDir(DIO_PORTD, SERVOPIN, DIO_OUTPUT);
    MDIO_vSetPinDir(DIO_PORTD, BuzzerPin, DIO_OUTPUT);
    MDIO_vSetPinVal(DIO_PORTA, 5, DIO_HIGH);  // Door sensor pull-up

    // intrupt
    MDIO_vSetPinDir(DIO_PORTD, DIO_PIN2, DIO_INPUT);
    MDIO_vSetPinVal(DIO_PORTD, DIO_PIN2, DIO_HIGH);

    MGIE_vEnableGlobalInterrupt();
       MEXTI_vInit();
       MTIMERS_vInit();
       MADC_vInit();
       MUSART_vInit();
       HLCD_vInit();
       HKPD_vInit();

   	MEXTI_vCallBackFunction(alarm_func, 0);

    MTIMERS_vStartTimer(TIM_1_A);
    Welcome_Screen();
    LCD_STATE();

    while (1)
    {
        y = MUSART_vReceive();

        switch (y)
        {
            case 'M': fanMode = 1;doorMode = 1; ;windMode =1 ;break;
            case 'A': fanMode = 0; doorMode = 0;windMode =0 ; break;
            case 'F': if (fanMode == 1) fanControlFlag = 1; break;
            case 'f': if (fanMode == 1) fanControlFlag = 0; break;

            case 'D': if (doorMode == 1) doorControlFlag = 1; break;
            case 'd': if (doorMode == 1) doorControlFlag = 0; break;
            case 'W': if (windMode == 1) windControlFlag = 1; break;
           case 'w': if (windMode == 1) windControlFlag = 0; break;


        }
        HLCD_vSetCursorPosition(1, 13);
           HLCD_vSendChar(doorMode == 0 ? 'A' : 'M');
        LDR_func();
        LM35_func();

        if (doorMode == 0)
        {
            u8 x = MDIO_u8GETPinVal(DIO_PORTA, DIO_PIN5);
            if (x == 0)
            {
                ServoOn_func(90);
                HLCD_vSetCursorPosition(1, 4);
                    HLCD_vDisplayString("O");
                _delay_ms(1000);
                ServoOff_func();
                HLCD_vSetCursorPosition(1, 4);
                  HLCD_vDisplayString("C");
            }
        }
        else
        {
            if (doorControlFlag)
            {
                ServoOn_func(90);
                HLCD_vSetCursorPosition(1, 4);
                  HLCD_vDisplayString("O");
            }
            else
            {
                ServoOff_func();
                HLCD_vSetCursorPosition(1, 4);
                  HLCD_vDisplayString("C");
            }
        }


    }
}
void alarm_func(void){

	   	   HLCD_vClearScreen();
	        HLCD_vSetCursorPosition(0, 0);
	        HLCD_vDisplayString("alert!!");
	        HLCD_vSetCursorPosition(1, 0);
	        HLCD_vDisplayString("Closing System");
	        motorstop(DIO_PORTA, fanPIn1);
	        ServoOn_func(90);
	        BuzzerOn_Func();
	        MDIO_vSetPinVal(DIO_PORTA, TEMP_LED, DIO_HIGH);

}

void LDR_func(void)
{
    u16 LDR_reading = MADC_u16AnalogRead(CHANNEL_0);
    _delay_ms(500);


    if(windMode==0){


    if ((LDR_reading > 550) && (motorState == 0))
    {   HLCD_vSetCursorPosition(0, 10);
    		HLCD_vDisplayString("O");

    	MDIO_vSetPinVal(DIO_PORTA,WINDOW_LED,DIO_HIGH);
        motorState = 1;
    }
    else if ((LDR_reading < 500) && (motorState == 1))
    {	HLCD_vSetCursorPosition(0, 10);
	HLCD_vDisplayString("C");

	MDIO_vSetPinVal(DIO_PORTA,WINDOW_LED,DIO_LOW);
			motorState = 0;
    }
}
else{
	 if (windControlFlag)
	            {
		 HLCD_vSetCursorPosition(0, 10);
		 	 HLCD_vDisplayString("O");

		  	MDIO_vSetPinVal(DIO_PORTA,WINDOW_LED,DIO_HIGH);
	            }
	            else
	            {
	            	HLCD_vSetCursorPosition(0, 10);
	           	HLCD_vDisplayString("C");
	            MDIO_vSetPinVal(DIO_PORTA,WINDOW_LED,DIO_LOW);
	            }



}}
void LM35_func(void)
{
    u32 reading = MADC_u16AnalogRead(CHANNEL_1);
    u16 millivolts = (reading * 5000UL) / 1024;
    u16 local_temp = millivolts / 10;

    HLCD_vSetCursorPosition(1, 10);
    HLCD_vPrintNumber(local_temp);
    _delay_ms(500);

    static u8 tempState = 0;

    if (fanMode == 0)
    {
        if ((local_temp >= 30 && local_temp < 60) && tempState == 0)
        {
            HLCD_vSetCursorPosition(0, 4);
            HLCD_vDisplayString("O");
            BuzzerOn_Func();
            _delay_ms(50);
            BuzzerOff_Func();
            MotorUp_Func(DIO_PORTA, fanPIn1);
            tempState = 1;
        }
        else if (local_temp < 30 && tempState == 1)
        {
            HLCD_vSetCursorPosition(0, 4);
            HLCD_vDisplayString("C");
            motorstop(DIO_PORTA, fanPIn1);
            tempState = 0;
        }
    }
    else
    {
        if (fanControlFlag)
        {
            HLCD_vSetCursorPosition(0, 4);
            HLCD_vDisplayString("O");
            MotorUp_Func(DIO_PORTA, fanPIn1);
        }
        else
        {
            HLCD_vSetCursorPosition(0, 4);
            HLCD_vDisplayString("C");
            motorstop(DIO_PORTA, fanPIn1);
        }
    }

    if (local_temp > 60)
    {alarm_func();
    }
    else
    {
        MDIO_vSetPinVal(DIO_PORTA, TEMP_LED, DIO_LOW);
    }
}

void MotorUp_Func(u8 port, u8 pin1)
{
    MDIO_vSetPinVal(port, pin1, DIO_HIGH);
}
void motorstop(u8 port, u8 pin1)
{
    MDIO_vSetPinVal(port, pin1, DIO_LOW);
}
void BuzzerOn_Func(void)
{
    MDIO_vSetPinVal(DIO_PORTD, BuzzerPin, DIO_HIGH);
}
void BuzzerOff_Func(void)
{
    MDIO_vSetPinVal(DIO_PORTD, BuzzerPin, DIO_LOW);
}
void ServoOn_func(u8 angle)
{
    MTIMERS_vSetCompareMatch(TIM_1_A, ((125 * angle) / 180) + 125);
}
void ServoOff_func(void)
{
    MTIMERS_vSetCompareMatch(TIM_1_A, ((125 * -90) / 180) + 125);
}

void Welcome_Screen(void)
{
    u8 alarm_var = 0;
    HLCD_vClearScreen();
    HLCD_vSetCursorPosition(0, 0);
    HLCD_vDisplayString("Welcome Home");
    _delay_ms(500);
    HLCD_vSetCursorPosition(1, 0);
    HLCD_vDisplayString("Enter The key:");
    _delay_ms(500);

    HLCD_vClearScreen();
    HLCD_vSetCursorPosition(0, 0);
    HLCD_vDisplayString("THE KEY IS:");
    HLCD_vSetCursorPosition(1, 0);

    for (u8 i = 0; i < 4; i++)
    {
        u8 key = NO_KEY;
        while (key == NO_KEY)
        {
            key = HKPD_u8GetPressedKey(KeyPap);
        }
        key_enter[i] = key;
        HLCD_vSendChar(key);
        _delay_ms(100);
        HLCD_vSendChar('*');
    }
    key_enter[4] = '\0';

    u8 pass_key[5] = "1234";
    _delay_ms(300);
    HLCD_vClearScreen();
    HLCD_vSetCursorPosition(0, 0);

    if (strcmp(key_enter, pass_key) == 0)
    {
        flage = 1;
        ServoOn_func(90);
        _delay_ms(1000);
        ServoOff_func();
    }
    else if (alarm_var == 2)
    {
        HLCD_vDisplayString("alarm");
    }
    else
    {
        HLCD_vDisplayString("wrong key");
        alarm_var++;
        _delay_ms(300);
        Welcome_Screen();
    }
}

void LCD_STATE(void)
{
    HLCD_vClearScreen();
    HLCD_vSetCursorPosition(0, 0);
    HLCD_vDisplayString("FAN");
    HLCD_vSetCursorPosition(0, 6);
    HLCD_vDisplayString("WIND");
    HLCD_vSetCursorPosition(1, 0);
    HLCD_vDisplayString("DOOR");
    HLCD_vSetCursorPosition(1, 6);
    HLCD_vDisplayString("TEMP");
}
