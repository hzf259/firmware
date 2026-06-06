# STM32 冷链终端固件

这个目录提供一套可直接改参数并编译烧录的 `STM32F103C8T6` 固件，适配当前项目的软件平台。

硬件方案：

- 主控：`STM32F103C8T6 (Blue Pill)`
- 传感器：`SHT31`
- 本地报警：`有源蜂鸣器模块`
- 联网：`ESP8266 + ESP-AT 固件`
- 交互：`2 个按键模块`
- 不使用 `OLED`
- 不使用 `LED`

## 功能

- 通过 `SHT31` 读取温湿度
- 超过阈值时本地蜂鸣器报警
- 通过 `ESP8266` 连接 Wi-Fi 和 MQTT
- 向当前项目后端的 MQTT 主题上报遥测数据
- 订阅 `coldchain/{device_id}/cmd`，接收后端告警命令
- 按键 1：静音蜂鸣器
- 按键 2：短按立即上报，长按强制重连 Wi-Fi/MQTT

## 重要前提

本固件默认 `ESP8266` 使用 `ESP-AT` 固件。

如果你买的是 `NodeMCU` 开发板，需要确认当前固件是 `ESP-AT`，且带有 MQTT AT 命令；当前这套 STM32 固件就是通过串口向 ESP8266 发送 AT 命令来完成联网和 MQTT 上报的。

## 接线

### SHT31

- `VCC -> 3.3V`
- `GND -> GND`
- `SCL -> PB6`
- `SDA -> PB7`

### 蜂鸣器模块

- `VCC -> 3.3V 或 5V`
- `GND -> GND`
- `IN -> PB12`

默认代码按“高电平触发蜂鸣器”写的；如果你买的是低电平触发模块，把 `include/app_config.h` 中的 `BUZZER_ACTIVE_HIGH` 改成 `false`。

### 按键模块

- 按键 1 输出脚 -> `PB13`
- 按键 2 输出脚 -> `PB14`
- 模块 `VCC -> 3.3V`
- 模块 `GND -> GND`

默认按“低电平触发按键模块”写的；如果你买的是高电平触发模块，把 `include/app_config.h` 中的 `BUTTON_ACTIVE_HIGH` 改成 `true`。

### ESP8266

- `STM32 PA2 (TX2) -> ESP8266 RX`
- `STM32 PA3 (RX2) -> ESP8266 TX`
- `STM32 GND -> ESP8266 GND`

供电建议：

- 如果你用的是 `NodeMCU`，建议单独用 USB 给它供电，再和 STM32 共地
- 如果你用的是 `ESP-01S`，一定要单独提供稳定 `3.3V`

## 参数修改

打开 [app_config.h](/Users/caihd/Desktop/ll/firmware/stm32_coldchain/include/app_config.h)，至少修改下面这些参数：

```c
#define DEVICE_ID "DEV001"
#define SHIPMENT_CODE "SHP001"

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define MQTT_HOST "192.168.1.100"
#define MQTT_PORT 1883
```

注意：

- `MQTT_HOST` 要填运行你当前项目 MQTT Broker 的那台机器的局域网 IP
- 不能填 `localhost`
- 当前项目默认 Broker 是 `1883`

## 平台侧主题

本固件会发布到：

```text
coldchain/{device_id}/telemetry
```

并订阅：

```text
coldchain/{device_id}/cmd
```

这和你当前仓库后端/模拟器使用的 MQTT 主题结构一致。

## 上传 JSON 示例

```json
{
  "device_id": "DEV001",
  "shipment_code": "SHP001",
  "temperature": 4.26,
  "humidity": 66.31,
  "alarm_local": false,
  "alarm_remote": false,
  "publish_reason": "periodic"
}
```

后端接收这类数据后，会把最新状态写入 MySQL，把时序数据写入 InfluxDB，并继续沿用当前仓库已有的告警链路。

## 编译和烧录

这是一个 `PlatformIO` 工程。

如果本机还没有 PlatformIO，可在 VS Code 安装 PlatformIO 插件后直接打开本目录。

常用步骤：

1. 打开 `firmware/stm32_coldchain`
2. 修改 `include/app_config.h`
3. 连接 `ST-Link`
4. 编译并烧录

默认 `platformio.ini` 使用：

- 板卡：`bluepill_f103c8`
- 框架：`Arduino`
- 上传方式：`stlink`

如果你不是用 `ST-Link`，再改 `platformio.ini`。

## 串口调试

调试串口使用：

- `PA9 -> USB-TTL RX`
- `PA10 -> USB-TTL TX`
- 波特率：`115200`

你可以在串口看到：

- 传感器读数
- Wi-Fi 连接状态
- MQTT 连接状态
- 上报结果
- 按键操作结果

## 按键逻辑

- `PB13` 静音键：
  - 按下后本地蜂鸣器静音 5 分钟
- `PB14` 功能键：
  - 短按：立即上报一次
  - 长按约 2.5 秒：强制重连 ESP8266 和 MQTT

## 已知限制

- 当前方案没有 RTC/NTP，同步时间由后端接收时间兜底
- 没有 GPS，所以不会上报位置
- 需要 `ESP8266` 端带 `ESP-AT + MQTT` 支持，否则不能直接工作
