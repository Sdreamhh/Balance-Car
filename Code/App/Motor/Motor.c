#include "Motor.h"
#include "stm32f10x_conf.h"
#include "stdio.h"


void MotorLoad(i32 MotorA, i32 MotorB)			//配置PWM
{
	DIR dir;


	if(MotorA < 0 && MotorB < 0) 
	{
		dir = DIR_FORWORD;
		MotorA = -MotorA;
		MotorB = -MotorB;
	}
	else if(MotorA > 0 && MotorB > 0)
	{
		dir = DIR_REVERSE; 
	}
	else 
	{
		dir = DIR_TURN;
	}
	
		
	if(dir == DIR_REVERSE)			//使用4路PWM控制，所以需要考虑正反转问题
	{
		PWMA_IN1 = MotorA;
		PWMA_IN2 = 0;
		
		PWMB_IN1 = MotorB;
		PWMB_IN2 = 0;
	}
	else if(dir == DIR_FORWORD)
	{
	
		PWMA_IN1 = 0;
		PWMA_IN2 = MotorA;	
		
		PWMB_IN1 = 0;
		PWMB_IN2 = MotorB;
	}
	else if(dir == DIR_TURN)
	{
		if(MotorA > 0 && MotorB < 0)
		{
			PWMA_IN1 = 0;
			PWMA_IN2 = MotorA;	
			
			PWMB_IN1 = -MotorB;
			PWMB_IN2 = 0;
		}
		else if(MotorA < 0 && MotorB > 0)
		{
			PWMA_IN1 = -MotorA;
			PWMA_IN2 = 0;	
			
			PWMB_IN1 = 0;
			PWMB_IN2 = MotorB;
		}
	}
	
}



