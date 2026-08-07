#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <stdint.h>

void LightSensor_Init(void);
void LightSensor_Update(void);

uint16_t LightSensor_GetRawValue(void);
uint8_t LightSensor_GetBrightness(void);

#endif