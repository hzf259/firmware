# Legacy StdPeriph Reference

This folder is a copied Keil5 STM32F103C8 standard-library project, now trimmed into a cold-chain-oriented hardware base.

Open this file in Keil:

- `MDK-ARM/Project.uvprojx`

Cold-chain entry files:

- `USER/main.c`
- `USER/esp01_at.[ch]`
- `USER/sht31.[ch]`
- `USER/coldchain_board.h`

Base support files:

- `USER/usart2.[ch]`
- `USER/debug_uart.[ch]`
- `USER/delay.[ch]`
- `USER/stm32f10x_conf.h`
- `USER/stm32f10x_it.[ch]`

Mostly historical reference unless needed:

- `USER/OLED_I2C.[ch]`
- `USER/ds18b20.[ch]`
- `USER/adc.[ch]`
- `Gizwits/`
- `Utils/`

Recommended use:

1. Open `MDK-ARM/Project.uvprojx` in Keil5.
2. Update Wi-Fi/MQTT/device parameters in `USER/esp01_at.h` and `USER/coldchain_board.h`.
3. Build and download to your STM32F103C8 board.
