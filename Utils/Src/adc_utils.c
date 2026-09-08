#include "adc_utils.h"

extern ADC_HandleTypeDef hadc1;

static uint16_t adc_values[2];
static volatile uint8_t adc_done = 0;
static uint32_t last_update = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  if (hadc->Instance == ADC1) {
    adc_done = 1;
  }
}

static void ADC_UpdateIfNeeded(void) {
  uint32_t now = HAL_GetTick();
  if ((now - last_update) < ADC_UPDATE_INTERVAL_MS)
    return;

  adc_done = 0;
  if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_values, 2) != HAL_OK)
    return;

  uint32_t start = HAL_GetTick();

  while (!adc_done) {
    if ((HAL_GetTick() - start) >= ADC_DMA_TIMEOUT_MS) {
      HAL_ADC_Stop_DMA(&hadc1);
      return;
    }
  }

  last_update = HAL_GetTick();
}

uint16_t ADC_ReadLight(void) {
  ADC_UpdateIfNeeded();
  return adc_values[0];
}

uint16_t ADC_ReadBattery(void) {
  ADC_UpdateIfNeeded();
  return adc_values[1];
}