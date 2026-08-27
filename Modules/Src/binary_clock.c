#include "binary_clock.h"

static RTC_Time clock;
static RTC_Time editClock;
static EditField editField;
static uint32_t lastUpdate = 0;

static uint16_t BinaryClock_GetTimeValue(uint8_t hour, uint8_t minute);

void BinaryClock_Init(void) { RTC_GetTime(&clock); }

void BinaryClock_Update(void) {
  uint32_t now = HAL_GetTick();
  if (now - lastUpdate < CLOCK_UPDATE_DELAY_MS)
    return;

  lastUpdate = now;

  RTC_GetTime(&clock);
}

uint16_t BinaryClock_GetDisplayValue(void) {
  return BinaryClock_GetTimeValue(clock.hour, clock.minute);
}

uint16_t BinaryClock_GetEditDisplayValue(void) {
  return BinaryClock_GetTimeValue(editClock.hour, editClock.minute);
}

static uint16_t BinaryClock_GetTimeValue(uint8_t hour, uint8_t minute) {
  uint8_t hourTens = hour / 10;
  uint8_t hourOnes = hour % 10;
  uint8_t minuteTens = minute / 10;
  uint8_t minuteOnes = minute % 10;

  return (minuteOnes << 12) | (minuteTens << 8) | (hourOnes << 4) | hourTens;
}

void BinaryClock_BeginEdit(void) {
  RTC_GetTime(&editClock);
  editField = EDIT_HOURS_TENS;
}

void BinaryClock_SelectNextField(void) {
  editField++;
  if (editField >= EDIT_COUNT)
    editField = EDIT_HOURS_TENS;
}

void BinaryClock_IncrementSelected(void) {
  uint8_t tens;
  uint8_t ones;

  switch (editField) {
  case EDIT_HOURS_TENS:
    tens = editClock.hour / 10;
    ones = editClock.hour % 10;
    tens = (tens + 1) % 3;
    editClock.hour = tens * 10 + ones;
    break;
  case EDIT_HOURS_ONES:
    tens = editClock.hour / 10;
    ones = editClock.hour % 10;
    ones = (ones + 1) % 10;
    editClock.hour = tens * 10 + ones;
    break;
  case EDIT_MINUTES_TENS:
    tens = editClock.minute / 10;
    ones = editClock.minute % 10;
    tens = (tens + 1) % 6;
    editClock.minute = tens * 10 + ones;
    break;
  case EDIT_MINUTES_ONES:
    tens = editClock.minute / 10;
    ones = editClock.minute % 10;
    ones = (ones + 1) % 10;
    editClock.minute = tens * 10 + ones;
    break;
  default:
    break;
  }
}

void BinaryClock_DecrementSelected(void) {
  uint8_t tens;
  uint8_t ones;

  switch (editField) {
  case EDIT_HOURS_TENS:
    tens = editClock.hour / 10;
    ones = editClock.hour % 10;
    tens = (tens == 0) ? 2 : tens - 1;
    editClock.hour = tens * 10 + ones;
    break;
  case EDIT_HOURS_ONES:
    tens = editClock.hour / 10;
    ones = editClock.hour % 10;
    ones = (ones == 0) ? 9 : ones - 1;
    editClock.hour = tens * 10 + ones;
    break;
  case EDIT_MINUTES_TENS:
    tens = editClock.minute / 10;
    ones = editClock.minute % 10;
    tens = (tens == 0) ? 5 : tens - 1;
    editClock.minute = tens * 10 + ones;
    break;
  case EDIT_MINUTES_ONES:
    tens = editClock.minute / 10;
    ones = editClock.minute % 10;
    ones = (ones == 0) ? 9 : ones - 1;
    editClock.minute = tens * 10 + ones;
    break;
  default:
    break;
  }
}

static DisplayColumnMask BinaryClock_ValidateEdit(void) {
  DisplayColumnMask errors = DISPLAY_COLUMN_NONE;
  uint8_t hourTens = editClock.hour / 10;
  uint8_t hourOnes = editClock.hour % 10;
  if (hourTens == 2 && hourOnes > 3)
    errors |= DISPLAY_COLUMN_2;
  return errors;
}

DisplayColumnMask BinaryClock_SaveEdit(void) {
  DisplayColumnMask errors = BinaryClock_ValidateEdit();
  if (errors != DISPLAY_COLUMN_NONE)
    return errors;

  editClock.second = 0;
  if (RTC_SetTime(&editClock) == HAL_OK) {
    clock = editClock;
  }

  return DISPLAY_COLUMN_NONE;
}

DisplayColumnMask BinaryClock_GetSelectedColumn(void) {
  switch (editField) {
  case EDIT_HOURS_TENS:
    return DISPLAY_COLUMN_1;
  case EDIT_HOURS_ONES:
    return DISPLAY_COLUMN_2;
  case EDIT_MINUTES_TENS:
    return DISPLAY_COLUMN_3;
  case EDIT_MINUTES_ONES:
    return DISPLAY_COLUMN_4;
  default:
    return DISPLAY_COLUMN_NONE;
  }
}