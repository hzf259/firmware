#ifndef __SHT31_H
#define __SHT31_H

#include "stm32f10x.h"

typedef struct
{
    float temperature_c;
    float humidity_rh;
} SHT31_Data;

void SHT31_Init(void);
uint8_t SHT31_Read(SHT31_Data *out);
const char *SHT31_GetLastError(void);

#endif
