#include "LED.h"
#include "stm32f10x_conf.h"
#include "stdio.h"
#include "Encoder.h"

static  void  ConfigLEDGPIO(void);  //配置LED的GPIO

static  void  ConfigLEDGPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  //GPIO_InitStructure用于存放GPIO的参数
                                                                     
  //使能RCC相关时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //使能GPIOC的时钟
                
	//PC13
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;           //设置引脚
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //设置I/O输出速度
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;     //设置模式
  GPIO_Init(GPIOC, &GPIO_InitStructure);                //根据参数初始化LED1的GPIO

  GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);            //将LED1默认状态设置为点亮
}


void InitLED(void)
{
  ConfigLEDGPIO();  //配置LED的GPIO
}

void LEDFlicker(u16 cnt)
{
  static u16 s_iCnt;  //定义静态变量s_iCnt作为计数器
  
  s_iCnt++; //计数器的计数值加1
  
  if(s_iCnt >= cnt)   //计数器的计数值大于cnt
  { 
    s_iCnt = 0;       //重置计数器的计数值为0

    //LED1状态取反，实现LED0闪烁
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13)));

  }
}
