#define F_CPU 8000000UL
#include <util/delay.h>

#include "../LIB/BIT_MATH.h"
#include "../LIB/STD_TYPES.h"

#include "../MCAL/ADC/ADC_int.h"
#include "../MCAL/ADC/ADC_prv.h"
#include "../MCAL/DIO/DIO_int.h"
#include "../MCAL/EXTI/EXTI_int.h"
#include "../MCAL/GIE/GIE_int.h"
#include "../MCAL/USART/USART_int.h"
#include "../MCAL/TIMERS/TIMERS_cfg.h"
#include "../MCAL/TIMERS/TIMERS_int.h"
#include "../HAL/KPD/KPD_int.h"
#include "../HAL/LCD/LCD_int.h"
////////////////////////////////////////////////
/*used ports*/
/*
	KEYPAD PORTB ROWS 4,5,6,7
	KEYPAD PORTB COLUMNS 3,4,5
	#define LCD_CTRL_PORT 	DIO_PORTD
	#define LCD_DATA_PORT 	DIO_PORTC
	#define LCD_RS			DIO_PIN0
	#define LCD_RW			DIO_PIN1
	#define LCD_E			DIO_PIN2
	ADC ON PORTA
*/
/////////////////////////////////////////////////
#define LDRPIN     0   // ADC Channel 0
#define LM35PIN    1   // ADC Channel 1
#define MOTORPIN1  2   // PA2
#define MOTORPIN2  3   // PA3
#define TEMP_LED   4   // PA4
#define SERVOPIN 	5 //SERVO PD5
#define BuzzerPin   1// BUZZER	PB1
#define fanPIn1		2	//PB2
#define fanPIn2		3	//PB3
/////////////////////////////////////////////////
u8 motorState = 0;
u8 overheatFlag = 0;

u16 LM35_reading = 0;
u16 LDR_reading = 0;
u8 temp = 0;

u8 KeyPap[KPD_ROWS][KPD_COLS] =
{
    {'7', '8', '9'},
    {'4', '5', '6'},
    {'1', '2', '3'},
    {'*', '0', '#'}
};
u8 key_enter[5];

/////////////////////////////////////////////////
void LDR_func(void);
void MotorUp_Func(u8 port, u8 pin1 ,u8 pin2);
void MotorDown_Func(u8 port, u8 pin1 ,u8 pin2);
void motorstop(u8 port, u8 pin1 ,u8 pin2);
void LM35_func(void);
void Welcome_Screen(void);
void ServoOn_func(u8 angle);
void ServoOff_func(void);
/////////////////////////////////////////////////

void main(void)
{
    MGIE_vEnableGlobalInterrupt();
    MEXTI_vInit();
    MTIMERS_vInit();
    MADC_vInit();
    MUSART_vInit();
    HLCD_vInit();

    // Input Pins
    MDIO_vSetPinDir(DIO_PORTA, LM35PIN, DIO_INPUT);
    MDIO_vSetPinDir(DIO_PORTA, LDRPIN, DIO_INPUT);
    MDIO_vSetPinDir(DIO_PORTA, 6, DIO_INPUT);
    // Motor Control Output Pins
    MDIO_vSetPinDir(DIO_PORTA, MOTORPIN1, DIO_OUTPUT);
    MDIO_vSetPinDir(DIO_PORTA, MOTORPIN2, DIO_OUTPUT);
    MDIO_vSetPinDir(DIO_PORTB, fanPIn1, DIO_OUTPUT);
       MDIO_vSetPinDir(DIO_PORTB, fanPIn2, DIO_OUTPUT);
    //BUZZER
    MDIO_vSetPinDir(DIO_PORTB, BuzzerPin, DIO_OUTPUT);


    // LED or test pin output
    MDIO_vSetPinDir(DIO_PORTA, TEMP_LED, DIO_OUTPUT);
    MDIO_vSetPinDir(DIO_PORTD, SERVOPIN, DIO_OUTPUT);
        //TEST=++++===
        MDIO_vSetPinVal(DIO_PORTA, 6, DIO_HIGH);

    // Set temperature alarm check every X ticks (adjust internally)
    MTIMERS_vStartTimer(TIM_1_A);

    // SERVO PINS


    while (1)
    {	LDR_func();
    	LM35_func();

        // Skip all logic if overheat occurred




        	u8 x=MDIO_u8GETPinVal(DIO_PORTA, DIO_PIN6);
        	 			if(x==0)
        	 			{ServoOn_func(90);
        	 				_delay_ms(500);
        	 				ServoOff_func();
        	 			}

       // _delay_ms(500);
    }
}

/////////////////////////////////////////////////
void LDR_func(void)
{
    LDR_reading = MADC_u16AnalogRead(LDRPIN);

    if (LDR_reading >= 800 && motorState == 0)
    {

        MotorUp_Func(DIO_PORTA,MOTORPIN1,MOTORPIN2);
        _delay_ms(500);
        motorstop(DIO_PORTA,MOTORPIN1,MOTORPIN2);
        _delay_ms(500);
        motorState = 1;
    }
    else if (LDR_reading < 300 && motorState == 1)
    {
        MotorDown_Func(DIO_PORTA,MOTORPIN1,MOTORPIN2);
        _delay_ms(500);
        motorstop(DIO_PORTA,MOTORPIN1,MOTORPIN2);
        _delay_ms(500);
        motorState = 0;
    }
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
void MotorUp_Func(u8 port, u8 pin1 ,u8 pin2)
{
    MDIO_vSetPinVal(port, pin2, DIO_HIGH);
    MDIO_vSetPinVal(port, pin1, DIO_LOW);
}

void MotorDown_Func(u8 port, u8 pin1 ,u8 pin2)
{
    MDIO_vSetPinVal(port, pin1, DIO_HIGH);
    MDIO_vSetPinVal(port, pin2, DIO_LOW);
}

void motorstop(u8 port, u8 pin1 ,u8 pin2)
{
    MDIO_vSetPinVal(port, pin2, DIO_LOW);
    MDIO_vSetPinVal(port, pin1, DIO_LOW);
}

/////////////////////////////////////////////////


//THE WELCOME SCREEN
void Welcome_Screen(void){
	static u8 alarm_var=0;
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

	for(u8 i = 0; i < 4; i++)
	{
		u8 key = NO_KEY;//if it high meaning no switch pressed
		while(key == NO_KEY)//get out of the loop when sw pressed as low
		{
			key = HKPD_u8GetPressedKey(KeyPap);
		}
		key_enter[i] = key;
		HLCD_vSendChar(key);  // Send as char (e.g., '1', '2')
		_delay_ms(100);
		HLCD_vSendChar('*');
	}
	key_enter[4] = '\0';

	u8 pass_key[5]="1234";
	_delay_ms(300);
	HLCD_vClearScreen();
	HLCD_vSetCursorPosition(0, 0);

	if(strcmp(key_enter,pass_key)==0)
	{
		return;
	}

	else if(alarm_var==2){
		HLCD_vDisplayString("alarm");
	}

	else{
		HLCD_vDisplayString("wrong key");
		alarm_var=alarm_var+1;
		_delay_ms(300);
		Welcome_Screen();
	}


}
void BuzzerOn_Func(void){
	MDIO_vSetPinVal(DIO_PORTB, BuzzerPin, DIO_HIGH);
}

void BuzzerOff_Func(void){
	MDIO_vSetPinVal(DIO_PORTB, BuzzerPin, DIO_LOW);


}
void ServoOn_func(u8 angle){
MTIMERS_vSetCompareMatch(TIM_1_A,((125*angle)/180)+125);
}
void ServoOff_func(void)

{

MTIMERS_vSetCompareMatch(TIM_1_A, 0); // No pulse

	}

void LM35_func(void)
{

	static u8 tempState=0;
    u32 reading = MADC_u16AnalogRead(CHANNEL_1);
    u16 millivolts = (reading * 5000UL) / 1024;
    u16 local_temp = millivolts / 10;
    // USART Test Output
     //  MUSART_vTransmit((local_temp / 10) + '0');
      // MUSART_vTransmit((local_temp % 10) + '0');
       //MUSART_vTransmit('\n');

      if (local_temp >= 30 && local_temp < 60 && tempState == 0)
       {BuzzerOff_Func();
    	  HLCD_vClearScreen();
    	  HLCD_vSetCursorPosition(0,0);
    	  HLCD_vDisplayString("Open fan");
           BuzzerOn_Func();
           _delay_ms(50);
           BuzzerOff_Func();
           tempState = 1;
           MotorUp_Func(DIO_PORTB,fanPIn1,fanPIn2);

       }
       else if (local_temp < 30 && tempState == 1)
       {BuzzerOff_Func();
    	 HLCD_vClearScreen();
    	 HLCD_vSetCursorPosition(0,0);
    	 ServoOff_func();
    	 HLCD_vDisplayString("Close Window");
           tempState = 0;

           motorstop(DIO_PORTB,fanPIn1,fanPIn2);
       }
    if (local_temp > 60)
    {
        HLCD_vClearScreen();
        HLCD_vSetCursorPosition(0,0);
        HLCD_vDisplayString("alert!!");
        HLCD_vSetCursorPosition(1,0);
        HLCD_vDisplayString("Closing System");
        motorstop(DIO_PORTB,fanPIn1,fanPIn2);  // Stop motor
        ServoOn_func(90);
        BuzzerOn_Func();
         MDIO_vSetPinVal(DIO_PORTA, TEMP_LED, DIO_HIGH);
         tempState = 1;
// Overheat LED ON
    }
    else
        MDIO_vSetPinVal(DIO_PORTA, TEMP_LED, DIO_LOW);  // Overheat LED ON


}





