#include "main.h"
#include "codetab.h"

static uint8_t s_oled_address = OLED_ADDRESS;

static uint8_t OLED_GetAltAddress(uint8_t address)
{
	return (address == 0x78) ? 0x7A : 0x78;
}

static uint8_t OLED_WritePacket(uint8_t address, uint8_t control, uint8_t value)
{
	IIC_Start();
	Write_IIC_Byte(address);
	if (IIC_WaitAck())
	{
		IIC_Stop();
		return 0;
	}

	Write_IIC_Byte(control);
	if (IIC_WaitAck())
	{
		IIC_Stop();
		return 0;
	}

	Write_IIC_Byte(value);
	if (IIC_WaitAck())
	{
		IIC_Stop();
		return 0;
	}

	IIC_Stop();
	return 1;
}

static void OLED_DetectAddress(void)
{
	if (OLED_WritePacket(0x78, 0x00, 0xAE))
	{
		s_oled_address = 0x78;
	}
	else if (OLED_WritePacket(0x7A, 0x00, 0xAE))
	{
		s_oled_address = 0x7A;
	}
	else
	{
		s_oled_address = OLED_ADDRESS;
	}
}

void IIC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(OLED_SCL_RCC | OLED_SDA_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = OLED_SCL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(OLED_SCL_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = OLED_SDA_PIN;
	GPIO_Init(OLED_SDA_GPIO_PORT, &GPIO_InitStructure);

	IIC_Stop();
}

static void IIC_Delay(void)
{
	DelayUs(5);
}

void IIC_Start(void)
{
	OLED_SDA = 1;
	OLED_SCL = 1;
	IIC_Delay();
	OLED_SDA = 0;
	IIC_Delay();
	OLED_SCL = 0;
	IIC_Delay();
}

void IIC_Stop(void)
{
	OLED_SDA = 0;
	IIC_Delay();
	OLED_SCL = 1;
	IIC_Delay();
	OLED_SDA = 1;
	IIC_Delay();
}

uint8_t IIC_WaitAck(void)
{
	uint8_t re;

	OLED_SDA = 1;
	IIC_Delay();
	OLED_SCL = 1;
	IIC_Delay();
	re = OLED_IIC_SDA_READ() ? 1 : 0;
	OLED_SCL = 0;
	IIC_Delay();
	return re;
}

void Write_IIC_Byte(uint8_t _ucByte)
{
	uint8_t i;

	for (i = 0; i < 8; i++)
	{
		if (_ucByte & 0x80)
		{
			OLED_SDA = 1;
		}
		else
		{
			OLED_SDA = 0;
		}

		IIC_Delay();
		OLED_SCL = 1;
		IIC_Delay();
		OLED_SCL = 0;
		IIC_Delay();

		if (i == 7)
		{
			OLED_SDA = 1;
		}

		_ucByte <<= 1;
	}
}

void I2C_WriteByte(uint8_t addr, uint8_t data)
{
	IIC_Start();
	Write_IIC_Byte(addr);
	IIC_WaitAck();
	Write_IIC_Byte(data);
	IIC_WaitAck();
	IIC_Stop();
}

void WriteCmd(unsigned char I2C_Command)
{
	if (!OLED_WritePacket(s_oled_address, 0x00, I2C_Command))
	{
		uint8_t alt_address = OLED_GetAltAddress(s_oled_address);

		if (OLED_WritePacket(alt_address, 0x00, I2C_Command))
		{
			s_oled_address = alt_address;
		}
	}
}

void WriteDat(unsigned char I2C_Data)
{
	if (!OLED_WritePacket(s_oled_address, 0x40, I2C_Data))
	{
		uint8_t alt_address = OLED_GetAltAddress(s_oled_address);

		if (OLED_WritePacket(alt_address, 0x40, I2C_Data))
		{
			s_oled_address = alt_address;
		}
	}
}

void OLED_Init(void)
{
	DelayMs(100);
	IIC_GPIO_Config();
	OLED_DetectAddress();

	/* Vendor sequence for the 0.96-inch 4-pin SSD1315 I2C module. */
	WriteCmd(0xAE);
	WriteCmd(0x00);
	WriteCmd(0x10);
	WriteCmd(0x40);
	WriteCmd(0xB0);
	WriteCmd(0x81);
	WriteCmd(0xFF);
	WriteCmd(0xA1);
	WriteCmd(0xA6);
	WriteCmd(0xC8);
	WriteCmd(0xA8);
	WriteCmd(0x3F);
	WriteCmd(0xD3);
	WriteCmd(0x00);
	WriteCmd(0xD5);
	WriteCmd(0x80);
	WriteCmd(0xD9);
	WriteCmd(0xF1);
	WriteCmd(0xDA);
	WriteCmd(0x12);
	WriteCmd(0xDB);
	WriteCmd(0x40);
	WriteCmd(0x20);
	WriteCmd(0x02);
	WriteCmd(0x8D);
	WriteCmd(0x14);

	OLED_CLS();
	WriteCmd(0xAF);
}

void OLED_SetPos(unsigned char x, unsigned char y)
{
	WriteCmd(0xB0 + y);
	WriteCmd(((x & 0xF0) >> 4) | 0x10);
	WriteCmd(x & 0x0F);
}

void OLED_Fill(unsigned char fill_Data)
{
	unsigned char m, n;

	for (m = 0; m < 8; m++)
	{
		WriteCmd(0xB0 + m);
		WriteCmd(0x00);
		WriteCmd(0x10);
		for (n = 0; n < 128; n++)
		{
			WriteDat(fill_Data);
		}
	}
}

void OLED_CLS(void)
{
	OLED_Fill(0x00);
}

void OLED_TestFullOn(uint16_t hold_ms)
{
	OLED_Fill(0xFF);
	DelayMs(hold_ms);
	OLED_CLS();
}

void OLED_ON(void)
{
	WriteCmd(0x8D);
	WriteCmd(0x14);
	WriteCmd(0xAF);
}

void OLED_OFF(void)
{
	WriteCmd(0x8D);
	WriteCmd(0x10);
	WriteCmd(0xAE);
}

void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0, i = 0, j = 0;

	switch (TextSize)
	{
	case 1:
		while (ch[j] != '\0')
		{
			c = ch[j] - 32;
			if (x > 126)
			{
				x = 0;
				y++;
			}
			OLED_SetPos(x, y);
			for (i = 0; i < 6; i++)
			{
				WriteDat(F6x8[c][i]);
			}
			x += 6;
			j++;
		}
		break;

	case 2:
		while (ch[j] != '\0')
		{
			c = ch[j] - 32;
			if (x > 120)
			{
				x = 0;
				y++;
			}
			OLED_SetPos(x, y);
			for (i = 0; i < 8; i++)
			{
				WriteDat(F8X16[c * 16 + i]);
			}
			OLED_SetPos(x, y + 1);
			for (i = 0; i < 8; i++)
			{
				WriteDat(F8X16[c * 16 + i + 8]);
			}
			x += 8;
			j++;
		}
		break;
	}
}

void OLED_ShowStr_S(unsigned char x, unsigned char y, unsigned char ch[], unsigned char POS, unsigned char len, unsigned char TextSize)
{
	unsigned char c = 0, i = 0, j = 0;

	POS = POS - 1;
	switch (TextSize)
	{
	case 1:
		while (ch[j] != '\0')
		{
			c = ch[j] - 32;
			if (x > 126)
			{
				x = 0;
				y++;
			}
			OLED_SetPos(x, y);
			for (i = 0; i < 6; i++)
			{
				if ((j >= POS) & (j < (POS + len)))
				{
					WriteDat(~F6x8[c][i]);
				}
				else
				{
					WriteDat(F6x8[c][i]);
				}
			}
			x += 6;
			j++;
		}
		break;

	case 2:
		while (ch[j] != '\0')
		{
			c = ch[j] - 32;
			if (x > 120)
			{
				x = 0;
				y++;
			}
			OLED_SetPos(x, y);
			for (i = 0; i < 8; i++)
			{
				if ((j > POS - 1) & (j < (POS + len)))
				{
					WriteDat(~F8X16[c * 16 + i]);
				}
				else
				{
					WriteDat(F8X16[c * 16 + i]);
				}
			}
			OLED_SetPos(x, y + 1);
			for (i = 0; i < 8; i++)
			{
				if ((j >= POS) & (j < (POS + len)))
				{
					WriteDat(~F8X16[c * 16 + i + 8]);
				}
				else
				{
					WriteDat(F8X16[c * 16 + i + 8]);
				}
			}
			x += 8;
			j++;
		}
		break;
	}
}

void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm = 0;
	unsigned int adder = 32 * N;

	OLED_SetPos(x, y);
	for (wm = 0; wm < 16; wm++)
	{
		adder += 1;
	}

	OLED_SetPos(x, y + 1);
	for (wm = 0; wm < 16; wm++)
	{
		adder += 1;
	}
}

void OLED_ShowCC(unsigned char x, unsigned char y, unsigned char *s)
{
	u16 HZnum;
	u16 k, i, c;
	u8 wm = 0;
	u8 adder = 0;

	while (*s != 0)
	{
		if (*s <= 0x80)
		{
			c = *s - 32;
			OLED_SetPos(x, y);
			for (i = 0; i < 8; i++)
			{
				WriteDat(F8X16[c * 16 + i]);
			}
			OLED_SetPos(x, y + 1);
			for (i = 0; i < 8; i++)
			{
				WriteDat(F8X16[c * 16 + i + 8]);
			}
			x += 8;
			s += 1;
		}
		else
		{
			HZnum = sizeof(tfont16) / sizeof(typFNT_GB16);
			for (k = 0; k < HZnum; k++)
			{
				adder = 0;
				if ((tfont16[k].Index[0] == *(s)) && (tfont16[k].Index[1] == *(s + 1)))
				{
					OLED_SetPos(x, y);
					for (wm = 0; wm < 16; wm++)
					{
						WriteDat(tfont16[k].Msk[adder]);
						adder += 1;
					}
					OLED_SetPos(x, y + 1);
					for (wm = 0; wm < 16; wm++)
					{
						WriteDat(tfont16[k].Msk[adder]);
						adder += 1;
					}
					break;
				}
			}
			s += 2;
			x += 16;
		}
	}
}

void OLED_ShowCC_S(unsigned char x, unsigned char y, unsigned char *s, unsigned char POS, unsigned char len)
{
	u16 HZnum;
	u16 k, i, c;
	u8 wm = 0, j = 0;
	u8 adder = 0;

	POS = POS - 1;
	while (*s != 0)
	{
		if (*s <= 0x80)
		{
			c = *s - 32;
			if (x > 120)
			{
				x = 0;
				y++;
			}
			OLED_SetPos(x, y);
			for (i = 0; i < 8; i++)
			{
				if ((j > POS - 1) & (j < (POS + len)))
				{
					WriteDat(~F8X16[c * 16 + i]);
				}
				else
				{
					WriteDat(F8X16[c * 16 + i]);
				}
			}
			OLED_SetPos(x, y + 1);
			for (i = 0; i < 8; i++)
			{
				if ((j >= POS) & (j < (POS + len)))
				{
					WriteDat(~F8X16[c * 16 + i + 8]);
				}
				else
				{
					WriteDat(F8X16[c * 16 + i + 8]);
				}
			}
			x += 8;
			s += 1;
			j += 1;
		}
		else
		{
			HZnum = sizeof(tfont16) / sizeof(typFNT_GB16);
			for (k = 0; k < HZnum; k++)
			{
				adder = 0;
				if ((tfont16[k].Index[0] == *(s)) && (tfont16[k].Index[1] == *(s + 1)))
				{
					OLED_SetPos(x, y);
					for (wm = 0; wm < 16; wm++)
					{
						WriteDat(tfont16[k].Msk[adder]);
						adder += 1;
					}
					OLED_SetPos(x, y + 1);
					for (wm = 0; wm < 16; wm++)
					{
						WriteDat(tfont16[k].Msk[adder]);
						adder += 1;
					}
					break;
				}
			}
			s += 2;
			j += 2;
			x += 16;
		}
	}
}

void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
	unsigned int j = 0;
	unsigned char x, y;

	if (y1 % 8 == 0)
	{
		y = y1 / 8;
	}
	else
	{
		y = y1 / 8 + 1;
	}

	for (y = y0; y < y1; y++)
	{
		OLED_SetPos(x0, y);
		for (x = x0; x < x1; x++)
		{
			WriteDat(BMP[j++]);
		}
	}
}

void OLED_Set_Pos(u8 x, u8 y)
{
	WriteCmd(0xB0 + y);
	WriteCmd((((x + 2) & 0xF0) >> 4) | 0x10);
	WriteCmd((x + 2) & 0x0F);
}
