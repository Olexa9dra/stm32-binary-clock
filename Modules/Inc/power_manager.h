#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include "main.h"
#include <stdint.h>

#define POWER_MANAGER_IDLE_TIMEOUT_MS 60000U

typedef enum { POWER_STATE_ACTIVE = 0, POWER_STATE_SLEEPING } PowerState;

void PowerManager_Init(void);
void PowerManager_Update(void);
void PowerManager_ResetIdleTimer(void);
void PowerManager_Sleep(void);
void PowerManager_Wake(void);
PowerState PowerManager_GetState(void);
uint8_t PowerManager_IsSleeping(void);

#endif