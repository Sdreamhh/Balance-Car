/*********************************************************************************************************

1. ����TIM1������ ��
2. �������ڵ��ԡ� 	PA9 Rx PA10 Tx

3. ���ñ����� �� TIM2 PA0 PA1 : E1A E1B;  TIM4 PB6 PB7 �� E2A E2B
4. ����PWM	��		TIM3 CH1-4 A:IN1->PA6 IN2->PA7 B:IN1->PB1 IN2->PB0
5. ��ֲMPU6050 ��		PB10 PB11 SCL  SDA 

*********************************************************************************************************/
#include "Main.h"
#include "stm32f10x_conf.h"
#include "DataType.h"
#include "NVIC.h"
#include "SysTick.h"
#include "RCC.h"
#include "Timer.h"
#include "UART1.h"
#include "LED.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 

#include "Encoder.h"
#include "PWM.h"
#include "Motor.h"
#include "PID.h"


#include "Exti.h"


static  void  InitHardware(void);   //��ʼ��Ӳ����ص�ģ��
static  void  Proc2msTask(void);    //2ms��������

static  void  InitHardware(void)
{  
  SystemInit();       //ϵͳ��ʼ��
  InitRCC();          //��ʼ��RCCģ��
  InitNVIC();         //��ʼ��NVICģ��
  InitUART1(9600);  	//��ʼ��UARTģ��
  InitTimer();        //��ʼ��Timerģ��
  InitLED();          //��ʼ��LEDģ��
  InitSysTick();      //��ʼ��SysTickģ��
	
	InitEncoder();
	InitPWM();
	MotorLoad(0, 0);

	MPU_Init();					//��ʼ��MPU6050
	
	while(mpu_dmp_init() != 0) 
	{
		printf("Loading\r\n");
	}  
		
	InitMPU6050Exti();

}


static  void  Proc2msTask(void)
{  
  if(Get2msFlag())  //�ж�2ms��־״̬
  { 
	
    Clr2msFlag();   //���2ms��־
  }
}


int main(void)
{ 
  InitHardware();   //��ʼ��Ӳ����غ���
  

  while(1)
  {
    Proc2msTask();  //2ms��������
  }
}
