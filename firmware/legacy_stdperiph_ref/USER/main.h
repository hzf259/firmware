#ifndef __MAIN_H
#define	__MAIN_H

#include <stdio.h>
#include "stm32f10x.h"
#include "GPIOLIKE51.h"
#include "delay.h"
#include "stdarg.h"	 	  	 
#include "string.h"	
#include "timer.h"
#include "OLED_I2C.h"
#include "flash.h"
#include "USART.h"
#include "usart2.h"
//#include "usart3.h"
#include "adc.h"
#include "gizwits_product.h" 
#include "ds18b20.h"
//#include "dht11.h" 


//#include "gizwits_product.h"
//#include "gizwits_protocol.h"
//#include <stdint.h>
//#include <stdbool.h>
//#include <stdlib.h>

#define  U8  u8
#define  U16  u16

#define KEY1  PCin(13)
#define KEY2  PCin(14)
#define KEY3  PCin(15)
#define KEY4  PAin(0)


#define bee_out  PBout(12)
#define water_pump_out  PAout(15)

//#define HONGWAI PAin(12)//红外感应
//#define beep_io PAout(8)//蜂鸣器

//#define JDQ_MINJIN PBout(15)//门禁
//#define JDQ_1 PBout(14)   //继电器1
//#define USB_OUT PBout(13)//USB灯
//#define ZWD PBout(12) //紫外灯

//#define CHUANG_RED  PBout(6)
//#define CHUANG_GREEN  PBout(5)
//#define MEN_RED  PBout(3)
//#define MEN_GREEN  PAout(15)

extern u8 b_1s,eer_f,smart_config;
extern u32 set_code[21];//设置参数



extern u8 b_1s,eer_f,water_pump_f,medicinal_f,heat_f,smart_config;;
extern u32 set_code[21];//设置参数
extern u8 run_mod;//工作模式


extern u16 User_ID,integral;
extern u8 mod;//显示模式

extern float PH,PH_temp;//PH 变量
extern u16 temperature;	//温度存储变量			
extern  float temperature_temp;	//温度存储临时变量
extern float turbidity,turbidity_temp;//PH值浊度值存储变量
extern float TDS_DAT;
#endif



