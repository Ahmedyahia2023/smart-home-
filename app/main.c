
#define F_CPU 	8000000UL
#include "../LIB/BIT_MATH.h"
#include "../LIB/STD_TYPES.h"
#include "../MCAL/ADC/ADC_int.h"
#include "../MCAL/ADC/ADC_prv.h"
#include "../MCAL/DIO/DIO_int.h"
#include "../MCAL/DIO/DIO_prv.h"


#include "util/delay.h"
/////////////////////////////////////////////////////////////////
#define LDRPIN 0
#define MOTORPIN1 2
#define MOTORPIN2 3
//////////////////////////////////
u8 motorState = 0;

u16 LDR_reading ;
void main(void)
{
	MDIO_vSetPinDir(DIO_PORTA,LDRPIN,DIO_INPUT);

	MDIO_vSetPinDir(DIO_PORTA,MOTORPIN1,DIO_OUTPUT);
	MDIO_vSetPinDir(DIO_PORTA,MOTORPIN2,DIO_OUTPUT);


	MADC_vInit();

while (1){
LDR_func();

}
}
void LDR_func(void){

LDR_reading=MADC_u16AnalogRead(CHANNEL_0);
	if(LDR_reading>=800 &&motorState==0){
		MotorUp_Func();
		motorState =1;



	}
	else if(LDR_reading<700 && motorState == 1){
		MotorDown_Func();
		_delay_ms(300);
		motorState =0;
	}
}
void MotorUp_Func(void){
	MDIO_vSetPinVal(DIO_PORTA,MOTORPIN1,DIO_HIGH);
	MDIO_vSetPinVal(DIO_PORTA,MOTORPIN2,DIO_LOW);
		_delay_ms(3000);
		MDIO_vSetPinVal(DIO_PORTA,MOTORPIN1,DIO_LOW);
		MDIO_vSetPinVal(DIO_PORTA,MOTORPIN2,DIO_LOW);

}
void MotorDown_Func(void){
	MDIO_vSetPinVal(DIO_PORTA,MOTORPIN2,DIO_HIGH);
		MDIO_vSetPinVal(DIO_PORTA,MOTORPIN1,DIO_LOW);
		_delay_ms(3000);
		MDIO_vSetPinVal(DIO_PORTA,MOTORPIN1,DIO_LOW);
		MDIO_vSetPinVal(DIO_PORTA,MOTORPIN2,DIO_LOW);
}

