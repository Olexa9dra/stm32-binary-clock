#ifndef __BATTERY_DRIVER_H
#define __BATTERY_DRIVER_H

#include "main.h"

HAL_StatusTypeDef BatteryDriver_ReadADC(uint16_t *value);

#endif