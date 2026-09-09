#include "adc_utils.h"

extern ADC_HandleTypeDef hadc1;

static uint16_t adc_values[2];
static volatile uint8_t adc_done = 0;
static uint8_t adc_running = 0;
static uint32_t last_update = 0;

static void ADC_UpdateIfNeeded(void);

uint16_t ADC_ReadLight(void) {
  ADC_UpdateIfNeeded();
  return adc_values[0];
}

uint16_t ADC_ReadBattery(void) {
  ADC_UpdateIfNeeded();
  return adc_values[1];
}

static void ADC_UpdateIfNeeded(void) {
  if (adc_running)
    return;

  uint32_t now = HAL_GetTick();
  if ((now - last_update) < ADC_UPDATE_INTERVAL_MS)
    return;

  adc_done = 0;

  if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_values, 2) == HAL_OK) {
    adc_running = 1;
    last_update = now;
  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  if (hadc->Instance == ADC1) {
    adc_done = 1;
    adc_running = 0;
  }
}