#ifndef __POWER_MANAGER_H
#define __POWER_MANAGER_H

#include "stm32f1xx_hal.h"

typedef enum {
  POWER_STATE_ACTIVE,
  POWER_STATE_SLEEPING,
} PowerState;

#define POWER_MANAGER_IDLE_TIMEOUT_MS 30000U

void PowerManager_Init(void);
void PowerManager_Update(void);
void PowerManager_ResetIdleTimer(void);
void PowerManager_Sleep(void);
void PowerManager_Wake(void);
uint8_t PowerManager_IsSleeping(void);

#endif