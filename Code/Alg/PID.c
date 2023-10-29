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


float pitch,roll,yaw; 		//ŷ����
short aacx,aacy,aacz;			//���ٶȴ�����ԭʼ����
short gyrox,gyroy,gyroz;	//������ԭʼ����

i32 med_angle = 0;
i32 expected_angle = 0;
i32 expected_speed = 0;


//ֱ����
float Kp2 = -5000, Kd = 11.19;

//�ٶȻ�
float Kp1 = 3000,  Ki = 27;

/*�ٶȻ� PWM_out = Kp1 * err + Ki * s_err*/
static i32 PI_Velocity()
{
	i32 PWM_out, err;
	
	static i32 sum_err;
	
	err = expected_speed - GetEncoderSpeed(TIM2);
	sum_err += err;
	
	//�����޷�
	if(sum_err >  6000) sum_err = 6000;
	if(sum_err < -6000) sum_err =-6000;
	
	PWM_out = Kp1 * err + Ki * sum_err;
	
	return PWM_out;
}


/*ֱ����*/
static i32 PD_Vertical(i32 now_angle, i32 gy)
{
	i32 PWM_out, err;
		
	err = med_angle + expected_angle - now_angle;
	
	PWM_out = Kp2 * err + Kd * gy;
	
	return PWM_out;
}

extern u8 flag_move, flag_back, flag_stop, flag_left, flag_right;

/*ת��*/
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


static void BlueTooth()							//��������ģ��
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
	//�޷�
	if(PWM > PWMMAX) PWM = PWMMAX;
	if(PWM < PWMMIN) PWM = PWMMIN;
	
//	//����
//	if(PWM > 0) PWM += 1200;
//	if(PWM < 0) PWM -= 1200;
	
	
	//��������ģ��
	BlueTooth();
	
	//if(flag_left || flag_right) PWM = 0;
	
	Turn_out = Turn();

	MotorLoad(PWM + Turn_out, PWM - Turn_out);

}

//���غ�����mpu6050���ⲿ�жϺ���ִ��
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5)!=0)
	{
		if(PBin(5) == 0)
		{
			EXTI_ClearITPendingBit(EXTI_Line5);	
			
			while(mpu_dmp_get_data(&pitch, &roll, &yaw) != 0) {}
			
			MPU_Get_Accelerometer(&aacx, &aacy, &aacz);	//�õ����ٶȴ���������
			MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);	//�õ�����������
				
			PID();
		}
	}
}


