#include "buttons.h"
#include "accelerometer.h"
#include "alarm.h"
#include "app.h"
#include "binary_clock.h"
#include "date.h"
#include "display.h"
#include "power_manager.h"

static Button buttons[BUTTON_COUNT] = {
    {BUTTON1_PIN, GPIO_PIN_SET, 0},
    {BUTTON2_PIN, GPIO_PIN_SET, 0},
    {BUTTON3_PIN, GPIO_PIN_SET, 0},
    {BUTTON4_PIN, GPIO_PIN_SET, 0},
};

static void Buttons_Update(uint16_t button);
static void Buttons_HandleNormalState(uint16_t button);
static void Buttons_HandleEditTimeState(uint16_t button);
static void Buttons_HandleEditDateState(uint16_t button);
static void Buttons_HandleEditAlarmState(uint16_t button);
static uint16_t Buttons_GetLogicalPin(uint16_t physicalPin);

void Buttons_Check(void) {
  uint32_t now = HAL_GetTick();

  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    GPIO_PinState state = HAL_GPIO_ReadPin(BUTTON_GPIO_PORT, buttons[i].pin);

    if (buttons[i].prevState == GPIO_PIN_SET && state == GPIO_PIN_RESET &&
        now - buttons[i].lastPressTime >= BUTTON_DEBOUNCE_MS) {
      buttons[i].lastPressTime = now;

      if (Alarm_IsRinging()) {
        Alarm_Disable();
        PowerManager_ResetIdleTimer();
        continue;
      }

      if (PowerManager_IsSleeping()) {
        PowerManager_Wake();
        buttons[i].prevState = state;
        for (uint8_t j = 0; j < BUTTON_COUNT; j++) {
          if (j != i)
            buttons[j].prevState =
                HAL_GPIO_ReadPin(BUTTON_GPIO_PORT, buttons[j].pin);
        }
        return;
      }

      PowerManager_ResetIdleTimer();
      Buttons_Update(Buttons_GetLogicalPin(buttons[i].pin));
    }

    buttons[i].prevState = state;
  }
}

static void Buttons_Update(uint16_t button) {
  switch (App_GetUiState()) {
  case UI_STATE_NORMAL:
    Buttons_HandleNormalState(button);
    break;
  case UI_STATE_EDIT_TIME:
    Buttons_HandleEditTimeState(button);
    break;
  case UI_STATE_EDIT_DATE:
    Buttons_HandleEditDateState(button);
    break;
  case UI_STATE_EDIT_ALARM:
    Buttons_HandleEditAlarmState(button);
    break;
  default:
    break;
  }
}

static void Buttons_HandleNormalState(uint16_t button) {
  DisplayMode mode = App_GetDisplayMode();

  switch (button) {
  case BUTTON1_PIN:
    mode++;
    if (mode >= DISPLAY_MODE_COUNT)
      mode = DISPLAY_MODE_TIME;
    App_SetDisplayMode(mode);
    break;
  case BUTTON2_PIN:
    if (mode == DISPLAY_MODE_TIME)
      mode = DISPLAY_MODE_COUNT - 1;
    else
      mode--;

    App_SetDisplayMode(mode);
    break;
  case BUTTON3_PIN:
    break;
  case BUTTON4_PIN:
    switch (mode) {
    case DISPLAY_MODE_TIME:
      BinaryClock_BeginEdit();
      App_SetUiState(UI_STATE_EDIT_TIME);
      Display_SetBlinkColumn(BinaryClock_GetSelectedColumn());
      break;
    case DISPLAY_MODE_DATE:
      Date_BeginEdit();
      App_SetUiState(UI_STATE_EDIT_DATE);
      Display_SetBlinkColumn(Date_GetSelectedColumn());
      break;
    case DISPLAY_MODE_ALARM:
      Alarm_BeginEdit();
      App_SetUiState(UI_STATE_EDIT_ALARM);
      Display_SetBlinkColumn(Alarm_GetSelectedColumn());
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

static void Buttons_HandleEditTimeState(uint16_t button) {
  switch (button) {
  case BUTTON1_PIN:
    BinaryClock_IncrementSelected();
    break;
  case BUTTON2_PIN:
    BinaryClock_DecrementSelected();
    break;
  case BUTTON3_PIN:
    BinaryClock_SelectNextField();
    Display_SetBlinkColumn(BinaryClock_GetSelectedColumn());
    break;
  case BUTTON4_PIN: {
    DisplayColumnMask errors = BinaryClock_SaveEdit();
    if (errors == DISPLAY_COLUMN_NONE) {
      Display_SetBlinkColumn(DISPLAY_COLUMN_NONE);
      App_SetUiState(UI_STATE_NORMAL);
    } else {
      Display_StartErrorBlink(errors);
    }
    break;
  }
  default:
    break;
  }
}

static void Buttons_HandleEditDateState(uint16_t button) {
  switch (button) {
  case BUTTON1_PIN:
    Date_IncrementSelected();
    break;
  case BUTTON2_PIN:
    Date_DecrementSelected();
    break;
  case BUTTON3_PIN:
    Date_SelectNextField();
    Display_SetBlinkColumn(Date_GetSelectedColumn());
    break;
  case BUTTON4_PIN: {
    DisplayColumnMask errors = Date_SaveEdit();
    if (errors != DISPLAY_COLUMN_NONE) {
      Display_StartErrorBlink(errors);
      break;
    }
    if (Date_IsYearEdit()) {
      Display_SetBlinkColumn(Date_GetSelectedColumn());
    } else {
      Display_SetBlinkColumn(DISPLAY_COLUMN_NONE);
      App_SetUiState(UI_STATE_NORMAL);
    }
    break;
  }
  default:
    break;
  }
}

static void Buttons_HandleEditAlarmState(uint16_t button) {
  switch (button) {
  case BUTTON1_PIN:
    Alarm_IncrementSelected();
    break;
  case BUTTON2_PIN:
    Alarm_DecrementSelected();
    break;
  case BUTTON3_PIN:
    Alarm_SelectNextField();
    Display_SetBlinkColumn(Alarm_GetSelectedColumn());
    break;
  case BUTTON4_PIN: {
    DisplayColumnMask errors = Alarm_SaveEdit();
    if (errors == DISPLAY_COLUMN_NONE) {
      Display_SetBlinkColumn(DISPLAY_COLUMN_NONE);
      App_SetUiState(UI_STATE_NORMAL);
    } else {
      Display_StartErrorBlink(errors);
    }
    break;
  }
  default:
    break;
  }
}

static uint16_t Buttons_GetLogicalPin(uint16_t physicalPin) {
  uint8_t physicalIndex;

  switch (physicalPin) {
  case BUTTON1_PIN:
    physicalIndex = 0;
    break;
  case BUTTON2_PIN:
    physicalIndex = 1;
    break;
  case BUTTON3_PIN:
    physicalIndex = 2;
    break;
  case BUTTON4_PIN:
    physicalIndex = 3;
    break;
  default:
    return physicalPin;
  }

  const uint8_t *mask;

  switch (Accelerometer_GetRotation()) {
  case DISPLAY_ROTATION_0:
    mask = BUTTON_MASK_0;
    break;

  case DISPLAY_ROTATION_90:
    mask = BUTTON_MASK_90;
    break;

  case DISPLAY_ROTATION_180:
    mask = BUTTON_MASK_180;
    break;

  case DISPLAY_ROTATION_270:
  default:
    mask = BUTTON_MASK_270;
    break;
  }

  switch (mask[physicalIndex]) {
  case 0:
    return BUTTON1_PIN;
  case 1:
    return BUTTON2_PIN;
  case 2:
    return BUTTON3_PIN;
  case 3:
    return BUTTON4_PIN;
  default:
    return physicalPin;
  }
}
