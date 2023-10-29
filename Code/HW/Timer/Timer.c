#include "Timer.h"
#include "stm32f10x_tim.h"

static  u8  s_i2msFlag  = FALSE;    //��2ms��־λ��ֵ����ΪFALSE

static  void  ConfigTimer1(u16 arr, u16 psc);  //����TIM2




static  void ConfigTimer1(u16 arr, u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//TIM_TimeBaseStructure���ڴ�Ŷ�ʱ���Ĳ���
  NVIC_InitTypeDef NVIC_InitStructure;           //NVIC_InitStructure���ڴ��NVIC�Ĳ���

  //ʹ��RCC���ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  //ʹ��TIM2��ʱ��
  
  //����TIM1
  TIM_TimeBaseStructure.TIM_Period        = arr;  //�����Զ���װ��ֵ
  TIM_TimeBaseStructure.TIM_Prescaler     = psc;  //����Ԥ��Ƶ��ֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //����ʱ�ӷָtDTS = tCK_INT
  TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; //�������ϼ���ģʽ
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);               //���ݲ�����ʼ����ʱ��
 
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);                    //ʹ�ܶ�ʱ���ĸ����ж�

  //����NVIC
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;               //�ж�ͨ����
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     //������ռ���ȼ�
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;            //���������ȼ�
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //ʹ���ж�
  NVIC_Init(&NVIC_InitStructure);                               //���ݲ�����ʼ��NVIC
                                                                
  TIM_Cmd(TIM1, ENABLE);                                        //ʹ�ܶ�ʱ��  
}

void TIM1_UP_IRQHandler(void)  
{  
  static  u16 s_iCnt2 = 0;                            //����һ����̬����s_iCnt2��Ϊ2ms������

  if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)     //�ж϶�ʱ�������ж��Ƿ���
  {
    TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);     //�����ʱ�������жϱ�־
  }

  s_iCnt2++;            //2ms�������ļ���ֵ��1
                                
  if(s_iCnt2 >= 1)      //2ms�������ļ���ֵ���ڻ����2
  {                                                   
    s_iCnt2 = 0;        //����2ms�������ļ���ֵΪ0
    s_i2msFlag = TRUE;  //��2ms��־λ��ֵ����ΪTRUE 
  }
}





void InitTimer(void)
{
  ConfigTimer1(999, 71);  //72MHz/(71+1)=1MHz����0������999Ϊ1ms
}

u8  Get2msFlag(void)
{
  return(s_i2msFlag);     //����2ms��־λ��ֵ
}

void  Clr2msFlag(void)
{
  s_i2msFlag = FALSE;     //��2ms��־λ��ֵ����ΪFALSE 
}

