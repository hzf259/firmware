#include "coldchain_board.h"
#include "debug_uart.h"
#include "sht31.h"

#define DEBUG_UART_BAUD        115200U
#define SENSOR_REPORT_MS       5000U
#define HEARTBEAT_MS           500U
#define LOOP_DELAY_MS          20U

static void Board_Init(void)
{
    GPIO_InitTypeDef gpio_init;

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,
        ENABLE
    );

    gpio_init.GPIO_Pin = GPIO_Pin_13;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &gpio_init);
    GPIO_SetBits(GPIOC, GPIO_Pin_13);

    gpio_init.GPIO_Pin = GPIO_Pin_12;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &gpio_init);
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);

    gpio_init.GPIO_Pin = GPIO_Pin_0;
    gpio_init.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &gpio_init);
}

static void Heartbeat_Toggle(void)
{
    if (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET)
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    }
    else
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
    }
}

static void Report_SensorReading(void)
{
    char log_line[96];
    SHT31_Data data;

    if (SHT31_Read(&data) == 0U)
    {
        DebugUart_SendString("SHT31 read failed\r\n");
        return;
    }

    snprintf(
        log_line,
        sizeof(log_line),
        "T=%.2fC H=%.2f%%\r\n",
        data.temperature_c,
        data.humidity_rh
    );
    DebugUart_SendString(log_line);
}

int main(void)
{
    uint32_t app_ms = 0U;
    uint32_t last_report_ms = 0U;
    uint32_t last_heartbeat_ms = 0U;

    SystemInit();
    DelayInit();
    Board_Init();
    SHT31_Init();
    DebugUart_Init(DEBUG_UART_BAUD);

    DelayMs(200U);
    DebugUart_SendString("\r\nCold-chain node boot (serial-only)\r\n");

    while (1)
    {
        if ((app_ms - last_heartbeat_ms) >= HEARTBEAT_MS)
        {
            last_heartbeat_ms = app_ms;
            Heartbeat_Toggle();
        }

        if ((app_ms - last_report_ms) >= SENSOR_REPORT_MS)
        {
            last_report_ms = app_ms;
            Report_SensorReading();
        }

        DelayMs(LOOP_DELAY_MS);
        app_ms += LOOP_DELAY_MS;
    }
}
