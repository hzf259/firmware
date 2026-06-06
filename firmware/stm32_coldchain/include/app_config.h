#pragma once

// =========================
// 基础设备标识
// =========================
#define DEVICE_ID "DEV001"
#define SHIPMENT_CODE "SHP001"

// =========================
// 温湿度阈值
// 超过阈值时本地蜂鸣器报警，同时上报平台
// =========================
#define TEMP_MIN_C 0.0f
#define TEMP_MAX_C 8.0f
#define HUM_MIN_RH 30.0f
#define HUM_MAX_RH 85.0f

// =========================
// 周期上报配置
// =========================
#define SAMPLE_INTERVAL_MS 5000UL
#define TELEMETRY_INTERVAL_MS 15000UL
#define WIFI_RECONNECT_INTERVAL_MS 10000UL
#define MQTT_RECONNECT_INTERVAL_MS 10000UL
#define SENSOR_WARMUP_MS 2000UL
#define SILENCE_DURATION_MS 300000UL
#define BUTTON_LONG_PRESS_MS 2500UL

// =========================
// SHT31 传感器地址
// 常见为 0x44，某些模块可切换到 0x45
// =========================
#define SHT31_I2C_ADDR 0x44

// =========================
// ESP8266 ESP-AT 配置
// 重要：这里假定 ESP8266 已烧录 ESP-AT 固件，并支持 MQTT AT 命令
// 如果使用 NodeMCU，请先确认或改刷 ESP-AT 固件
// =========================
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define MQTT_HOST "192.168.1.100"
#define MQTT_PORT 1883
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""
#define MQTT_CLIENT_ID "stm32-dev001"
#define ESP_AT_BAUDRATE 115200

// =========================
// 引脚定义（Blue Pill）
// =========================
// I2C1: PB6=SCL, PB7=SDA
// Serial1: PA10(RX), PA9(TX)   调试口
// Serial2: PA3(RX), PA2(TX)    ESP8266
#define BUZZER_PIN PB12
#define SILENCE_BUTTON_PIN PB13
#define ACTION_BUTTON_PIN PB14

// 按键模块默认低电平触发；若你买的是高电平触发模块，改成 true
#define BUTTON_ACTIVE_HIGH false

// 蜂鸣器默认高电平触发；若你买的是低电平触发模块，改成 false
#define BUZZER_ACTIVE_HIGH true

