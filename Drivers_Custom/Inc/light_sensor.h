#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <stdint.h>

#define ADC_MIN 800
#define ADC_MAX 2500
#define BRIGHTNESS_MIN 5
#define BRIGHTNESS_MAX 40
#define UPDATE_DELAY_MS 100
#define ADC_SAMPLES 16
#define BRIGHTNESS_HYSTERESIS 10

void LightSensor_Init(void);
void LightSensor_Update(void);
uint16_t LightSensor_GetRawValue(void);
uint8_t LightSensor_GetBrightness(void);

#endif