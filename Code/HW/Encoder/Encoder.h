#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "DataType.h"
#include "stm32f10x_conf.h"



void InitEncoder(void);
i32 GetEncoderSpeed(TIM_TypeDef* TIMx);




#endif
