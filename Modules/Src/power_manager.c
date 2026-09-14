#include "power_manager.h"
#include "accelerometer.h"
#include "led.h"

static PowerState state = POWER_STATE_ACTIVE;
static uint32_t lastActivityTick = 0;

static void PowerManager_BlankDisplay(void);

void PowerManager_Init(void) {
  state = POWER_STATE_ACTIVE;
  lastActivityTick = HAL_GetTick();
  Accelerometer_EnableTapDetection();
}

void PowerManager_Update(void) {
  uint32_t now = HAL_GetTick();

  if (Accelerometer_TapDetected()) {
    PowerManager_ResetIdleTimer();
    return;
  }
  if (state == POWER_STATE_SLEEPING)
    return;
  if ((now - lastActivityTick) >= POWER_MANAGER_IDLE_TIMEOUT_MS) {
    PowerManager_Sleep();
  }
}

void PowerManager_ResetIdleTimer(void) {
  lastActivityTick = HAL_GetTick();

  if (state == POWER_STATE_SLEEPING) {
    PowerManager_Wake();
  }
}

void PowerManager_Sleep(void) {
  if (state == POWER_STATE_SLEEPING)
    return;

  state = POWER_STATE_SLEEPING;
  PowerManager_BlankDisplay();
}

void PowerManager_Wake(void) {
  if (state == POWER_STATE_ACTIVE) {
    lastActivityTick = HAL_GetTick();
    return;
  }

  state = POWER_STATE_ACTIVE;
  lastActivityTick = HAL_GetTick();
}

uint8_t PowerManager_IsSleeping(void) { return state == POWER_STATE_SLEEPING; }

static void PowerManager_BlankDisplay(void) {
  LED_Clear();
  HAL_Delay(1);
  LED_Show();
}