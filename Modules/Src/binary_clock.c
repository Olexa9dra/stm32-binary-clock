#include "binary_clock.h"
#include "editor.h"

static RTC_Time clock;
static RTC_Time editClock;
static uint32_t lastUpdate = 0;
static Editor clockEditor;

static const EditorDigitRestriction clockRestrictions[EDITOR_FIELD_COUNT] = {
    {0, 2, 1},
    {0, 9, 1},
    {0, 5, 1},
    {0, 9, 1},
};

static uint16_t BinaryClock_GetTimeValue(uint8_t hour, uint8_t minute);
static void BinaryClock_UpdateEditTime(void);
static DisplayColumnMask BinaryClock_ValidateEdit(void);

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
  BinaryClock_UpdateEditTime();
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

  uint8_t digits[] = {
      editClock.hour / 10,
      editClock.hour % 10,
      editClock.minute / 10,
      editClock.minute % 10,
  };

  Editor_Begin(&clockEditor, digits, clockRestrictions);
}

void BinaryClock_IncrementSelected(void) { Editor_Increment(&clockEditor); }

void BinaryClock_DecrementSelected(void) { Editor_Decrement(&clockEditor); }

void BinaryClock_SelectNextField(void) { Editor_SelectNext(&clockEditor); }

DisplayColumnMask BinaryClock_GetSelectedColumn(void) {
  return Editor_GetSelectedColumn(&clockEditor);
}

static void BinaryClock_UpdateEditTime(void) {
  editClock.hour =
      Editor_GetDigit(&clockEditor, 0) * 10 + Editor_GetDigit(&clockEditor, 1);

  editClock.minute =
      Editor_GetDigit(&clockEditor, 2) * 10 + Editor_GetDigit(&clockEditor, 3);
}

static DisplayColumnMask BinaryClock_ValidateEdit(void) {
  BinaryClock_UpdateEditTime();

  if (editClock.hour > 23)
    return DISPLAY_COLUMN_2;

  return DISPLAY_COLUMN_NONE;
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
