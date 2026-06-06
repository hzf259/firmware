#ifndef __ESP01_AT_H
#define __ESP01_AT_H

#include "stm32f10x.h"

#define ESP01_UART_BAUD           115200U
#define ESP01_UART_BAUD_FALLBACK  9600U

#define ESP01_WIFI_SSID      "ChinaNet-DpMd"
#define ESP01_WIFI_PASSWORD  "s78xcenq"

#define ESP01_MQTT_HOST      "192.168.1.3"
#define ESP01_MQTT_PORT      1883U
#define ESP01_MQTT_USERNAME  ""
#define ESP01_MQTT_PASSWORD  ""
#define ESP01_MQTT_CLIENT_ID "stm32_dev001"

uint8_t ESP01_BasicSetup(void);
uint8_t ESP01_ConnectWiFi(void);
uint8_t ESP01_MQTTConnect(void);
uint8_t ESP01_MQTTSubscribeCommand(void);
uint8_t ESP01_MQTTPublishTelemetry(const char *payload);
void ESP01_Poll(void);
uint8_t ESP01_IsAlarmOn(void);
const char *ESP01_GetAlarmReason(void);

#endif
