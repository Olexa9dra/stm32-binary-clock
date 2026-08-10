#include "ws2812.h"

extern TIM_HandleTypeDef htim1;

static uint8_t ledData[WS2812_LED_COUNT][3];
static uint16_t
    pwmData[WS2812_LED_COUNT * WS2812_LED_BITS + WS2812_RESET_SLOTS];
static volatile uint8_t dmaFinished = 0;
static uint8_t brightness = WS2812_BRIGHTNESS_MAX;

void WS2812_Init(void) { WS2812_Clear(); }

void WS2812_SetBrightness(uint8_t value) {
  if (value < WS2812_BRIGHTNESS_MIN)
    value = WS2812_BRIGHTNESS_MIN;
  if (value > WS2812_BRIGHTNESS_MAX)
    value = WS2812_BRIGHTNESS_MAX;

  brightness = value;
}

void WS2812_SetPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
  if (index >= WS2812_LED_COUNT)
    return;

  // WS2812 uses GRB order.
  ledData[index][0] = g;
  ledData[index][1] = r;
  ledData[index][2] = b;
}

void WS2812_Clear(void) {
  for (uint16_t i = 0; i < WS2812_LED_COUNT; i++) {
    ledData[i][0] = 0;
    ledData[i][1] = 0;
    ledData[i][2] = 0;
  }
}

void WS2812_Show(void) {
  uint32_t idx = 0;

  for (uint16_t led = 0; led < WS2812_LED_COUNT; led++) {
    for (uint8_t color = 0; color < 3; color++) {
      uint8_t value =
          ((uint16_t)ledData[led][color] * brightness) / WS2812_BRIGHTNESS_FULL;

      for (int8_t bit = 7; bit >= 0; bit--) {
        if (value & (1U << bit))
          pwmData[idx++] = WS2812_PWM_HIGH;
        else
          pwmData[idx++] = WS2812_PWM_LOW;
      }
    }
  }

  for (uint8_t i = 0; i < WS2812_RESET_SLOTS; i++)
    pwmData[idx++] = 0;

  dmaFinished = 0;

  HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)pwmData, idx);

  while (!dmaFinished)
    ;
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  if (htim == &htim1) {
    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
    dmaFinished = 1;
  }
}