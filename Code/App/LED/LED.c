#include "LED.h"
#include "stm32f10x_conf.h"
#include "stdio.h"
#include "Encoder.h"

static  void  ConfigLEDGPIO(void);  //����LED��GPIO

static  void  ConfigLEDGPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  //GPIO_InitStructure���ڴ��GPIO�Ĳ���
                                                                     
  //ʹ��RCC���ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //ʹ��GPIOC��ʱ��
                
	//PC13
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;           //��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //����I/O����ٶ�
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;     //����ģʽ
  GPIO_Init(GPIOC, &GPIO_InitStructure);                //���ݲ�����ʼ��LED1��GPIO

  GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);            //��LED1Ĭ��״̬����Ϊ����
}


void InitLED(void)
{
  ConfigLEDGPIO();  //����LED��GPIO
}

void LEDFlicker(u16 cnt)
{
  static u16 s_iCnt;  //���徲̬����s_iCnt��Ϊ������
  
  s_iCnt++; //�������ļ���ֵ��1
  
  if(s_iCnt >= cnt)   //�������ļ���ֵ����cnt
  { 
    s_iCnt = 0;       //���ü������ļ���ֵΪ0

    //LED1״̬ȡ����ʵ��LED0��˸
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13)));

  }
}
