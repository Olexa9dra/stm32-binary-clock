#ifndef __ENVIRONMENT_H
#define __ENVIRONMENT_H

#include <stdint.h>

#define ENVIRONMENT_UPDATE_INTERVAL_MS 5000U // Prod: 60s, Debug: 5s

void Environment_Init(void);
void Environment_Update(void);
uint16_t Environment_GetDisplayValue(void);

#endif