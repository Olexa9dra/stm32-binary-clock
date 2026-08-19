#include "light_sensor.h"
#include "main.h"

extern ADC_HandleTypeDef hadc1;

static uint16_t rawValue;
static uint8_t brightness = BRIGHTNESS_MIN;
static uint32_t lastUpdate;

static uint8_t LightSensor_MapBrightness(uint16_t value);

void LightSensor_Init(void) { LightSensor_Update(); }

void LightSensor_Update(void) {
  uint32_t now = HAL_GetTick();

  if (now - lastUpdate < UPDATE_DELAY_MS)
    return;

  lastUpdate = now;
  uint32_t sum = 0;

  for (uint8_t i = 0; i < ADC_SAMPLES; i++) {
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
      sum += HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
  }

  uint16_t newValue = sum / ADC_SAMPLES;
  // Smooth ADC reading.
  rawValue = (rawValue * 3 + newValue) / 4;
  uint8_t target = LightSensor_MapBrightness(rawValue);
  int16_t diff = (int16_t)target - (int16_t)brightness;

  if (diff > BRIGHTNESS_HYSTERESIS || diff < -BRIGHTNESS_HYSTERESIS) {
    brightness = target;
  }
}

uint16_t LightSensor_GetRawValue(void) { return rawValue; }

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