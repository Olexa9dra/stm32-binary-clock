#include "buttons.h"
#include "alarm.h"
#include "app.h"
#include "binary_clock.h"
#include "date.h"
#include "display.h"

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

void Buttons_Check(void) {
  uint32_t now = HAL_GetTick();

  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    GPIO_PinState state = HAL_GPIO_ReadPin(BUTTON_GPIO_PORT, buttons[i].pin);
    if (buttons[i].prevState == GPIO_PIN_SET && state == GPIO_PIN_RESET &&
        now - buttons[i].lastPressTime >= BUTTON_DEBOUNCE_MS) {
      buttons[i].lastPressTime = now;
      Buttons_Update(buttons[i].pin);
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
    // TODO
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
  case BUTTON4_PIN:
    BinaryClock_SaveEdit();
    Display_SetBlinkColumn(DISPLAY_COLUMN_NONE);
    App_SetUiState(UI_STATE_NORMAL);
    break;
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
  case BUTTON4_PIN:
    if (Date_SaveEdit()) {
      Display_SetBlinkColumn(DISPLAY_COLUMN_NONE);
      App_SetUiState(UI_STATE_NORMAL);
    } else {
      Display_SetBlinkColumn(Date_GetSelectedColumn());
    }
    break;
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
  case BUTTON4_PIN:
    Alarm_SaveEdit();
    Display_SetBlinkColumn(DISPLAY_COLUMN_NONE);
    App_SetUiState(UI_STATE_NORMAL);
    break;
  default:
    break;
  }
}
