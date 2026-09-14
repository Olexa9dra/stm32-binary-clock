#include "led.h"

extern TIM_HandleTypeDef htim1;

static uint8_t ledData[LED_LED_COUNT][3];
static uint16_t pwmData[LED_LED_COUNT * LED_LED_BITS + LED_RESET_SLOTS];
static volatile uint8_t dmaFinished = 0;
static uint8_t brightness = LED_BRIGHTNESS_MAX;

void LED_Init(void) { LED_Clear(); }

void LED_SetBrightness(uint8_t value) {
  if (value < LED_BRIGHTNESS_MIN)
    value = LED_BRIGHTNESS_MIN;
  if (value > LED_BRIGHTNESS_MAX)
    value = LED_BRIGHTNESS_MAX;

  brightness = value;
}

void LED_SetPixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
  if (index >= LED_LED_COUNT)
    return;

  // LED uses GRB order.
  ledData[index][0] = g;
  ledData[index][1] = r;
  ledData[index][2] = b;
}

void LED_Clear(void) {
  for (uint16_t i = 0; i < LED_LED_COUNT; i++) {
    ledData[i][0] = 0;
    ledData[i][1] = 0;
    ledData[i][2] = 0;
  }
}

void LED_Show(void) {
  uint32_t idx = 0;

  HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
  __HAL_TIM_SET_COUNTER(&htim1, 0);

  for (uint16_t led = 0; led < LED_LED_COUNT; led++) {
    for (uint8_t color = 0; color < 3; color++) {

      uint8_t value =
          ((uint16_t)ledData[led][color] * brightness) / LED_BRIGHTNESS_FULL;

      for (int8_t bit = 7; bit >= 0; bit--)
        pwmData[idx++] = (value & (1U << bit)) ? LED_PWM_HIGH : LED_PWM_LOW;
    }
  }

  for (uint8_t i = 0; i < LED_RESET_SLOTS; i++)
    pwmData[idx++] = 0;

  dmaFinished = 0;

  if (HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)pwmData, idx) !=
      HAL_OK) {
    Error_Handler();
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM1) {
    HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_1);
    dmaFinished = 1;
  }
}