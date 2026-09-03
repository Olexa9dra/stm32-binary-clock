#ifndef __BATTERY_H
#define __BATTERY_H

#include <stdint.h>

#define BATTERY_UPDATE_DELAY_MS 1000U
#define BATTERY_ADC_MAX 4095.0f
#define BATTERY_ADC_REFERENCE 3.3f
#define BATTERY_DIVIDER_RATIO 2.0f
#define BATTERY_MIN_VOLTAGE 3.0f
#define BATTERY_MAX_VOLTAGE 4.2f
#define BATTERY_PERCENTAGE_HYSTERESIS 2U

void Battery_Init(void);
void Battery_Update(void);
uint16_t Battery_GetDisplayValue(void);

#endif