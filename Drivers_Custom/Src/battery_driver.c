#include "battery_driver.h"

extern ADC_HandleTypeDef hadc1;

HAL_StatusTypeDef BatteryDriver_ReadADC(uint16_t *value) {
  ADC_ChannelConfTypeDef config = {0};

  config.Channel = ADC_CHANNEL_6;
  config.Rank = ADC_REGULAR_RANK_1;
  config.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

  if (HAL_ADC_ConfigChannel(&hadc1, &config) != HAL_OK)
    return HAL_ERROR;

  if (HAL_ADC_Start(&hadc1) != HAL_OK)
    return HAL_ERROR;

  if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) != HAL_OK) {
    HAL_ADC_Stop(&hadc1);
    return HAL_ERROR;
  }

  *value = HAL_ADC_GetValue(&hadc1);

  HAL_ADC_Stop(&hadc1);

  return HAL_OK;
}