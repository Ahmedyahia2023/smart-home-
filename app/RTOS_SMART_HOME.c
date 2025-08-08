#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

/* FreeRTOS includes */
#include "../Free_RTOS/FreeRTOS.h"
#include "../Free_RTOS/task.h"
#include "../Free_RTOS/queue.h"
#include "../Free_RTOS/semphr.h"

/* Your project headers */
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

/* Pin definitions */
#define LDRPIN     0
#define LM35PIN    1
#define WINDOW_LED 2
#define TEMP_LED   4
#define fanPIn1    6
#define SERVOPIN   5
#define BuzzerPin  7

/* Shared state (volatile for cross-context access) */
volatile u8 motorState = 0;
volatile u8 y = 0;
volatile u8 flage = 0;

volatile u8 fanMode = 0;         /* 0: Auto, 1: Manual */
volatile u8 fanControlFlag = 0;  /* Fan manual control */

volatile u8 doorMode = 0;        /* 0: Auto, 1: Manual */
volatile u8 doorControlFlag = 0; /* Door manual control */

volatile u8 windMode = 0;        /* 0: Auto, 1: Manual */
volatile u8 windControlFlag = 0;

/* Keypad */
u8 KeyPap[KPD_ROWS][KPD_COLS] = {
    {'7', '8', '9'},
    {'4', '5', '6'},
    {'1', '2', '3'},
    {'*', '0', '#'}
};
u8 key_enter[5];

/* Prototypes */
void LDR_func_nodelay(void);
void LM35_func_nodelay(void);
void MotorUp_Func(u8 port, u8 pin1);
void motorstop(u8 port, u8 pin1);
void Welcome_Screen(void);
void ServoOn_func(u8 angle);
void ServoOff_func(void);
void BuzzerOn_Func(void);
void BuzzerOff_Func(void);
void LCD_STATE(void);
void alarm_func(void);

/* FreeRTOS objects */
xQueueHandle xUartQueue;

/* --------------------------- Tasks ---------------------------- */

void vUartTask(void *pvParameters)
{
    u8 cmd;
    (void)pvParameters;
    for (;;)
    {
        if (xQueueReceive(xUartQueue, &cmd, portMAX_DELAY) == pdPASS)
        {
            switch (cmd)
            {
                case 'M': fanMode = 1; doorMode = 1; windMode = 1; break;
                case 'A': fanMode = 0; doorMode = 0; windMode = 0; break;
                case 'F': if (fanMode == 1) fanControlFlag = 1; break;
                case 'f': if (fanMode == 1) fanControlFlag = 0; break;
                case 'D': if (doorMode == 1) doorControlFlag = 1; break;
                case 'd': if (doorMode == 1) doorControlFlag = 0; break;
                case 'W': if (windMode == 1) windControlFlag = 1; break;
                case 'w': if (windMode == 1) windControlFlag = 0; break;
                default: break;
            }
        }
    }
}

void vLightTask(void *pvParameters)
{
    (void)pvParameters;
    const portTickType xPeriod = pdMS_TO_TICKS(500);
    for (;;)
    {
        LDR_func_nodelay();
        vTaskDelay(xPeriod);
    }
}

void vTempTask(void *pvParameters)
{
    (void)pvParameters;
    const portTickType xPeriod = pdMS_TO_TICKS(500);
    for (;;)
    {
        LM35_func_nodelay();
        vTaskDelay(xPeriod);
    }
}

void vDoorTask(void *pvParameters)
{
    (void)pvParameters;
    const portTickType xPeriod = pdMS_TO_TICKS(200);
    for (;;)
    {
        if (doorMode == 0)
        {
            u8 x = MDIO_u8GETPinVal(DIO_PORTA, DIO_PIN5);
            if (x == 0)
            {
                ServoOn_func(90);
                HLCD_vSetCursorPosition(1, 4);
                HLCD_vDisplayString("O");
                vTaskDelay(pdMS_TO_TICKS(1000));
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
        vTaskDelay(xPeriod);
    }
}

/* ---------------------- ISR: USART RX ------------------------- */
/* Many AVR headers use USART_RXC_vect for ATmega32A; if your compiler uses
   USART_RX_vect or USART0_RX_vect, replace accordingly. */
ISR(USART_RXC_vect)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    u8 c = UDR; /* read received byte from UART data register (ATmega32A) */
    if (xUartQueue != NULL)
    {
        xQueueSendFromISR(xUartQueue, &c, &xHigherPriorityTaskWoken);
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* ------------------------ main -------------------------------- */
int main(void)
{
    /* Basic pin initialization (kept from your original) */
    MDIO_vSetPinVal(DIO_PORTA, WINDOW_LED, DIO_LOW);

    MDIO_vSetPinDir(DIO_PORTA, LM35PIN, DIO_INPUT);
    MDIO_vSetPinDir(DIO_PORTA, LDRPIN, DIO_INPUT);
    MDIO_vSetPinDir(DIO_PORTA, 5, DIO_INPUT);
    MDIO_vSetPinDir(DIO_PORTA, WINDOW_LED, DIO_OUTPUT);
    MDIO_vSetPinDir(DIO_PORTA, fanPIn1, DIO_OUTPUT);
    MDIO_vSetPinDir(DIO_PORTA, TEMP_LED, DIO_OUTPUT);
    MDIO_vSetPinDir(DIO_PORTD, SERVOPIN, DIO_OUTPUT);
    MDIO_vSetPinDir(DIO_PORTD, BuzzerPin, DIO_OUTPUT);
    MDIO_vSetPinVal(DIO_PORTA, 5, DIO_HIGH);  /* Door sensor pull-up */

    /* External interrupt pin */
    MDIO_vSetPinDir(DIO_PORTD, DIO_PIN2, DIO_INPUT);
    MDIO_vSetPinVal(DIO_PORTD, DIO_PIN2, DIO_HIGH);

    /* Init drivers */
    MGIE_vEnableGlobalInterrupt();
    MEXTI_vInit();
    MTIMERS_vInit();
    MADC_vInit();
    MUSART_vInit();   /* enable RX Complete Interrupt (RXCIE) */
    HLCD_vInit();
    HKPD_vInit();

    /* Register EXTI callback (keeps alarm_func as immediate handler) */
    MEXTI_vCallBackFunction(alarm_func, 0);

    /* Timer used for servo */
    MTIMERS_vStartTimer(TIM_1_A);

    /* Blocking welcome screen before RTOS scheduler runs */
    Welcome_Screen();
    LCD_STATE();

    /* Create queue for UART bytes */
    xUartQueue = xQueueCreate(10, sizeof(u8));//8 bytes (the size of the data of the uart)
    if (xUartQueue == NULL)
    {
        while (1) { /* blink LED or halt here */ }
    }

    /* Create tasks - keep stacks small on ATmega32 (2KB SRAM) */
    if (xTaskCreate(vUartTask, "UART", 120, NULL, 3, NULL) != pdPASS) while (1) {};
    if (xTaskCreate(vLightTask, "Light", 120, NULL, 2, NULL) != pdPASS) while (1) {};
    if (xTaskCreate(vTempTask, "Temp", 120, NULL, 2, NULL) != pdPASS) while (1) {};
    if (xTaskCreate(vDoorTask, "Door", 120, NULL, 1, NULL) != pdPASS) while (1) {};

    /* Start scheduler */
    vTaskStartScheduler();

    /* If scheduler exits, loop forever */
    while (1) { }
    return 0;
}

/* ------------------------ Functions --------------------------- */

void alarm_func(void)
{
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

void LDR_func_nodelay(void)
{
    u16 LDR_reading = MADC_u16AnalogRead(CHANNEL_0);

    if (windMode == 0)
    {
        if ((LDR_reading > 550) && (motorState == 0))
        {
            HLCD_vSetCursorPosition(0, 10);
            HLCD_vDisplayString("O");
            MDIO_vSetPinVal(DIO_PORTA, WINDOW_LED, DIO_HIGH);
            motorState = 1;
        }
        else if ((LDR_reading < 500) && (motorState == 1))
        {
            HLCD_vSetCursorPosition(0, 10);
            HLCD_vDisplayString("C");
            MDIO_vSetPinVal(DIO_PORTA, WINDOW_LED, DIO_LOW);
            motorState = 0;
        }
    }
    else
    {
        if (windControlFlag)
        {
            HLCD_vSetCursorPosition(0, 10);
            HLCD_vDisplayString("O");
            MDIO_vSetPinVal(DIO_PORTA, WINDOW_LED, DIO_HIGH);
        }
        else
        {
            HLCD_vSetCursorPosition(0, 10);
            HLCD_vDisplayString("C");
            MDIO_vSetPinVal(DIO_PORTA, WINDOW_LED, DIO_LOW);
        }
    }
}

void LM35_func_nodelay(void)
{
    u32 reading = MADC_u16AnalogRead(CHANNEL_1);
    u16 millivolts = (reading * 5000UL) / 1024;
    u16 local_temp = millivolts / 10;

    HLCD_vSetCursorPosition(1, 10);
    HLCD_vPrintNumber(local_temp);

    static u8 tempState = 0;

    if (fanMode == 0)
    {
        if ((local_temp >= 30 && local_temp < 60) && tempState == 0)
        {
            HLCD_vSetCursorPosition(0, 4);
            HLCD_vDisplayString("O");
            BuzzerOn_Func();
            /* short blocking beep - small delay */
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
    {
        alarm_func();
    }
    else
    {
        MDIO_vSetPinVal(DIO_PORTA, TEMP_LED, DIO_LOW);
    }
}

/* Hardware wrappers */
void MotorUp_Func(u8 port, u8 pin1) { MDIO_vSetPinVal(port, pin1, DIO_HIGH); }
void motorstop(u8 port, u8 pin1)    { MDIO_vSetPinVal(port, pin1, DIO_LOW); }
void BuzzerOn_Func(void)            { MDIO_vSetPinVal(DIO_PORTD, BuzzerPin, DIO_HIGH); }
void BuzzerOff_Func(void)           { MDIO_vSetPinVal(DIO_PORTD, BuzzerPin, DIO_LOW); }
void ServoOn_func(u8 angle)         { MTIMERS_vSetCompareMatch(TIM_1_A, ((125 * angle) / 180) + 125); }
void ServoOff_func(void)            { MTIMERS_vSetCompareMatch(TIM_1_A, ((125 * -90) / 180) + 125); }

/* Welcome screen - kept blocking (runs before scheduler) */
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

    if (strcmp((char *)key_enter, (char *)pass_key) == 0)
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
