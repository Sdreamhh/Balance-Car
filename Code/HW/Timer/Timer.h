#ifndef _TIMER_H_
#define _TIMER_H_

#include "DataType.h"

void  InitTimer(void);      //初始化Timer模块

u8    Get2msFlag(void);     //获取2ms标志位的值
void  Clr2msFlag(void);     //清除2ms标志位

 
#endif
