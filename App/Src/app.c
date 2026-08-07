#include "app.h"
#include "binary_clock.h"
#include "buttons.h"
#include "buzzer.h"
#include "date.h"
#include "delay_us.h"
#include "display.h"
#include "environment.h"
#include "light_sensor.h"
#include "ws2812.h"

extern TIM_HandleTypeDef htim1;
static DisplayMode selectedMode = DISPLAY_MODE_TIME;
static UiState uiState = UI_STATE_NORMAL;

void App_Init(void) {
  DelayUs_Init();
  WS2812_Init(&htim1);
  LightSensor_Init();
  BinaryClock_Init();
  Date_Init();
  Environment_Init();
  Buttons_Init();
  // Buzzer_Init();
  HAL_Delay(1000);
}

void App_Run(void) {
  LightSensor_Update();
  Buttons_Check();
  BinaryClock_Update();
  Date_Update();
  Environment_Update();

  switch (selectedMode) {
  case DISPLAY_MODE_TIME:
    if (uiState == UI_STATE_NORMAL)
      Display_Show(BinaryClock_GetDisplayValue(), DISPLAY_MODE_TIME);
    else
      Display_Show(BinaryClock_GetEditDisplayValue(), DISPLAY_MODE_TIME);
    break;
  case DISPLAY_MODE_DATE:
    if (uiState == UI_STATE_NORMAL)
      Display_Show(Date_GetDisplayValue(), DISPLAY_MODE_DATE);
    else
      Display_Show(Date_GetEditDisplayValue(), DISPLAY_MODE_DATE);
    break;
  case DISPLAY_MODE_ENVIRONMENT:
    Display_Show(Environment_GetDisplayValue(), DISPLAY_MODE_ENVIRONMENT);
    break;
  default:
    break;
  }
}

DisplayMode App_GetDisplayMode(void) { return selectedMode; }

void App_SetDisplayMode(DisplayMode mode) { selectedMode = mode; }

UiState App_GetUiState(void) { return uiState; }
void App_SetUiState(UiState state) { uiState = state; }
