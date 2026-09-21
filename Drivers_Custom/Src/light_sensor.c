#include "light_sensor.h"
#include "adc_utils.h"

static uint16_t rawValue;
static uint8_t brightness = BRIGHTNESS_MIN;
static uint32_t lastUpdate;
static uint8_t initialized = 0;

static uint8_t LightSensor_MapBrightness(uint16_t value);

void LightSensor_Init(void) {
  lastUpdate = HAL_GetTick() - UPDATE_DELAY_MS;
  LightSensor_Update();
}

void LightSensor_Update(void) {
  uint32_t now = HAL_GetTick();
  if (now - lastUpdate < UPDATE_DELAY_MS)
    return;

  lastUpdate = now;

  uint16_t newValue = ADC_ReadLight();

  if (!initialized) {
    rawValue = newValue;
    initialized = 1;
  } else {
    rawValue = (rawValue * 3 + newValue) / 4;
  }

  uint8_t target = LightSensor_MapBrightness(rawValue);
  int16_t diff = (int16_t)target - (int16_t)brightness;

  if (diff > BRIGHTNESS_HYSTERESIS || diff < -BRIGHTNESS_HYSTERESIS) {
    brightness = target;
  }
}

uint8_t LightSensor_GetBrightness(void) { return brightness; }

static uint8_t LightSensor_MapBrightness(uint16_t value) {
  if (value < ADC_MIN)
    value = ADC_MIN;
  if (value > ADC_MAX)
    value = ADC_MAX;
  return BRIGHTNESS_MIN +
         ((uint32_t)(value - ADC_MIN) * (BRIGHTNESS_MAX - BRIGHTNESS_MIN)) /
             (ADC_MAX - ADC_MIN);
}