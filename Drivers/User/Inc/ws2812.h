#ifndef _WS2812_H_
#define _WS2812_H_

#include "stm32f4xx_hal.h"

#define WS2012_RESET_TIME	80

#pragma pack(1)
typedef struct{
	uint8_t g;
	uint8_t r;
	uint8_t b;
}Color_t;

#define MAX_2812_COUNT 100

void Ws2812Set(Color_t *p, int len);

#endif
