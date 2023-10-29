/*********************************************************************************************************

1. 配置TIM1调试用 √
2. 蓝牙串口调试√ 	PA9 Rx PA10 Tx

3. 配置编码器 √ TIM2 PA0 PA1 : E1A E1B;  TIM4 PB6 PB7 ： E2A E2B
4. 配置PWM	√		TIM3 CH1-4 A:IN1->PA6 IN2->PA7 B:IN1->PB1 IN2->PB0
5. 移植MPU6050 √		PB10 PB11 SCL  SDA 

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


static  void  InitHardware(void);   //初始化硬件相关的模块
static  void  Proc2msTask(void);    //2ms处理任务

static  void  InitHardware(void)
{  
  SystemInit();       //系统初始化
  InitRCC();          //初始化RCC模块
  InitNVIC();         //初始化NVIC模块
  InitUART1(9600);  	//初始化UART模块
  InitTimer();        //初始化Timer模块
  InitLED();          //初始化LED模块
  InitSysTick();      //初始化SysTick模块
	
	InitEncoder();
	InitPWM();
	MotorLoad(0, 0);

	MPU_Init();					//初始化MPU6050
	
	while(mpu_dmp_init() != 0) 
	{
		printf("Loading\r\n");
	}  
		
	InitMPU6050Exti();

}


static  void  Proc2msTask(void)
{  
  if(Get2msFlag())  //判断2ms标志状态
  { 
	
    Clr2msFlag();   //清除2ms标志
  }
}


int main(void)
{ 
  InitHardware();   //初始化硬件相关函数
  

  while(1)
  {
    Proc2msTask();  //2ms处理任务
  }
}
