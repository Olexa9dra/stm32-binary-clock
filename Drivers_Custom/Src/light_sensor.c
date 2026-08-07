#include "light_sensor.h"
#include "main.h"

extern ADC_HandleTypeDef hadc1;

static uint16_t rawValue = 0;
static uint8_t brightness = 255;
static uint32_t lastUpdate = 0;

static uint8_t LightSensor_MapBrightness(uint16_t value);

void LightSensor_Init(void) { LightSensor_Update(); }

void LightSensor_Update(void) {
  if (HAL_GetTick() - lastUpdate < 500)
    return;

  lastUpdate = HAL_GetTick();

  uint32_t sum = 0;

  for (uint8_t i = 0; i < 16; i++) {
    HAL_ADC_Start(&hadc1);

    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
      sum += HAL_ADC_GetValue(&hadc1);

    HAL_ADC_Stop(&hadc1);
  }

  uint16_t newValue = sum / 16;

  // Smooth ADC reading
  rawValue = (rawValue * 3 + newValue) / 4;

  uint8_t target = LightSensor_MapBrightness(rawValue);

  brightness = target;

  // int16_t diff = (int16_t)target - (int16_t)brightness;

  // // Ignore small changes (hysteresis)
  // if (diff > 10) {
  //   brightness += 3;
  // } else if (diff < -10) {
  //   brightness -= 3;
  // }

  // Clamp to 0-255 range
  if (brightness < 8)
    brightness = 8;
  else if (brightness > 60)
    brightness = 255;
}

uint16_t LightSensor_GetRawValue(void) { return rawValue; }

uint8_t LightSensor_GetBrightness(void) { return brightness; }

static uint8_t LightSensor_MapBrightness(uint16_t value) {
  const uint16_t adcMin = 800;  // brightest
  const uint16_t adcMax = 2500; // darkest

  const uint8_t minBrightness = 5;
  const uint8_t maxBrightness = 40;

  if (value < adcMin)
    value = adcMin;

  if (value > adcMax)
    value = adcMax;

  return maxBrightness -
         ((uint32_t)(value - adcMin) * (maxBrightness - minBrightness)) /
             (adcMax - adcMin);
}