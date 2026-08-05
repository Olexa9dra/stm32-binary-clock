#include "binary_clock.h"
#include "display.h"
#include "rtc_ds3231.h"
#include <stdint.h>

RTC_Time clock;
RTC_Time editClock;
static EditField editField;
static uint32_t lastUpdate = 0;

static uint16_t BinaryClock_GetTimeValue(uint16_t h, uint16_t m);

void BinaryClock_Init(void) {
  RTC3231_Init();
  RTC3231_GetTime(&clock);
}

uint16_t BinaryClock_GetDisplayValue(void) {
  return BinaryClock_GetTimeValue(clock.hour, clock.minute);
}

uint16_t BinaryClock_GetEditDisplayValue(void) {
  return BinaryClock_GetTimeValue(editClock.hour, editClock.minute);
}

static uint16_t BinaryClock_GetTimeValue(uint16_t h, uint16_t m) {
  uint8_t ht = h / 10;
  uint8_t ho = h % 10;
  uint8_t mt = m / 10;
  uint8_t mo = m % 10;

  return (mo << 12) | (mt << 8) | (ho << 4) | ht;
}

void BinaryClock_BeginEdit(void) {
  RTC3231_GetTime(&editClock);
  editField = EDIT_HOURS_TENS;
}

void BinaryClock_SelectNextField(void) {
  editField++;
  if (editField >= EDIT_COUNT)
    editField = EDIT_HOURS_TENS;
}

void BinaryClock_IncrementSelected(void) {
  uint8_t tens, ones;
  switch (editField) {
  case EDIT_HOURS_TENS:
    tens = editClock.hour / 10;
    ones = editClock.hour % 10;
    tens = (tens + 1) % 3;
    // Clamp 24-29 to 20-23
    if (tens == 2 && ones > 3)
      ones = 3;
    editClock.hour = tens * 10 + ones;
    break;
  case EDIT_HOURS_ONES:
    tens = editClock.hour / 10;
    ones = editClock.hour % 10;
    if (tens == 2)
      ones = (ones + 1) % 4; // 0-3
    else
      ones = (ones + 1) % 10; // 0-9
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
  uint8_t tens, ones;
  switch (editField) {
  case EDIT_HOURS_TENS:
    tens = editClock.hour / 10;
    ones = editClock.hour % 10;
    tens = (tens == 0) ? 2 : tens - 1;
    if (tens == 2 && ones > 3)
      ones = 3;
    editClock.hour = tens * 10 + ones;
    break;
  case EDIT_HOURS_ONES:
    tens = editClock.hour / 10;
    ones = editClock.hour % 10;
    if (tens == 2)
      ones = (ones == 0) ? 3 : ones - 1;
    else
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

void BinaryClock_SaveEdit(void) {
  editClock.second = 0;
  RTC3231_SetTime(&editClock);
  clock = editClock;
}

void BinaryClock_Update(void) {
  if (HAL_GetTick() - lastUpdate >= 250) {
    lastUpdate = HAL_GetTick();
    RTC3231_GetTime(&clock);
  }
}

DisplayColumn BinaryClock_GetSelectedColumn(void) {
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