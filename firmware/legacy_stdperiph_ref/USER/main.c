#include "coldchain_board.h"
#include "debug_uart.h"
#include "esp01_at.h"
#include "sht31.h"

#define DEBUG_UART_BAUD        115200U
#define TELEMETRY_INTERVAL_MS  5000U
#define ESP_RETRY_MS           5000U
#define HEARTBEAT_MS           500U
#define LOOP_DELAY_MS          20U
#define BUTTON_DEBOUNCE_MS     30U
#define ALARM_AUTO_CLEAR_MS    30000U
#define MOCK_TEMP_C            4.50f
#define MOCK_HUMIDITY_RH       65.00f

static uint8_t g_esp_ready = 0U;
static uint8_t g_wifi_ready = 0U;
static uint8_t g_mqtt_ready = 0U;
static uint8_t g_subscribed = 0U;
static uint8_t g_muted = 0U;
static uint8_t g_last_button_pressed = 0U;
static uint32_t g_last_button_edge_ms = 0U;
static uint32_t g_alarm_started_ms = 0U;

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

static uint8_t Button_IsPressed(void)
{
    return (uint8_t)(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_RESET);
}

static void Buzzer_Set(uint8_t on)
{
    if (on != 0U)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_12);
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_12);
    }
}

static void Handle_Button(uint32_t now_ms)
{
    uint8_t pressed = Button_IsPressed();

    if ((pressed != g_last_button_pressed) && ((now_ms - g_last_button_edge_ms) >= BUTTON_DEBOUNCE_MS))
    {
        g_last_button_edge_ms = now_ms;
        g_last_button_pressed = pressed;

        if ((pressed != 0U) && (ESP01_IsAlarmOn() != 0U))
        {
            g_muted = 1U;
            DebugUart_SendString("Local alarm muted by button\r\n");
        }
    }
}

static void Update_Buzzer(uint32_t now_ms)
{
    uint8_t alarm_on = ESP01_IsAlarmOn();

    if ((alarm_on != 0U) && (g_alarm_started_ms == 0U))
    {
        g_alarm_started_ms = now_ms;
    }

    if (alarm_on == 0U)
    {
        g_alarm_started_ms = 0U;
        g_muted = 0U;
    }
    else if ((g_alarm_started_ms != 0U) && ((now_ms - g_alarm_started_ms) > ALARM_AUTO_CLEAR_MS))
    {
        g_muted = 1U;
    }

    Buzzer_Set((uint8_t)((alarm_on != 0U) && (g_muted == 0U)));
}

static void Reset_MQTT_State(void)
{
    g_mqtt_ready = 0U;
    g_subscribed = 0U;
}

static void Try_ConnectStack(void)
{
    if (g_esp_ready == 0U)
    {
        g_esp_ready = ESP01_BasicSetup();
        if (g_esp_ready == 0U)
        {
            return;
        }
    }

    if (g_wifi_ready == 0U)
    {
        g_wifi_ready = ESP01_ConnectWiFi();
        if (g_wifi_ready == 0U)
        {
            return;
        }
    }

    if (g_mqtt_ready == 0U)
    {
        g_mqtt_ready = ESP01_MQTTConnect();
        if (g_mqtt_ready == 0U)
        {
            return;
        }
    }

    if (g_subscribed == 0U)
    {
        g_subscribed = ESP01_MQTTSubscribeCommand();
        if (g_subscribed == 0U)
        {
            Reset_MQTT_State();
            return;
        }

        DebugUart_SendString("Cold-chain MQTT command topic subscribed\r\n");
    }
}

static uint8_t Publish_Telemetry(void)
{
    char payload[384];
    SHT31_Data data;
    float temperature_c;
    float humidity_rh;
    uint8_t sensor_ok = 1U;

    if (SHT31_Read(&data) == 0U)
    {
        sensor_ok = 0U;
        temperature_c = MOCK_TEMP_C;
        humidity_rh = MOCK_HUMIDITY_RH;
        DebugUart_SendString("SHT31 read failed, publish mock telemetry\r\n");
    }
    else
    {
        temperature_c = data.temperature_c;
        humidity_rh = data.humidity_rh;
    }

    snprintf(
        payload,
        sizeof(payload),
        "{\"device_id\":\"%s\",\"shipment_code\":\"%s\",\"ts\":\"\","
        "\"temperature\":%.2f,\"humidity\":%.2f,"
        "\"gps\":{\"lat\":0,\"lon\":0},\"battery\":100.0,\"rssi\":-60}",
        COLDCHAIN_DEVICE_ID,
        COLDCHAIN_SHIPMENT_CODE,
        temperature_c,
        humidity_rh
    );

    if (ESP01_MQTTPublishTelemetry(payload) == 0U)
    {
        DebugUart_SendString("Telemetry publish failed\r\n");
        Reset_MQTT_State();
        return 0U;
    }

    {
        char log_line[96];

        snprintf(
            log_line,
            sizeof(log_line),
            (sensor_ok != 0U) ? "Telemetry ok T=%.2fC H=%.2f%%\r\n" : "Mock telemetry ok T=%.2fC H=%.2f%%\r\n",
            temperature_c,
            humidity_rh
        );
        DebugUart_SendString(log_line);
    }

    return 1U;
}

int main(void)
{
    uint32_t app_ms = 0U;
    uint32_t last_retry_ms = 0U;
    uint32_t last_publish_ms = 0U;
    uint32_t last_heartbeat_ms = 0U;

    SystemInit();
    DelayInit();
    Board_Init();
    SHT31_Init();
    DebugUart_Init(DEBUG_UART_BAUD);
    usart2_init(ESP01_UART_BAUD);

    DelayMs(200U);
    DebugUart_SendString("\r\nCold-chain node boot\r\n");

    while (1)
    {
        ESP01_Poll();
        Handle_Button(app_ms);
        Update_Buzzer(app_ms);

        if ((app_ms - last_heartbeat_ms) >= HEARTBEAT_MS)
        {
            last_heartbeat_ms = app_ms;
            Heartbeat_Toggle();
        }

        if ((app_ms - last_retry_ms) >= ESP_RETRY_MS)
        {
            last_retry_ms = app_ms;
            Try_ConnectStack();
        }

        if ((g_esp_ready != 0U) &&
            (g_wifi_ready != 0U) &&
            (g_mqtt_ready != 0U) &&
            (g_subscribed != 0U) &&
            ((app_ms - last_publish_ms) >= TELEMETRY_INTERVAL_MS))
        {
            last_publish_ms = app_ms;
            (void)Publish_Telemetry();
        }

        DelayMs(LOOP_DELAY_MS);
        app_ms += LOOP_DELAY_MS;
    }
}
