#ifndef LED_H
#define LED_H

#include "main.h"
#include <stdint.h>

#define LED_LED_COUNT 16U
#define LED_LED_BITS 24U
#define LED_RESET_SLOTS 50U
#define LED_PWM_HIGH 13U
#define LED_PWM_LOW 6U
#define LED_BRIGHTNESS_MIN 8U
#define LED_BRIGHTNESS_MAX 40U
#define LED_BRIGHTNESS_FULL 100U

void LED_Init(void);
void LED_SetPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void LED_SetBrightness(uint8_t value);
void LED_Clear(void);
void LED_Show(void);

#endif