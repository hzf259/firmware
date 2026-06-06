

//说明：

// 引脚A1输出频率=系统频率72M/（1+1） /  (1799+1) = 20KHz

//占空比=1350 / (1799+1)*100% = 75%

#include "main.h"
//**********************************************************************************
void GPIO_CFG()                                                                               //PWM输出引脚A1函数
{
 GPIO_InitTypeDef GPIO_InitStructure;                                             //声明GPIO_InitStructure结构变量
 // 原版创作，引用请指明出处 https://www.cnblogs.com/beiyhs/p/12312888.html 北有寒山
 /* 使能GPIOA时钟 */
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //使能AHB预分频器到端口A的开关
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);   //引脚复用 进行重映射时需要开启AFIO 时钟

/* 配置LED相应引脚PA1*/
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;                                  //指定脚1输出
GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;                   //指定模式为功能复用推挽输出
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;              //设定端口最快输出50MHz
GPIO_Init(GPIOA, &GPIO_InitStructure);                                         //按以上参数设置A口
}

//*********************************************************************************
void TIM2_PWM_Init(uint16_t period, uint16_t prescaler,uint16_t Pulse)
{
	u32 Pulse_temp;
Pulse_temp = (period*Pulse)/100;
Pulse = Pulse_temp;	
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;   //声明TIM_TimeBaseStructure结构变量
TIM_OCInitTypeDef TIM_OCInitStructure;                   //声明TIM_OCInitStructure结构变量

RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //使能AHB预分频器到TIM2
TIM_TimeBaseStructure.TIM_Period = period;                                //设置TIM2自动重装周期值
TIM_TimeBaseStructure.TIM_Prescaler = prescaler;                      //设置TIM2预分频值
TIM_TimeBaseStructure.TIM_ClockDivision = 0;                             //设置时钟分割
TIM_TimeBaseStructure.TIM_CounterMode =TIM_CounterMode_Up; //向上计数
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);                        //按以上参数初始化TIM2

TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;           //PWM2模式
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
TIM_OCInitStructure.TIM_Pulse =Pulse;                                            //通道占空比系数
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        //输出低
TIM_OC2Init(TIM2,&TIM_OCInitStructure);                                      //按以上参数初始化TIM2
TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);               //使能预装载寄存器

TIM_OC3Init(TIM2,&TIM_OCInitStructure);                                      //按以上参数初始化TIM2
TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);               //使能预装载寄存器
TIM_Cmd(TIM2, ENABLE);                                                               //使能TIM2
}

