#include "PID.h"
#include "stm32f10x_conf.h"
#include "stdio.h"
#include "Encoder.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "DataType.h"
#include "Motor.h"
#include "stdio.h"


float pitch,roll,yaw; 		//欧拉角
short aacx,aacy,aacz;			//加速度传感器原始数据
short gyrox,gyroy,gyroz;	//陀螺仪原始数据

i32 med_angle = 0;
i32 expected_angle = 0;
i32 expected_speed = 0;


//直立环
float Kp2 = -5000, Kd = 11.19;

//速度环
float Kp1 = 3000,  Ki = 27;

/*速度环 PWM_out = Kp1 * err + Ki * s_err*/
static i32 PI_Velocity()
{
	i32 PWM_out, err;
	
	static i32 sum_err;
	
	err = expected_speed - GetEncoderSpeed(TIM2);
	sum_err += err;
	
	//积分限幅
	if(sum_err >  6000) sum_err = 6000;
	if(sum_err < -6000) sum_err =-6000;
	
	PWM_out = Kp1 * err + Ki * sum_err;
	
	return PWM_out;
}


/*直立环*/
static i32 PD_Vertical(i32 now_angle, i32 gy)
{
	i32 PWM_out, err;
		
	err = med_angle + expected_angle - now_angle;
	
	PWM_out = Kp2 * err + Kd * gy;
	
	return PWM_out;
}

extern u8 flag_move, flag_back, flag_stop, flag_left, flag_right;

/*转向环*/
static i32 Turn(void)
{
	i32 PWM_out;
	float a = -1.5, b = 0;
	i32 turn_speed = 15000;
	
	if(flag_left)
	{
		a = 0;
		b = 1;
	}
	else if(flag_right)
	{
		a = 0;
		b = -1;
	}
	else 
	{
		a = -1.5;
		b = 0;
	}
	
	PWM_out = a * gyroz + b * turn_speed;
	
	return PWM_out;
}


static void BlueTooth()							//蓝牙控制模块
{
		if(flag_move)
	{
		expected_angle = 7;
		expected_speed = 15;
	}
	
	if(flag_stop)
	{
		expected_angle = 0;
		expected_speed = 0;
	}
	
	if(flag_back)
	{
		expected_angle = -7;
		expected_speed = -15;
	}
	
}

void PID(void)
{
	i32 PWM, Turn_out;
	
	
	PWM = PD_Vertical(pitch , gyroy) + PI_Velocity();
	//限幅
	if(PWM > PWMMAX) PWM = PWMMAX;
	if(PWM < PWMMIN) PWM = PWMMIN;
	
//	//死区
//	if(PWM > 0) PWM += 1200;
//	if(PWM < 0) PWM -= 1200;
	
	
	//蓝牙控制模块
	BlueTooth();
	
	//if(flag_left || flag_right) PWM = 0;
	
	Turn_out = Turn();

	MotorLoad(PWM + Turn_out, PWM - Turn_out);

}

//主控函数在mpu6050的外部中断函数执行
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5)!=0)
	{
		if(PBin(5) == 0)
		{
			EXTI_ClearITPendingBit(EXTI_Line5);	
			
			while(mpu_dmp_get_data(&pitch, &roll, &yaw) != 0) {}
			
			MPU_Get_Accelerometer(&aacx, &aacy, &aacz);	//得到加速度传感器数据
			MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);	//得到陀螺仪数据
				
			PID();
		}
	}
}


