#ifndef _MOTOR_H_
#define _MOTOR_H_
#include "DataType.h"

#define   PWMA_IN1  TIM3->CCR1
#define   PWMA_IN2  TIM3->CCR2
#define   PWMB_IN1  TIM3->CCR3
#define   PWMB_IN2  TIM3->CCR4


#define PWMMAX 35000
#define PWMMIN -35000


typedef enum 
{
	DIR_FORWORD, DIR_REVERSE, DIR_TURN
} DIR;

void MotorLoad(i32 MotorA, i32 MotorB);

#endif
