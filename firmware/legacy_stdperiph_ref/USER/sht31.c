#include "coldchain_board.h"
#include "sht31.h"

#define SHT31_ADDR_7BIT_PRIMARY    0x44U
#define SHT31_ADDR_7BIT_SECONDARY  0x45U
#define SHT31_CMD_MEAS_HIGHREP 0x2400U

#define SHT31_SCL PBout(6)
#define SHT31_SDA PBout(7)

static void SHT31_BusDelay(void)
{
    DelayUs(5U);
}

static void SHT31_Start(void)
{
    SHT31_SDA = 1;
    SHT31_SCL = 1;
    SHT31_BusDelay();
    SHT31_SDA = 0;
    SHT31_BusDelay();
    SHT31_SCL = 0;
    SHT31_BusDelay();
}

static void SHT31_Stop(void)
{
    SHT31_SDA = 0;
    SHT31_BusDelay();
    SHT31_SCL = 1;
    SHT31_BusDelay();
    SHT31_SDA = 1;
    SHT31_BusDelay();
}

static uint8_t SHT31_WaitAck(void)
{
    uint8_t ack;

    SHT31_SDA = 1;
    SHT31_BusDelay();
    SHT31_SCL = 1;
    SHT31_BusDelay();
    ack = (uint8_t)GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);
    SHT31_SCL = 0;
    SHT31_BusDelay();
    return ack;
}

static void SHT31_WriteByte(uint8_t value)
{
    uint8_t i;

    for (i = 0U; i < 8U; i++)
    {
        SHT31_SDA = (value & 0x80U) ? 1 : 0;
        SHT31_BusDelay();
        SHT31_SCL = 1;
        SHT31_BusDelay();
        SHT31_SCL = 0;
        SHT31_BusDelay();
        value <<= 1;
    }

    SHT31_SDA = 1;
}

static uint8_t SHT31_ReadByte(uint8_t ack)
{
    uint8_t i;
    uint8_t value = 0U;

    SHT31_SDA = 1;

    for (i = 0U; i < 8U; i++)
    {
        value <<= 1;
        SHT31_SCL = 1;
        SHT31_BusDelay();
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) != Bit_RESET)
        {
            value |= 0x01U;
        }
        SHT31_SCL = 0;
        SHT31_BusDelay();
    }

    SHT31_SDA = (ack != 0U) ? 0 : 1;
    SHT31_BusDelay();
    SHT31_SCL = 1;
    SHT31_BusDelay();
    SHT31_SCL = 0;
    SHT31_BusDelay();
    SHT31_SDA = 1;

    return value;
}

static uint8_t SHT31_Crc8(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0xFFU;
    uint8_t i;
    uint8_t bit;

    for (i = 0U; i < len; i++)
    {
        crc ^= data[i];
        for (bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 0x80U) != 0U)
            {
                crc = (uint8_t)((crc << 1) ^ 0x31U);
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}

static uint8_t SHT31_ReadAtAddress(uint8_t addr_7bit, SHT31_Data *out)
{
    uint8_t rx[6];
    uint16_t raw_t;
    uint16_t raw_h;
    uint8_t write_addr = (uint8_t)((addr_7bit << 1) | 0U);
    uint8_t read_addr = (uint8_t)((addr_7bit << 1) | 1U);

    SHT31_Start();
    SHT31_WriteByte(write_addr);
    if (SHT31_WaitAck() != 0U)
    {
        SHT31_Stop();
        return 0U;
    }

    SHT31_WriteByte((uint8_t)(SHT31_CMD_MEAS_HIGHREP >> 8));
    if (SHT31_WaitAck() != 0U)
    {
        SHT31_Stop();
        return 0U;
    }

    SHT31_WriteByte((uint8_t)(SHT31_CMD_MEAS_HIGHREP & 0xFFU));
    if (SHT31_WaitAck() != 0U)
    {
        SHT31_Stop();
        return 0U;
    }
    SHT31_Stop();

    DelayMs(20U);

    SHT31_Start();
    SHT31_WriteByte(read_addr);
    if (SHT31_WaitAck() != 0U)
    {
        SHT31_Stop();
        return 0U;
    }

    rx[0] = SHT31_ReadByte(1U);
    rx[1] = SHT31_ReadByte(1U);
    rx[2] = SHT31_ReadByte(1U);
    rx[3] = SHT31_ReadByte(1U);
    rx[4] = SHT31_ReadByte(1U);
    rx[5] = SHT31_ReadByte(0U);
    SHT31_Stop();

    if ((SHT31_Crc8(&rx[0], 2U) != rx[2]) || (SHT31_Crc8(&rx[3], 2U) != rx[5]))
    {
        return 0U;
    }

    raw_t = (uint16_t)(((uint16_t)rx[0] << 8) | rx[1]);
    raw_h = (uint16_t)(((uint16_t)rx[3] << 8) | rx[4]);

    out->temperature_c = -45.0f + (175.0f * (float)raw_t / 65535.0f);
    out->humidity_rh = 100.0f * (float)raw_h / 65535.0f;

    return 1U;
}

void SHT31_Init(void)
{
    GPIO_InitTypeDef gpio_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    gpio_init.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &gpio_init);

    SHT31_SCL = 1;
    SHT31_SDA = 1;
}

uint8_t SHT31_Read(SHT31_Data *out)
{
    static uint8_t current_addr = SHT31_ADDR_7BIT_PRIMARY;
    uint8_t alternate_addr;

    if (out == 0)
    {
        return 0U;
    }

    if (SHT31_ReadAtAddress(current_addr, out) != 0U)
    {
        return 1U;
    }

    alternate_addr = (current_addr == SHT31_ADDR_7BIT_PRIMARY) ?
        SHT31_ADDR_7BIT_SECONDARY : SHT31_ADDR_7BIT_PRIMARY;
    if (SHT31_ReadAtAddress(alternate_addr, out) != 0U)
    {
        current_addr = alternate_addr;
        return 1U;
    }

    return 0U;
}
