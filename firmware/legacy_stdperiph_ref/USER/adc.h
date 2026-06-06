#ifndef __adc_H
#define __adc_H 
  
#define ADC1_DR_Address ((uint32_t)0x4001244C)//定义硬件ADC1的物理地址



void ADC1_DMA_Config(void);	//adc初始化函数



extern  __IO uint16_t ADCConvertedValue[4];//没一个通道的ad值存储的变量
#endif











