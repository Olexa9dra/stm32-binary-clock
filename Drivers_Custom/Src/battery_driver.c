#include "battery_driver.h"
#include "adc_utils.h"

HAL_StatusTypeDef BatteryDriver_ReadADC(uint16_t *value) {
  if (value == NULL)
    return HAL_ERROR;
  *value = ADC_ReadBattery();
  return HAL_OK;
}