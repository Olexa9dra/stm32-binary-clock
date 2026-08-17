#include "alarm.h"
#include "buzzer.h"

static RTC_Time alarmTime = {0};
static RTC_Time editAlarmTime = {0};
static AlarmEditField editField = ALARM_EDIT_HOURS_TENS;
static uint8_t alarmEnabled = 0;
static uint32_t lastUpdate = 0;
static uint32_t lastTriggeredMinute = UINT32_MAX;

static uint16_t Alarm_GetTimeValue(uint8_t hour, uint8_t minute);
static void Alarm_Clear(void);

void Alarm_Update(void) {
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
    Alarm_Clear();
    Buzzer_PlayMelody();
  }
}

uint16_t Alarm_GetDisplayValue(void) {
  if (alarmEnabled == 0)
    return 0;
  return Alarm_GetTimeValue(alarmTime.hour, alarmTime.minute);
}

uint16_t Alarm_GetEditDisplayValue(void) {
  if (alarmEnabled == 0 && editAlarmTime.hour == 0 &&
      editAlarmTime.minute == 0) {
    return 0;
  }

  uint8_t displayHour = editAlarmTime.hour;
  if (displayHour == 0 && editAlarmTime.minute == 0)
    displayHour = 24;

  return Alarm_GetTimeValue(displayHour, editAlarmTime.minute);
}

static uint16_t Alarm_GetTimeValue(uint8_t hour, uint8_t minute) {
  uint8_t hourTens = hour / 10;
  uint8_t hourOnes = hour % 10;
  uint8_t minuteTens = minute / 10;
  uint8_t minuteOnes = minute % 10;

  return (minuteOnes << 12) | (minuteTens << 8) | (hourOnes << 4) | hourTens;
}

void Alarm_BeginEdit(void) {
  editAlarmTime = alarmTime;
  editField = ALARM_EDIT_HOURS_TENS;
}

void Alarm_SelectNextField(void) {
  editField++;
  if (editField >= ALARM_EDIT_COUNT)
    editField = ALARM_EDIT_HOURS_TENS;
}

void Alarm_IncrementSelected(void) {
  uint8_t tens;
  uint8_t ones;

  switch (editField) {
  case ALARM_EDIT_HOURS_TENS:
    tens = editAlarmTime.hour / 10;
    ones = editAlarmTime.hour % 10;
    tens++;
    if (tens > 2)
      tens = 0;
    if (tens == 2 && ones > 4)
      ones = 4;
    editAlarmTime.hour = tens * 10 + ones;
    break;
  case ALARM_EDIT_HOURS_ONES:
    tens = editAlarmTime.hour / 10;
    ones = editAlarmTime.hour % 10;
    if (tens == 2) {
      ones++;
      if (ones > 4)
        ones = 0;
    } else {
      ones++;
      if (ones > 9)
        ones = 0;
    }
    editAlarmTime.hour = tens * 10 + ones;
    if (editAlarmTime.hour == 24)
      editAlarmTime.minute = 0;
    break;
  case ALARM_EDIT_MINUTES_TENS:
    if (editAlarmTime.hour == 24) {
      editAlarmTime.minute = 0;
      break;
    }
    tens = editAlarmTime.minute / 10;
    ones = editAlarmTime.minute % 10;
    tens++;
    if (tens > 5)
      tens = 0;
    editAlarmTime.minute = tens * 10 + ones;
    break;
  case ALARM_EDIT_MINUTES_ONES:
    if (editAlarmTime.hour == 24) {
      editAlarmTime.minute = 0;
      break;
    }
    tens = editAlarmTime.minute / 10;
    ones = editAlarmTime.minute % 10;
    ones++;
    if (ones > 9)
      ones = 0;
    editAlarmTime.minute = tens * 10 + ones;
    break;
  default:
    break;
  }
}

void Alarm_DecrementSelected(void) {
  uint8_t tens;
  uint8_t ones;

  switch (editField) {
  case ALARM_EDIT_HOURS_TENS:
    tens = editAlarmTime.hour / 10;
    ones = editAlarmTime.hour % 10;
    if (tens == 0)
      tens = 2;
    else
      tens--;
    if (tens == 2 && ones > 4)
      ones = 4;
    editAlarmTime.hour = tens * 10 + ones;
    break;
  case ALARM_EDIT_HOURS_ONES:
    tens = editAlarmTime.hour / 10;
    ones = editAlarmTime.hour % 10;
    if (tens == 2) {
      if (ones == 0)
        ones = 4;
      else
        ones--;
    } else {
      if (ones == 0)
        ones = 9;
      else
        ones--;
    }
    editAlarmTime.hour = tens * 10 + ones;
    break;
  case ALARM_EDIT_MINUTES_TENS:
    if (editAlarmTime.hour == 24) {
      editAlarmTime.minute = 0;
      break;
    }
    tens = editAlarmTime.minute / 10;
    ones = editAlarmTime.minute % 10;
    if (tens == 0)
      tens = 5;
    else
      tens--;
    editAlarmTime.minute = tens * 10 + ones;
    break;
  case ALARM_EDIT_MINUTES_ONES:
    if (editAlarmTime.hour == 24) {
      editAlarmTime.minute = 0;
      break;
    }
    tens = editAlarmTime.minute / 10;
    ones = editAlarmTime.minute % 10;
    if (ones == 0)
      ones = 9;
    else
      ones--;
    editAlarmTime.minute = tens * 10 + ones;
    break;
  default:
    break;
  }
}

void Alarm_SaveEdit(void) {
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
}

uint8_t Alarm_IsEnabled(void) { return alarmEnabled; }

void Alarm_Disable(void) { Alarm_Clear(); }

static void Alarm_Clear(void) {
  alarmTime = (RTC_Time){0};
  editAlarmTime = (RTC_Time){0};
  alarmEnabled = 0;
  lastTriggeredMinute = UINT32_MAX;
}

DisplayColumn Alarm_GetSelectedColumn(void) {
  switch (editField) {
  case ALARM_EDIT_HOURS_TENS:
    return DISPLAY_COLUMN_1;
  case ALARM_EDIT_HOURS_ONES:
    return DISPLAY_COLUMN_2;
  case ALARM_EDIT_MINUTES_TENS:
    return DISPLAY_COLUMN_3;
  case ALARM_EDIT_MINUTES_ONES:
    return DISPLAY_COLUMN_4;
  default:
    return DISPLAY_COLUMN_NONE;
  }
}
