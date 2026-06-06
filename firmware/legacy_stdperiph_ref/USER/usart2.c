#include "main.h"

u8 USART2_RX_BUF[USART2_MAX_RX_LEN];
u8 USART2_TX_BUF[USART2_MAX_TX_LEN];
volatile u16 USART2_RX_STA = 0U;

void usart2_init(u32 bound)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    USART_DeInit(USART2);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    USART_Cmd(USART2, ENABLE);

    USART2_ClearRxBuffer();
}

void USART2_IRQHandler(void)
{
    if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) == SET)
    {
        USART_ClearFlag(USART2, USART_FLAG_ORE);
        (void)USART_ReceiveData(USART2);
    }

    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        uint16_t count = (uint16_t)(USART2_RX_STA & 0x7FFFU);
        uint8_t data = (uint8_t)USART_ReceiveData(USART2);

        if (count < (USART2_MAX_RX_LEN - 1U))
        {
            USART2_RX_BUF[count] = data;
            count++;
            USART2_RX_BUF[count] = '\0';
            USART2_RX_STA = count;
        }

        if (data == '\n')
        {
            USART2_RX_STA_SET();
        }
    }

    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        (void)USART2->SR;
        (void)USART2->DR;
        USART2_RX_STA_SET();
    }
}

void u2_printf(char *fmt, ...)
{
    u16 i = 0U;
    va_list arg_ptr;

    va_start(arg_ptr, fmt);
    vsnprintf((char *)USART2_TX_BUF, USART2_MAX_TX_LEN, fmt, arg_ptr);
    va_end(arg_ptr);

    while ((i < USART2_MAX_TX_LEN) && (USART2_TX_BUF[i] != 0U))
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
        {
        }
        USART_SendData(USART2, USART2_TX_BUF[i++]);
    }

    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
    {
    }
}

void USART2_SendBuffer(const uint8_t *buf, uint16_t len)
{
    uint16_t i;

    for (i = 0U; i < len; i++)
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
        {
        }
        USART_SendData(USART2, buf[i]);
    }

    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
    {
    }
}

void USART2_ClearRxBuffer(void)
{
    uint16_t i;

    for (i = 0U; i < USART2_MAX_RX_LEN; i++)
    {
        USART2_RX_BUF[i] = 0U;
    }

    USART2_RX_STA = 0U;
}

const char *USART2_GetRxBuffer(void)
{
    return (const char *)USART2_RX_BUF;
}

void USART2_RX_STA_SET(void)
{
    USART2_RX_STA |= 0x8000U;
}