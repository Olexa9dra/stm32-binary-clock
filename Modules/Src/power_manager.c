#include "power_manager.h"
#include "accelerometer.h"
#include "led.h"

static PowerState state = POWER_STATE_ACTIVE;

static uint32_t lastActivityTick = 0;

static void PowerManager_TurnOffModules(void);
static void PowerManager_TurnOnModules(void);

void PowerManager_Init(void) {
  state = POWER_STATE_ACTIVE;
  lastActivityTick = HAL_GetTick();
  Accelerometer_Init();
  Accelerometer_EnableTapDetection();
}

void PowerManager_Update(void) {
  uint32_t now = HAL_GetTick();

  // Check tap even while sleeping.
  if (Accelerometer_TapDetected()) {
    PowerManager_Wake();
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

  PowerManager_TurnOffModules();

  state = POWER_STATE_SLEEPING;
}

void PowerManager_Wake(void) {
  if (state == POWER_STATE_ACTIVE) {
    lastActivityTick = HAL_GetTick();
    return;
  }

  PowerManager_TurnOnModules();

  state = POWER_STATE_ACTIVE;
  lastActivityTick = HAL_GetTick();
}

PowerState PowerManager_GetState(void) { return state; }

uint8_t PowerManager_IsSleeping(void) { return state == POWER_STATE_SLEEPING; }

static void PowerManager_TurnOffModules(void) {
  /*
   * RTC intentionally stays running.
   *
   * For now only turn off the LEDs.
   * Other modules can be added here once their
   * Sleep/Stop APIs are defined.
   */

  LED_Clear();
  LED_Show();
}

static void PowerManager_TurnOnModules(void) {
  /*
   * Other modules can be re-enabled here later.
   *
   * The display will be redrawn by App_Run().
   */
}