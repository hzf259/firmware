#ifndef __HC_RS04_H
#define __HC_RS04_H
//#include "stm32f10x.h"

/*设置中断优先级*/
void NVIC_Config(void);
/*初始化模块的GPIO以及初始化定时器TIM2*/
void CH_SR04_Init(void);
float Senor_Using(void);
#endif



