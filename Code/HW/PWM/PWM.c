#include "PWM.h"
#include "DataType.h"
#include "stm32f10x_conf.h"



//TIM3 CH2 PB5
static void ConfigTimer3ForPWM(u16 arr)
{

	GPIO_InitTypeDef GPIO_InitStructure;                //定义结构体GPIO_InitStructure
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;      //定义结构体TIM_TimeBaseStructure   
	TIM_OCInitTypeDef TIM_OCInitStructure;              //定义结构体TIM_OCInitStructure
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA,ENABLE);//使能PB端口时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//使能定时器3
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //复用模式输出
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1; //PB0 、PB1
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;        //IO口速度
	GPIO_Init(GPIOB, &GPIO_InitStructure);        	//GPIO初始化
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = arr;               						//设置下一个更新活动的自动重装载寄存器的值
	TIM_TimeBaseStructure.TIM_Prescaler = 0;             							//预分配值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;           //时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 			//向上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode= TIM_OCMode_PWM2;             //PWM脉冲宽度调制1
	TIM_OCInitStructure.TIM_Pulse = 0;                           //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //设置TIM输出极性为高
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	
	TIM_CtrlPWMOutputs(TIM3, ENABLE);//主输出使能
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);//使能预装载寄存器
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);              //使能自动装载允许位
	TIM_Cmd(TIM3, ENABLE);														//启动定时器3
}



void  InitPWM(void)
{

	ConfigTimer3ForPWM(65535);
	
	
}


