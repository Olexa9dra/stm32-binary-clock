#ifndef __ENVIRONMENT_H
#define __ENVIRONMENT_H

#include "stm32f4xx_hal.h"

#define UPDATE_TIME 5000U // Prod: 60s, Debug: 5s

void Environment_Init(void);
void Environment_Update(void);
uint16_t Environment_GetDisplayValue(void);

#endif