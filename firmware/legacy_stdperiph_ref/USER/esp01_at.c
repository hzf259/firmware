#include "coldchain_board.h"
#include "debug_uart.h"
#include "esp01_at.h"

#include <stdarg.h>

static uint8_t g_alarm_on = 0U;
static char g_alarm_reason[32];

static void ESP01_DebugLogf(const char *fmt, ...)
{
    char log_line[96];
    va_list args;

    va_start(args, fmt);
    vsnprintf(log_line, sizeof(log_line), fmt, args);
    va_end(args);

    DebugUart_SendString(log_line);
}

static const char *ESP01_GetRxBuffer(void)
{
    const char *rx = USART2_GetRxBuffer();

    return (rx != 0) ? rx : "";
}

static void ESP01_LogRawReply(void)
{
    const char *rx = ESP01_GetRxBuffer();

    if (rx[0] != '\0')
    {
        DebugUart_SendString("ESP01 raw reply: ");
        DebugUart_SendString(rx);
        DebugUart_SendString("\r\n");
    }
}

static uint8_t ESP01_ConfigMissing(const char *value)
{
    return (uint8_t)((value == 0) || (strncmp(value, "YOUR_", 5) == 0));
}

static uint8_t ESP01_WaitReply(const char *expect, uint32_t timeout_ms)
{
    while (timeout_ms > 0U)
    {
        const char *rx = ESP01_GetRxBuffer();

        if ((expect != 0) && (strstr(rx, expect) != 0))
        {
            return 1U;
        }

        if ((strstr(rx, "ERROR") != 0) ||
            (strstr(rx, "FAIL") != 0) ||
            (strstr(rx, "busy") != 0))
        {
            return 0U;
        }

        DelayMs(1U);
        timeout_ms--;
    }

    return 0U;
}

static uint8_t ESP01_SendCommand(const char *cmd, const char *expect, uint32_t timeout_ms)
{
    USART2_ClearRxBuffer();
    u2_printf("%s\r\n", cmd);
    return ESP01_WaitReply(expect, timeout_ms);
}

static void ESP01_EscapeString(const char *src, char *dst, uint16_t dst_size)
{
    uint16_t si = 0U;
    uint16_t di = 0U;

    if (dst_size == 0U)
    {
        return;
    }

    while ((src[si] != '\0') && (di < (uint16_t)(dst_size - 1U)))
    {
        if (((src[si] == '"') || (src[si] == '\\')) && (di < (uint16_t)(dst_size - 2U)))
        {
            dst[di++] = '\\';
        }

        dst[di++] = src[si++];
    }

    dst[di] = '\0';
}

static void ESP01_ParseAlarmReason(const char *line)
{
    const char *reason = strstr(line, "\"reason\":\"");
    uint16_t i = 0U;

    if (reason == 0)
    {
        g_alarm_reason[0] = '\0';
        return;
    }

    reason += strlen("\"reason\":\"");

    while ((reason[i] != '\0') &&
           (reason[i] != '"') &&
           (i < (sizeof(g_alarm_reason) - 1U)))
    {
        g_alarm_reason[i] = reason[i];
        i++;
    }

    g_alarm_reason[i] = '\0';
}

static void ESP01_ParseAlarmPayload(const char *line)
{
    if (strstr(line, "\"type\":\"ALARM\"") == 0)
    {
        return;
    }

    if (strstr(line, "\"state\":\"ON\"") != 0)
    {
        g_alarm_on = 1U;
        DebugUart_SendString("Alarm command ON received\r\n");
    }
    else if (strstr(line, "\"state\":\"OFF\"") != 0)
    {
        g_alarm_on = 0U;
        DebugUart_SendString("Alarm command OFF received\r\n");
    }

    ESP01_ParseAlarmReason(line);
}

uint8_t ESP01_BasicSetup(void)
{
    static const uint32_t baud_candidates[] = {
        ESP01_UART_BAUD,
        ESP01_UART_BAUD_FALLBACK
    };
    uint32_t i;

    for (i = 0U; i < (sizeof(baud_candidates) / sizeof(baud_candidates[0])); i++)
    {
        if ((i > 0U) && (baud_candidates[i] == baud_candidates[0]))
        {
            continue;
        }

        usart2_init(baud_candidates[i]);
        ESP01_DebugLogf("ESP01 try baud %lu\r\n", (unsigned long)baud_candidates[i]);

        if (ESP01_SendCommand("AT", "OK", 1000U) != 0U)
        {
            (void)ESP01_SendCommand("ATE0", "OK", 1000U);
            (void)ESP01_SendCommand("AT+CWMODE=1", "OK", 1000U);
            (void)ESP01_SendCommand("AT+CWAUTOCONN=1", "OK", 1000U);
            ESP01_DebugLogf("ESP01 AT ready at %lu\r\n", (unsigned long)baud_candidates[i]);
            return 1U;
        }

        ESP01_LogRawReply();
    }

    DebugUart_SendString("ESP01 no AT response. Check baud and wiring.\r\n");
    return 0U;
}

uint8_t ESP01_ConnectWiFi(void)
{
    char cmd[160];

    if ((ESP01_ConfigMissing(ESP01_WIFI_SSID) != 0U) || (ESP01_ConfigMissing(ESP01_WIFI_PASSWORD) != 0U))
    {
        DebugUart_SendString("Set ESP01_WIFI_SSID and ESP01_WIFI_PASSWORD first\r\n");
        return 0U;
    }

    snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", ESP01_WIFI_SSID, ESP01_WIFI_PASSWORD);
    if (ESP01_SendCommand(cmd, "OK", 20000U) == 0U)
    {
        DebugUart_SendString("ESP01 WiFi connect failed\r\n");
        return 0U;
    }

    DebugUart_SendString("ESP01 WiFi connected\r\n");
    return 1U;
}

uint8_t ESP01_MQTTConnect(void)
{
    char cmd[256];

    if (ESP01_ConfigMissing(ESP01_MQTT_HOST) != 0U)
    {
        DebugUart_SendString("Set ESP01_MQTT_HOST first\r\n");
        return 0U;
    }

    snprintf(
        cmd,
        sizeof(cmd),
        "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"",
        ESP01_MQTT_CLIENT_ID,
        ESP01_MQTT_USERNAME,
        ESP01_MQTT_PASSWORD
    );
    if (ESP01_SendCommand(cmd, "OK", 3000U) == 0U)
    {
        DebugUart_SendString("ESP01 MQTT user config failed\r\n");
        return 0U;
    }

    snprintf(
        cmd,
        sizeof(cmd),
        "AT+MQTTCONN=0,\"%s\",%u,0",
        ESP01_MQTT_HOST,
        (unsigned int)ESP01_MQTT_PORT
    );
    if (ESP01_SendCommand(cmd, "OK", 8000U) == 0U)
    {
        DebugUart_SendString("ESP01 MQTT connect failed\r\n");
        return 0U;
    }

    DebugUart_SendString("ESP01 MQTT connected\r\n");
    return 1U;
}

uint8_t ESP01_MQTTSubscribeCommand(void)
{
    char topic[96];
    char cmd[160];

    snprintf(topic, sizeof(topic), "coldchain/%s/cmd", COLDCHAIN_DEVICE_ID);
    snprintf(cmd, sizeof(cmd), "AT+MQTTSUB=0,\"%s\",1", topic);

    if (ESP01_SendCommand(cmd, "OK", 3000U) == 0U)
    {
        DebugUart_SendString("ESP01 MQTT subscribe failed\r\n");
        return 0U;
    }

    return 1U;
}

uint8_t ESP01_MQTTPublishTelemetry(const char *payload)
{
    char topic[96];
    char cmd[512];
    char escaped[384];

    if (payload == 0)
    {
        return 0U;
    }

    snprintf(topic, sizeof(topic), "coldchain/%s/telemetry", COLDCHAIN_DEVICE_ID);
    ESP01_EscapeString(payload, escaped, sizeof(escaped));
    snprintf(cmd, sizeof(cmd), "AT+MQTTPUB=0,\"%s\",\"%s\",1,0", topic, escaped);

    return ESP01_SendCommand(cmd, "OK", 5000U);
}

void ESP01_Poll(void)
{
    const char *rx = ESP01_GetRxBuffer();

    if (rx[0] == '\0')
    {
        return;
    }

    if (strstr(rx, "\"type\":\"ALARM\"") != 0)
    {
        ESP01_ParseAlarmPayload(rx);
        USART2_ClearRxBuffer();
    }
}

uint8_t ESP01_IsAlarmOn(void)
{
    return g_alarm_on;
}

const char *ESP01_GetAlarmReason(void)
{
    return g_alarm_reason;
}
