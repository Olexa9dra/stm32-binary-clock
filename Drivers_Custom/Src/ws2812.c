#include "ws2812.h"

#define LED_BITS 24
#define RESET_SLOTS 50

#define PWM_HIGH 13
#define PWM_LOW 6

static TIM_HandleTypeDef *timer;

static uint8_t ledData[WS2812_LED_COUNT][3];
static uint16_t pwmData[WS2812_LED_COUNT * LED_BITS + RESET_SLOTS];

static volatile uint8_t dmaFinished = 0;
static uint8_t brightness = 255;

void WS2812_Init(TIM_HandleTypeDef *htim) {
  timer = htim;
  WS2812_Clear();
}

void WS2812_SetBrightness(uint8_t value) { brightness = value; }

void WS2812_SetPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
  if (index >= WS2812_LED_COUNT)
    return;

  ledData[index][0] = g;
  ledData[index][1] = r;
  ledData[index][2] = b;
}

void WS2812_Clear(void) {
  for (int i = 0; i < WS2812_LED_COUNT; i++) {
    ledData[i][0] = 0;
    ledData[i][1] = 0;
    ledData[i][2] = 0;
  }
}

void WS2812_Show(void) {
  uint32_t idx = 0;

  for (int led = 0; led < WS2812_LED_COUNT; led++) {
    for (int color = 0; color < 3; color++) {

      uint8_t value = ((uint16_t)ledData[led][color] * brightness) / 255;

      for (int bit = 7; bit >= 0; bit--) {
        if (value & (1 << bit))
          pwmData[idx++] = PWM_HIGH;
        else
          pwmData[idx++] = PWM_LOW;
      }
    }
  }

  for (int i = 0; i < RESET_SLOTS; i++)
    pwmData[idx++] = 0;

  dmaFinished = 0;

  HAL_TIM_PWM_Start_DMA(timer, TIM_CHANNEL_1, (uint32_t *)pwmData, idx);

  while (!dmaFinished)
    ;
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  if (htim == timer) {
    HAL_TIM_PWM_Stop_DMA(timer, TIM_CHANNEL_1);
    dmaFinished = 1;
  }
}