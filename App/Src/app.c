#include "app.h"
#include "accelerometer.h"
#include "alarm.h"
#include "battery.h"
#include "binary_clock.h"
#include "buttons.h"
#include "buzzer.h"
#include "date.h"
#include "display.h"
#include "environment.h"
#include "power_manager.h"

static DisplayMode selectedMode = DISPLAY_MODE_TIME;
static UiState uiState = UI_STATE_NORMAL;

void App_Init(void) {
  BinaryClock_Init();
  Date_Init();
  Environment_Init();
  Accelerometer_Init();
  Battery_Init();
  Display_Init();
  Buzzer_Init();
  PowerManager_Init();
}

void App_Run(void) {
  PowerManager_Update();
  Buttons_Check();
  Alarm_Update();

  if (PowerManager_IsSleeping())
    return;

  BinaryClock_Update();
  Date_Update();
  Environment_Update();
  Battery_Update();

  uint16_t value;

  switch (selectedMode) {
  case DISPLAY_MODE_TIME:
    value = (uiState == UI_STATE_NORMAL) ? BinaryClock_GetDisplayValue()
                                         : BinaryClock_GetEditDisplayValue();
    break;
  case DISPLAY_MODE_DATE:
    value = (uiState == UI_STATE_NORMAL) ? Date_GetDisplayValue()
                                         : Date_GetEditDisplayValue();
    break;
  case DISPLAY_MODE_ENVIRONMENT:
    value = Environment_GetDisplayValue();
    break;
  case DISPLAY_MODE_ALARM:
    value = (uiState == UI_STATE_NORMAL) ? Alarm_GetDisplayValue()
                                         : Alarm_GetEditDisplayValue();
    break;
  case DISPLAY_MODE_BATTERY:
    value = Battery_GetDisplayValue();
    break;
  default:
    return;
  }

  Display_Show(value, selectedMode);
}

DisplayMode App_GetDisplayMode(void) { return selectedMode; }

void App_SetDisplayMode(DisplayMode mode) { selectedMode = mode; }

UiState App_GetUiState(void) { return uiState; }

void App_SetUiState(UiState state) { uiState = state; }
