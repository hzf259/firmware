#ifndef __USART2_H
#define __USART2_H

#include "stm32f10x.h"

#define USART2_MAX_TX_LEN 500U
#define USART2_MAX_RX_LEN 1000U

extern u8 USART2_RX_BUF[USART2_MAX_RX_LEN];
extern u8 USART2_TX_BUF[USART2_MAX_TX_LEN];
extern volatile u16 USART2_RX_STA;

void usart2_init(u32 bound);
void u2_printf(char *fmt, ...);
void USART2_RX_STA_SET(void);
void USART2_ClearRxBuffer(void);
const char *USART2_GetRxBuffer(void);
void USART2_SendBuffer(const uint8_t *buf, uint16_t len);

#endif