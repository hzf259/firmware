#ifndef __DEBUG_UART_H
#define __DEBUG_UART_H

#include "stm32f10x.h"

void DebugUart_Init(uint32_t baudrate);
void DebugUart_SendString(const char *text);

#endif
