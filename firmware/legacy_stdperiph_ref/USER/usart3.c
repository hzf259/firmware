#include "main.h"	  

//USART3_TX   GPIOB.10
//USART3_RX	  GPIOB.11

u8  USART3_TX_BUF[USART3_MAX_TX_LEN]; 		//发送缓冲,最大USART3_MAX_TX_LEN-1字节
u8  USART3_RX_BUF[USART3_MAX_RX_LEN]; 		//接收缓冲,最大USART3_MAX_RX_LEN-1字节
u8	u3_cmd;

volatile u16 USART3_RX_STA=0;               //bit15:接收完成标志   bit14~0:接收到的有效字节数目 

void USART3_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//时钟设置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		
 
	//USARTx_TX 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//USARTx_RX 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);   

	//NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel                   = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;		 
	NVIC_Init(&NVIC_InitStructure);	 
	
	 //USARTx初始化 
	USART_InitStructure.USART_BaudRate            = bound; 
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits            = USART_StopBits_1; 
	USART_InitStructure.USART_Parity              = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; 

	//使能配置
	USART_Init(USART3, &USART_InitStructure);     //初始化串口
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//开启串口空闲IDEL中断
	USART_Cmd(USART3, ENABLE);                    //使能串口
	USART3_printf("USART3 OK...\r\n");
}


 /*发送一个字节数据*/
void UART3SendByte(unsigned char SendData)
{	   
		USART_SendData(USART3,SendData);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	    
}


void USART3_printf (char *fmt, ...)
{
	u16 i = 0;
	va_list arg_ptr;
	va_start(arg_ptr, fmt); 
	vsnprintf((char *)USART3_TX_BUF, USART3_MAX_TX_LEN+1, fmt, arg_ptr);
	va_end(arg_ptr);

	while ((i < USART3_MAX_TX_LEN) && USART3_TX_BUF[i])
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET); 
		USART_SendData(USART3, (u8) USART3_TX_BUF[i++]);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC)  == RESET);
	}
}
  
void USART3_Send_Array (u8 *array,u16 num)
{
	u16 i=0;
	while (i < num)
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET); 
		USART_SendData(USART3, (u8) array[i++]);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC)  == RESET);
	}
}

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)               // 空闲中断
	{
		USART_ReceiveData(USART3);                                      //清除空闲中断标志
		USART3_RX_STA_SET();
		//添加处理函数,最好主函数查询处理

	}	
	if(USART_GetFlagStatus(USART3,USART_FLAG_ORE) == SET)               // 检查 ORE 标志,防止开关总中断死机，放在接收中断前面
	{
		USART_ClearFlag(USART3,USART_FLAG_ORE);
		USART_ReceiveData(USART3);
	}
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)               //接收中断 
	{
	//	u8 res = USART_ReceiveData(USART3);	                            //读取接收到的数据	
		u3_cmd = USART_ReceiveData(USART3);
		gizPutData(&u3_cmd, 1);//数据写入到缓冲区
	}
}

void USART3_RX_STA_SET(void)
{
	if (USART3_RX_BUF[(USART3_RX_STA&0X7FFF)-2]=='\r') 
	{
		USART3_RX_BUF[(USART3_RX_STA&0X7FFF)-2]=0;		                //收到\r\n就删掉\r\n，添加\0
		USART3_RX_STA=USART3_RX_STA-2;                                  //删掉\r\n计数2
	}
	else                                                            
	{ 
		USART3_RX_BUF[(USART3_RX_STA&0X7FFF)]='\0';		                //没收到\r\n，直接添加\0
	}
	USART3_RX_STA|=0x8000;	                                            //标记接收完成了
}
 
