#ifndef __DS1302_H__
#define __DS1302_H__
#include <stm32f10x.h>
#define uchar unsigned char
#define uint unsigned int
/***DS1302时钟芯片****************************/
//CLK--->PC15
//IO---->PC14
//RES--->PC13
/****************************/

#define OUT   0x01
#define IN    0x00
//extern uchar label2[16];//时间存放数组
extern uchar year,mon,day,hour,min,sec;
//extern uchar Settime[6]={0x17,0x04,0x18,0x16,0x57,0x30}; //设置时间函数
#define DS1302_PORT   GPIOB

#define CLK_Reset_0  GPIOB->BRR=GPIO_Pin_14
#define CLK_Set_1    GPIOB->BSRR=GPIO_Pin_14

#define IO_Reset_0   GPIOB->BRR=GPIO_Pin_13
#define IO_Set_1     GPIOB->BSRR=GPIO_Pin_13

#define RES_Reset_0  GPIOB->BRR=GPIO_Pin_12
#define RES_Set_1    GPIOB->BSRR=GPIO_Pin_12

#define IO_Read   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)
/*************************/
#define Time_24_Hour    0x00    //24小时制
#define Time_Start     0x00    //开始走时
//DS1302寄存器操作指令定义时间地址
#define ds1302_sec_addr   0x80       //秒
#define ds1302_min_addr   0x82       //分
#define ds1302_hour_addr  0x84       //时
#define ds1302_day_addr   0x86       //日
#define ds1302_month_addr 0x88       //月
#define ds1302_year_addr  0x8c       //年
/********************/
void DS1302_GPIOInit(void);
void DS1302_IO_GPIO(uchar FLAG);//配置IO的方向
void DS1302_delay(u8 dd);
void DS1302_Write(uchar add,uchar dat);
uchar DS1302_Read(uchar add);
void DS1302_SetTime(uchar s_year,uchar s_mon,uchar s_day,uchar s_hour,uchar s_min,uchar s_sec);
void DS1302_OFF(void);
void DS1302_ON(void);
void DS1302_init(uchar s_year,uchar s_mon,uchar s_day,uchar s_hour,uchar s_min,uchar s_sec);
void DS1302_Readtime(void);
/*BCD码转十进制*/
uint8_t BCD_DEC(uint8_t BCD);
/*十进制转BCD*/
uint8_t DEC_BCD(uint8_t DEC);
#endif






