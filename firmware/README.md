# Firmware Entry

Use only this Keil project:

- `legacy_stdperiph_ref/MDK-ARM/Project.uvprojx`

Current bring-up firmware:

- serial-only debug output on `USART1` (`PA9` -> USB-TTL RX)
- `SHT31` on hardware `I2C1` (`PB6/PB7`)
- no ESP8266, no Wi-Fi, no MQTT

Recommended workflow:

1. Open `legacy_stdperiph_ref/MDK-ARM/Project.uvprojx` in Keil.
2. Rebuild the project.
3. Download with ST-Link.
4. Read logs from `PA9` at `115200 8N1`.

If the board is running the latest firmware, boot logs start with:

- `Cold-chain node boot (serial-only, hw-i2c)`
- `SHT31 bus: I2C1 PB6/PB7`
