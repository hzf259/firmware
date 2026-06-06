#include "coldchain_board.h"
#include "sht31.h"

#define SHT31_ADDR_7BIT_PRIMARY    0x44U
#define SHT31_ADDR_7BIT_SECONDARY  0x45U
#define SHT31_CMD_MEAS_HIGHREP_MSB 0x24U
#define SHT31_CMD_MEAS_HIGHREP_LSB 0x00U
#define SHT31_I2C_TIMEOUT          100000U

static char g_sht31_last_error[96] = "not started";

static void SHT31_SetError(const char *text)
{
    snprintf(g_sht31_last_error, sizeof(g_sht31_last_error), "%s", text);
}

static void SHT31_ResetBus(void)
{
    I2C_GenerateSTOP(I2C1, ENABLE);
    I2C_SoftwareResetCmd(I2C1, ENABLE);
    DelayUs(10U);
    I2C_SoftwareResetCmd(I2C1, DISABLE);
    I2C_Cmd(I2C1, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE);
}

static uint8_t SHT31_WaitEvent(uint32_t event, const char *error_text)
{
    uint32_t timeout = SHT31_I2C_TIMEOUT;

    while (I2C_CheckEvent(I2C1, event) != SUCCESS)
    {
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET)
        {
            I2C_ClearFlag(I2C1, I2C_FLAG_AF);
            SHT31_SetError(error_text);
            return 0U;
        }

        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BERR) == SET)
        {
            I2C_ClearFlag(I2C1, I2C_FLAG_BERR);
            SHT31_SetError("i2c bus error");
            return 0U;
        }

        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_ARLO) == SET)
        {
            I2C_ClearFlag(I2C1, I2C_FLAG_ARLO);
            SHT31_SetError("i2c arbitration lost");
            return 0U;
        }

        if (timeout-- == 0U)
        {
            SHT31_SetError(error_text);
            return 0U;
        }
    }

    return 1U;
}

static uint8_t SHT31_WaitFlagSet(uint32_t flag, const char *error_text)
{
    uint32_t timeout = SHT31_I2C_TIMEOUT;

    while (I2C_GetFlagStatus(I2C1, flag) == RESET)
    {
        if (timeout-- == 0U)
        {
            SHT31_SetError(error_text);
            return 0U;
        }
    }

    return 1U;
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

static uint8_t SHT31_SendMeasureCommand(uint8_t addr_7bit)
{
    I2C_GenerateSTART(I2C1, ENABLE);
    if (SHT31_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT, "start timeout") == 0U)
    {
        return 0U;
    }

    I2C_Send7bitAddress(I2C1, (uint8_t)(addr_7bit << 1), I2C_Direction_Transmitter);
    if (SHT31_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, "write address NACK") == 0U)
    {
        return 0U;
    }

    I2C_SendData(I2C1, SHT31_CMD_MEAS_HIGHREP_MSB);
    if (SHT31_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED, "command MSB timeout") == 0U)
    {
        return 0U;
    }

    I2C_SendData(I2C1, SHT31_CMD_MEAS_HIGHREP_LSB);
    if (SHT31_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED, "command LSB timeout") == 0U)
    {
        return 0U;
    }

    I2C_GenerateSTOP(I2C1, ENABLE);
    return 1U;
}

static uint8_t SHT31_ReadPayload(uint8_t addr_7bit, uint8_t *rx)
{
    uint8_t remaining = 6U;
    uint8_t index = 0U;

    I2C_AcknowledgeConfig(I2C1, ENABLE);
    I2C_GenerateSTART(I2C1, ENABLE);
    if (SHT31_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT, "restart timeout") == 0U)
    {
        return 0U;
    }

    I2C_Send7bitAddress(I2C1, (uint8_t)(addr_7bit << 1), I2C_Direction_Receiver);
    if (SHT31_WaitEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, "read address NACK") == 0U)
    {
        return 0U;
    }

    while (remaining > 3U)
    {
        if (SHT31_WaitEvent(I2C_EVENT_MASTER_BYTE_RECEIVED, "read timeout") == 0U)
        {
            return 0U;
        }
        rx[index++] = I2C_ReceiveData(I2C1);
        remaining--;
    }

    if (SHT31_WaitFlagSet(I2C_FLAG_BTF, "final bytes timeout") == 0U)
    {
        return 0U;
    }

    I2C_AcknowledgeConfig(I2C1, DISABLE);
    rx[index++] = I2C_ReceiveData(I2C1);
    I2C_GenerateSTOP(I2C1, ENABLE);
    rx[index++] = I2C_ReceiveData(I2C1);

    if (SHT31_WaitEvent(I2C_EVENT_MASTER_BYTE_RECEIVED, "last byte timeout") == 0U)
    {
        return 0U;
    }
    rx[index++] = I2C_ReceiveData(I2C1);
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    return 1U;
}

static uint8_t SHT31_ReadAtAddress(uint8_t addr_7bit, SHT31_Data *out)
{
    uint8_t rx[6];
    uint16_t raw_t;
    uint16_t raw_h;

    if (SHT31_SendMeasureCommand(addr_7bit) == 0U)
    {
        return 0U;
    }

    DelayMs(20U);

    if (SHT31_ReadPayload(addr_7bit, rx) == 0U)
    {
        return 0U;
    }

    if ((SHT31_Crc8(&rx[0], 2U) != rx[2]) || (SHT31_Crc8(&rx[3], 2U) != rx[5]))
    {
        SHT31_SetError("crc mismatch");
        return 0U;
    }

    raw_t = (uint16_t)(((uint16_t)rx[0] << 8) | rx[1]);
    raw_h = (uint16_t)(((uint16_t)rx[3] << 8) | rx[4]);

    out->temperature_c = -45.0f + (175.0f * (float)raw_t / 65535.0f);
    out->humidity_rh = 100.0f * (float)raw_h / 65535.0f;

    SHT31_SetError("ok");
    return 1U;
}

void SHT31_Init(void)
{
    GPIO_InitTypeDef gpio_init;
    I2C_InitTypeDef i2c_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    gpio_init.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &gpio_init);

    I2C_DeInit(I2C1);
    I2C_StructInit(&i2c_init);
    i2c_init.I2C_ClockSpeed = 100000U;
    i2c_init.I2C_Mode = I2C_Mode_I2C;
    i2c_init.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c_init.I2C_OwnAddress1 = 0U;
    i2c_init.I2C_Ack = I2C_Ack_Enable;
    i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &i2c_init);
    I2C_Cmd(I2C1, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    SHT31_SetError("initialized");
}

uint8_t SHT31_Read(SHT31_Data *out)
{
    static uint8_t current_addr = SHT31_ADDR_7BIT_PRIMARY;
    uint8_t alternate_addr;
    char first_error[48];
    char second_error[48];

    if (out == 0)
    {
        SHT31_SetError("null output pointer");
        return 0U;
    }

    if (SHT31_ReadAtAddress(current_addr, out) != 0U)
    {
        return 1U;
    }
    snprintf(first_error, sizeof(first_error), "0x%02X %s", current_addr, g_sht31_last_error);
    SHT31_ResetBus();

    alternate_addr = (current_addr == SHT31_ADDR_7BIT_PRIMARY) ?
        SHT31_ADDR_7BIT_SECONDARY : SHT31_ADDR_7BIT_PRIMARY;
    if (SHT31_ReadAtAddress(alternate_addr, out) != 0U)
    {
        current_addr = alternate_addr;
        return 1U;
    }
    snprintf(second_error, sizeof(second_error), "0x%02X %s", alternate_addr, g_sht31_last_error);
    SHT31_ResetBus();

    snprintf(g_sht31_last_error, sizeof(g_sht31_last_error), "%s; %s", first_error, second_error);
    return 0U;
}

const char *SHT31_GetLastError(void)
{
    return g_sht31_last_error;
}
