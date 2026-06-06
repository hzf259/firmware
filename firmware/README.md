# Firmware Layout

This directory now contains two different firmware paths for the same `STM32F103C8T6` hardware.

## 1. `stm32f103c8t6_hal/`

This is the current cold-chain application code written in STM32 HAL style.

What it contains:

- `coldchain_app.[ch]`: app loop, buzzer, button, telemetry publish
- `esp8266_at.[ch]`: ESP8266 AT + MQTT wrapper
- `sht31.[ch]`: SHT31 driver
- `app_config.h`: Wi-Fi, MQTT, device parameters

What it does **not** contain:

- a complete CubeMX project
- a full Keil project that can be flashed directly
- startup files, interrupt files, or generated peripheral init code

## 2. `legacy_stdperiph_ref/`

This started as a reusable Keil5 + STM32 standard-peripheral-library reference project copied from your existing local project at `C:\Users\32824\Desktop\程序`.

Why it was copied here:

- you said CubeMX is not practical for you right now
- this old project already has a working Keil project structure
- it includes startup code, CMSIS, StdPeriph drivers, delay code, USART2 driver, and debug UART code

It has now been trimmed into a cold-chain-oriented Keil base.

Useful files inside this folder:

- `MDK-ARM/Project.uvprojx`: Keil project entry
- `USER/main.c`: cold-chain app loop
- `USER/esp01_at.[ch]`: ESP8266 AT + MQTT wrapper
- `USER/sht31.[ch]`: SHT31 software-I2C driver on `PB6/PB7`
- `USER/coldchain_board.h`: device ID and board-level constants
- `USER/delay.[ch]`: delay base
- `USER/usart2.[ch]`: ESP8266 serial base on `PA2/PA3`
- `USER/debug_uart.[ch]`: debug serial base on `PA9/PA10`
- `USER/system_stm32f10x.c`: system clock support
- `USER/stm32f10x_conf.h`: StdPeriph config
- `USER/stm32f10x_it.[ch]`: interrupt handlers
- `Libraries/`: CMSIS + STM32F10x standard peripheral library

Files that are mostly historical reference and not part of the cold-chain target:

- `USER/main.c`
- `USER/main.h`
- `USER/OLED_I2C.[ch]`
- `USER/ds18b20.[ch]`
- `USER/adc.[ch]`
- `Gizwits/`
- `Utils/`

## Recommended Direction

For your current hardware bring-up, the practical route is:

1. Open `legacy_stdperiph_ref/MDK-ARM/Project.uvprojx` in Keil5.
2. Verify download/debug works with your board and ST-Link.
3. Keep the StdPeriph project as the hardware base.
4. Gradually migrate the cold-chain logic from `stm32f103c8t6_hal/` into that base.

## Pin Notes

If you reuse the legacy project as the base, the natural wiring is:

- `USART2 (PA2/PA3)` -> ESP8266
- `USART1 (PA9/PA10)` -> debug log
- `PB6/PB7` -> SHT31
- buzzer and button can be reassigned in the StdPeriph app layer

This avoids conflict between ESP8266 and debug UART.

## Important Constraint

The code in `stm32f103c8t6_hal/` cannot be dropped directly into `legacy_stdperiph_ref/` because the driver model is different:

- `stm32f103c8t6_hal/` uses HAL APIs
- `legacy_stdperiph_ref/` uses the old STM32 standard peripheral library

So the reusable part is the project skeleton and low-level base, not a direct copy-paste compile.
