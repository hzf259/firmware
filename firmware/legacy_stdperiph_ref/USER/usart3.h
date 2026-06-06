#ifndef _USART3_H_
#define _USART3_H_
//#include "sys.h" 

#define     USART3_MAX_TX_LEN		    300   //最大发送缓存字节数，刨去\0 最大-1
#define     USART3_MAX_RX_LEN		    200   //最大接收缓存字节数，刨去\0 最大-1
extern volatile u16 USART3_RX_STA;            //接收状态标记
extern u8   USART3_TX_BUF[USART3_MAX_TX_LEN]; //发送缓冲区
extern u8   USART3_RX_BUF[USART3_MAX_RX_LEN]; //接收缓冲区    		             
extern u8	u3_cmd; 
void USART3_Init(u32 bound);
void USART3_printf (char *fmt, ...);
void USART3_Send_Array (u8 *array,u16 num);
void USART3_RX_STA_SET(void);

void UART3SendByte(unsigned char SendData);
#endif





