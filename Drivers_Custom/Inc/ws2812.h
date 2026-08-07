#ifndef WS2812_H
#define WS2812_H

#include "main.h"
#include <stdint.h>

#define WS2812_LED_COUNT 16

void WS2812_Init(TIM_HandleTypeDef *htim);
void WS2812_SetPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void WS2812_Clear(void);
void WS2812_Show(void);

#endif