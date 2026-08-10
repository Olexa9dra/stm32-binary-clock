#ifndef WS2812_H
#define WS2812_H

#include "main.h"
#include <stdint.h>

#define WS2812_LED_COUNT 16U
#define WS2812_LED_BITS 24U
#define WS2812_RESET_SLOTS 50U
#define WS2812_PWM_HIGH 13U
#define WS2812_PWM_LOW 6U
#define WS2812_BRIGHTNESS_MIN 8U
#define WS2812_BRIGHTNESS_MAX 40U
#define WS2812_BRIGHTNESS_FULL 100U

void WS2812_Init(void);
void WS2812_SetPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void WS2812_SetBrightness(uint8_t value);
void WS2812_Clear(void);
void WS2812_Show(void);

#endif