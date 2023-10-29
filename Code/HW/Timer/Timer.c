#include "Timer.h"
#include "stm32f10x_tim.h"

static  u8  s_i2msFlag  = FALSE;    //将2ms标志位的值设置为FALSE

static  void  ConfigTimer1(u16 arr, u16 psc);  //配置TIM2




static  void ConfigTimer1(u16 arr, u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//TIM_TimeBaseStructure用于存放定时器的参数
  NVIC_InitTypeDef NVIC_InitStructure;           //NVIC_InitStructure用于存放NVIC的参数

  //使能RCC相关时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  //使能TIM2的时钟
  
  //配置TIM1
  TIM_TimeBaseStructure.TIM_Period        = arr;  //设置自动重装载值
  TIM_TimeBaseStructure.TIM_Prescaler     = psc;  //设置预分频器值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //设置时钟分割：tDTS = tCK_INT
  TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; //设置向上计数模式
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);               //根据参数初始化定时器
 
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);                    //使能定时器的更新中断

  //配置NVIC
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;               //中断通道号
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     //设置抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;            //设置子优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //使能中断
  NVIC_Init(&NVIC_InitStructure);                               //根据参数初始化NVIC
                                                                
  TIM_Cmd(TIM1, ENABLE);                                        //使能定时器  
}

void TIM1_UP_IRQHandler(void)  
{  
  static  u16 s_iCnt2 = 0;                            //定义一个静态变量s_iCnt2作为2ms计数器

  if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)     //判断定时器更新中断是否发生
  {
    TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);     //清除定时器更新中断标志
  }

  s_iCnt2++;            //2ms计数器的计数值加1
                                
  if(s_iCnt2 >= 1)      //2ms计数器的计数值大于或等于2
  {                                                   
    s_iCnt2 = 0;        //重置2ms计数器的计数值为0
    s_i2msFlag = TRUE;  //将2ms标志位的值设置为TRUE 
  }
}





void InitTimer(void)
{
  ConfigTimer1(999, 71);  //72MHz/(71+1)=1MHz，由0计数到999为1ms
}

u8  Get2msFlag(void)
{
  return(s_i2msFlag);     //返回2ms标志位的值
}

void  Clr2msFlag(void)
{
  s_i2msFlag = FALSE;     //将2ms标志位的值设置为FALSE 
}

