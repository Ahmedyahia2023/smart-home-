/*
 * BIT_MATH.h
 *
 *  Created on: Feb 20, 2023
 *      Author: Nada Mamdouh
 */

#ifndef LIB_BIT_MATH_H_
#define LIB_BIT_MATH_H_

#define No_of_bits 8
#define SET_BIT(reg,bitN)       	(reg|=  (1<<bitN))
#define CLR_BIT(reg,bitN)       	(reg&= (~(1<<bitN)))
#define TOG_BIT(reg,bitN)           (reg^=  (1<<bitN))
#define GET_BIT(reg,bitN)           ((reg>>bitN) &0x01)
#define SET_BYTE(reg,value)         ( reg = value )
#define ROT_LEFT(reg,bitN)          (reg<<bitN)|(reg>>(No_of_bits - bitN))
#define ROT_RIGHT(reg,bitN)         (reg>>bitN)|(reg<<(No_of_bits - bitN))



#endif /* LIB_BIT_MATH_H_ */
