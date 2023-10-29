#include "PWM.h"
#include "DataType.h"
#include "stm32f10x_conf.h"



//TIM3 CH2 PB5
static void ConfigTimer3ForPWM(u16 arr)
{

	GPIO_InitTypeDef GPIO_InitStructure;                //����ṹ��GPIO_InitStructure
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;      //����ṹ��TIM_TimeBaseStructure   
	TIM_OCInitTypeDef TIM_OCInitStructure;              //����ṹ��TIM_OCInitStructure
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PB�˿�ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//ʹ�ܶ�ʱ��3
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //����ģʽ���
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1; //PB0 ��PB1
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;        //IO���ٶ�
	GPIO_Init(GPIOB, &GPIO_InitStructure);        	//GPIO��ʼ��
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = arr;               						//������һ�����»���Զ���װ�ؼĴ�����ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 0;             							//Ԥ����ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;           //ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 			//���ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode= TIM_OCMode_PWM2;             //PWM�����ȵ���1
	TIM_OCInitStructure.TIM_Pulse = 0;                           //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //����TIM�������Ϊ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	
	TIM_CtrlPWMOutputs(TIM3, ENABLE);//�����ʹ��
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);//ʹ��Ԥװ�ؼĴ���
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);              //ʹ���Զ�װ������λ
	TIM_Cmd(TIM3, ENABLE);														//������ʱ��3
}



void  InitPWM(void)
{

	ConfigTimer3ForPWM(65535);
	
	
}


