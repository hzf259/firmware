#ifndef __OLED_I2C_H
#define __OLED_I2C_H

//#include "stm32f10x.h"

#define OLED_ADDRESS    0x78 // 4-pin SSD1315 module write address, 7-bit address = 0x3C

// Modify these pin definitions if SCL/SDA are wired to different GPIOs.
#define OLED_SCL PBout(6)
#define OLED_SDA PBout(7)

#define OLED_SCL_GPIO_PORT GPIOB
#define OLED_SCL_RCC       RCC_APB2Periph_GPIOB
#define OLED_SCL_PIN       GPIO_Pin_6

#define OLED_SDA_GPIO_PORT GPIOB
#define OLED_SDA_RCC       RCC_APB2Periph_GPIOB
#define OLED_SDA_PIN       GPIO_Pin_7

#define OLED_IIC_SDA_READ()  GPIO_ReadInputDataBit(OLED_SDA_GPIO_PORT, OLED_SDA_PIN)

void IIC_GPIO_Config(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_WaitAck(void);
void Write_IIC_Byte(uint8_t _ucByte);
void I2C_WriteByte(uint8_t addr,uint8_t data);
void WriteCmd(unsigned char I2C_Command);
void WriteDat(unsigned char I2C_Data);
void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_CLS(void);
void OLED_TestFullOn(uint16_t hold_ms);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowStr_S(unsigned char x, unsigned char y, unsigned char ch[], unsigned char POS,unsigned char len,unsigned char TextSize);
void OLED_ShowCC_S(unsigned char x, unsigned char y, unsigned char *s ,unsigned char POS,unsigned char len);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);
void OLED_ShowCC(unsigned char x, unsigned char y, unsigned char *s);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
void OLED_Set_Pos(u8 x, u8 y);

#endif
