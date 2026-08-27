#include "battery.h"
#include "battery_driver.h"

static uint16_t batteryAdcValue = 0;
static float batteryVoltage = 0.0f;
static uint8_t batteryPercentage = 0;
static uint32_t lastUpdate = 0;
static uint8_t initialized = 0;

void Battery_Init(void) {
  if (BatteryDriver_ReadADC(&batteryAdcValue) != HAL_OK)
    return;

  initialized = 1;
  batteryVoltage = (batteryAdcValue / BATTERY_ADC_MAX) * BATTERY_ADC_REFERENCE *
                   BATTERY_DIVIDER_RATIO;

  if (batteryVoltage <= BATTERY_MIN_VOLTAGE) {
    batteryPercentage = 0;
  } else if (batteryVoltage >= BATTERY_MAX_VOLTAGE) {
    batteryPercentage = 100;
  } else {
    batteryPercentage =
        (uint8_t)(((batteryVoltage - BATTERY_MIN_VOLTAGE) /
                   (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE)) *
                      100.0f +
                  0.5f);
  }
}

void Battery_Update(void) {
  uint32_t now = HAL_GetTick();
  if (now - lastUpdate < BATTERY_UPDATE_DELAY_MS)
    return;

  lastUpdate = now;
  uint16_t newAdcValue;
  if (BatteryDriver_ReadADC(&newAdcValue) != HAL_OK)
    return;

  if (!initialized) {
    batteryAdcValue = newAdcValue;
    initialized = 1;
  } else {
    batteryAdcValue = (batteryAdcValue * 3 + newAdcValue) / 4;
  }

  uint8_t newPercentage = batteryPercentage;
  batteryVoltage = (batteryAdcValue / BATTERY_ADC_MAX) * BATTERY_ADC_REFERENCE *
                   BATTERY_DIVIDER_RATIO;

  if (batteryVoltage <= BATTERY_MIN_VOLTAGE) {
    newPercentage = 0;
  } else if (batteryVoltage >= BATTERY_MAX_VOLTAGE) {
    newPercentage = 100;
  } else {
    newPercentage = (uint8_t)(((batteryVoltage - BATTERY_MIN_VOLTAGE) /
                               (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE)) *
                                  100.0f +
                              0.5f);
  }
  if (newPercentage > batteryPercentage) {
    if (newPercentage - batteryPercentage > BATTERY_PERCENTAGE_HYSTERESIS)
      batteryPercentage = newPercentage;
  } else {
    if (batteryPercentage - newPercentage > BATTERY_PERCENTAGE_HYSTERESIS)
      batteryPercentage = newPercentage;
  }
}

uint8_t Battery_GetPercentage(void) { return batteryPercentage; }

float Battery_GetVoltage(void) { return batteryVoltage; }

uint16_t Battery_GetDisplayValue(void) {
  uint8_t percentage = Battery_GetPercentage();
  uint8_t hundreds = percentage / 100;
  uint8_t tens = (percentage / 10) % 10;
  uint8_t ones = percentage % 10;
  return (ones << 12) | (tens << 8) | (hundreds << 4);
}
