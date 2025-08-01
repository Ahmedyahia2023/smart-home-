/*
 * STEPPER_int.h
 *
 *  Created on: Jul 23, 2025
 *      Author: pc
 */

#ifndef HAL_STEPPER_STEPPER_INT_H_
#define HAL_STEPPER_STEPPER_INT_H_


#define STEPPET_PORT	DIO_PORTA

#define STEPPER_BLUE_T	    DIO_PIN0
#define STEPPER_PINK_T	    DIO_PIN1
#define STEPPER_ORANGE_T	DIO_PIN2
#define STEPPER_YELLOW_T	DIO_PIN3

void STEPPER_vInit(void);
void STEPPER_vRotateCW(f64 A_f64Speed); /* TODO : angle */
void STEPPER_vRotateCCW(f64 A_f64Speed); /* TODO : angle */
void STEPPER_vStop(void);


#endif /* HAL_STEPPER_STEPPER_INT_H_ */
