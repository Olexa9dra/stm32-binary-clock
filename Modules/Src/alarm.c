#include "alarm.h"
#include "buzzer.h"
#include "editor.h"

static RTC_Time alarmTime = {0};
static RTC_Time editAlarmTime = {0};
static uint8_t alarmEnabled = 0;
static uint32_t lastUpdate = 0;
static uint32_t lastTriggeredMinute = UINT32_MAX;
static uint8_t alarmRinging = 0;
static Editor alarmEditor;

static const EditorDigitRestriction alarmRestrictions[EDITOR_FIELD_COUNT] = {
    {0, 2, 1},
    {0, 9, 1},
    {0, 5, 1},
    {0, 9, 1},
};

static uint16_t Alarm_GetTimeValue(uint8_t hour, uint8_t minute);
static void Alarm_UpdateEditTime(void);
static DisplayColumnMask Alarm_ValidateEdit(void);
static void Alarm_Clear(void);

void Alarm_Update(void) {
  if (alarmRinging) {
    Buzzer_Update();
    return;
  }

  if (alarmEnabled == 0)
    return;

  uint32_t now = HAL_GetTick();
  if (now - lastUpdate < ALARM_UPDATE_DELAY_MS)
    return;

  lastUpdate = now;

  RTC_Time currentTime;
  if (RTC_GetTime(&currentTime) != HAL_OK)
    return;

  uint32_t currentMinute =
      ((uint32_t)currentTime.hour * 60U) + currentTime.minute;

  if (currentMinute == lastTriggeredMinute)
    return;

  if (currentTime.hour == alarmTime.hour &&
      currentTime.minute == alarmTime.minute) {

    lastTriggeredMinute = currentMinute;
    alarmRinging = 1;

    Buzzer_StartAlarm();
  }
}

uint16_t Alarm_GetDisplayValue(void) {
  if (alarmEnabled == 0)
    return 0;
  return Alarm_GetTimeValue(alarmTime.hour, alarmTime.minute);
}

uint16_t Alarm_GetEditDisplayValue(void) {
  Alarm_UpdateEditTime();

  if (alarmEnabled == 0 && editAlarmTime.hour == 0 &&
      editAlarmTime.minute == 0) {
    return 0;
  }

  uint8_t displayHour = editAlarmTime.hour;
  if (displayHour == 0 && editAlarmTime.minute == 0)
    displayHour = 24;

  return Alarm_GetTimeValue(displayHour, editAlarmTime.minute);
}

void Alarm_BeginEdit(void) {
  editAlarmTime = alarmTime;

  uint8_t digits[] = {
      editAlarmTime.hour / 10,
      editAlarmTime.hour % 10,
      editAlarmTime.minute / 10,
      editAlarmTime.minute % 10,
  };

  Editor_Begin(&alarmEditor, digits, alarmRestrictions);
}

void Alarm_SelectNextField(void) { Editor_SelectNext(&alarmEditor); }

void Alarm_IncrementSelected(void) { Editor_Increment(&alarmEditor); }

void Alarm_DecrementSelected(void) { Editor_Decrement(&alarmEditor); }

DisplayColumnMask Alarm_SaveEdit(void) {
  Alarm_UpdateEditTime();
  DisplayColumnMask errors = Alarm_ValidateEdit();

  if (errors != DISPLAY_COLUMN_NONE)
    return errors;

  Buzzer_StopAlarm();
  alarmRinging = 0;

  if (editAlarmTime.hour == 0 && editAlarmTime.minute == 0) {
    alarmEnabled = 0;
  } else {
    if (editAlarmTime.hour == 24) {
      editAlarmTime.hour = 0;
      editAlarmTime.minute = 0;
    }
    alarmTime = editAlarmTime;
    alarmEnabled = 1;
  }

  lastTriggeredMinute = UINT32_MAX;

  return DISPLAY_COLUMN_NONE;
}

void Alarm_Disable(void) { Alarm_Clear(); }

DisplayColumnMask Alarm_GetSelectedColumn(void) {
  return Editor_GetSelectedColumn(&alarmEditor);
}

uint8_t Alarm_IsRinging(void) { return alarmRinging; }

static uint16_t Alarm_GetTimeValue(uint8_t hour, uint8_t minute) {
  uint8_t hourTens = hour / 10;
  uint8_t hourOnes = hour % 10;
  uint8_t minuteTens = minute / 10;
  uint8_t minuteOnes = minute % 10;

  return (minuteOnes << 12) | (minuteTens << 8) | (hourOnes << 4) | hourTens;
}

static void Alarm_UpdateEditTime(void) {
  editAlarmTime.hour =
      Editor_GetDigit(&alarmEditor, 0) * 10 + Editor_GetDigit(&alarmEditor, 1);

  editAlarmTime.minute =
      Editor_GetDigit(&alarmEditor, 2) * 10 + Editor_GetDigit(&alarmEditor, 3);
}

static DisplayColumnMask Alarm_ValidateEdit(void) {
  uint8_t hourTens = editAlarmTime.hour / 10;
  uint8_t hourOnes = editAlarmTime.hour % 10;

  if (hourTens > 2)
    return DISPLAY_COLUMN_1;
  if (hourTens == 2 && hourOnes > 4)
    return DISPLAY_COLUMN_2;
  if (hourTens == 2 && hourOnes == 4) {
    if (editAlarmTime.minute / 10 != 0)
      return DISPLAY_COLUMN_3;
    if (editAlarmTime.minute % 10 != 0)
      return DISPLAY_COLUMN_4;
  }

  return DISPLAY_COLUMN_NONE;
}

static void Alarm_Clear(void) {
  alarmTime = (RTC_Time){0};
  editAlarmTime = (RTC_Time){0};
  alarmEnabled = 0;
  alarmRinging = 0;
  lastTriggeredMinute = UINT32_MAX;
  Buzzer_StopAlarm();
}