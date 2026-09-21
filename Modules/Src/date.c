#include "date.h"
#include "editor.h"

static RTC_Date date;
static RTC_Date editDate;
static DateEditStage editStage;
static Editor dateEditor;
static uint32_t lastUpdate = 0;

static const EditorDigitRestriction dateRestrictions[EDITOR_FIELD_COUNT] = {
    {0, 3, 1},
    {0, 9, 1},
    {0, 1, 1},
    {0, 9, 1},
};

static const EditorDigitRestriction yearRestrictions[EDITOR_FIELD_COUNT] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 9, 1},
    {0, 9, 1},
};

static uint16_t Date_GetDateValue(uint8_t day, uint8_t month);
static uint16_t Date_GetYearValue(uint16_t year);
static uint8_t IsLeapYear(uint16_t year);
static void Date_UpdateEditDate(void);
static DisplayColumnMask Date_ValidateDayMonth(void);

void Date_Init(void) { RTC_GetDate(&date); }

void Date_Update(void) {
  uint32_t now = HAL_GetTick();
  if (now - lastUpdate < DATE_UPDATE_DELAY_MS)
    return;

  lastUpdate = now;
  RTC_GetDate(&date);
}

uint16_t Date_GetDisplayValue(void) {
  return Date_GetDateValue(date.date, date.month);
}

uint16_t Date_GetEditDisplayValue(void) {
  Date_UpdateEditDate();

  if (editStage == DATE_EDIT_DAY_MONTH)
    return Date_GetDateValue(editDate.date, editDate.month);

  return Date_GetYearValue(editDate.year);
}

void Date_BeginEdit(void) {
  RTC_GetDate(&editDate);

  editStage = DATE_EDIT_DAY_MONTH;

  uint8_t digits[] = {
      editDate.date / 10,
      editDate.date % 10,
      editDate.month / 10,
      editDate.month % 10,
  };

  Editor_Begin(&dateEditor, digits, dateRestrictions);
}

void Date_SelectNextField(void) { Editor_SelectNext(&dateEditor); }

void Date_IncrementSelected(void) { Editor_Increment(&dateEditor); }

void Date_DecrementSelected(void) { Editor_Decrement(&dateEditor); }

DisplayColumnMask Date_SaveEdit(void) {
  Date_UpdateEditDate();

  if (editStage == DATE_EDIT_DAY_MONTH) {
    DisplayColumnMask errors = Date_ValidateDayMonth();

    if (errors != DISPLAY_COLUMN_NONE)
      return errors;

    editStage = DATE_EDIT_YEAR;

    uint8_t digits[] = {
        editDate.year / 1000,
        (editDate.year / 100) % 10,
        (editDate.year / 10) % 10,
        editDate.year % 10,
    };

    Editor_Begin(&dateEditor, digits, yearRestrictions);
    Editor_SetSelectedField(&dateEditor, 2);

    return DISPLAY_COLUMN_NONE;
  }

  if (editDate.month == 2 && editDate.date == 29 &&
      !IsLeapYear(editDate.year)) {
    editDate.date = 28;
  }
  if (RTC_SetDate(&editDate) != HAL_OK) {
    return DISPLAY_COLUMN_NONE;
  }

  RTC_GetDate(&date);
  editStage = DATE_EDIT_DAY_MONTH;

  return DISPLAY_COLUMN_NONE;
}

DisplayColumnMask Date_GetSelectedColumn(void) {
  return Editor_GetSelectedColumn(&dateEditor);
}

uint8_t Date_IsYearEdit(void) { return editStage == DATE_EDIT_YEAR; }

static void Date_UpdateEditDate(void) {
  if (editStage == DATE_EDIT_DAY_MONTH) {
    editDate.date =
        Editor_GetDigit(&dateEditor, 0) * 10 + Editor_GetDigit(&dateEditor, 1);

    editDate.month =
        Editor_GetDigit(&dateEditor, 2) * 10 + Editor_GetDigit(&dateEditor, 3);

    return;
  }

  editDate.year = Editor_GetDigit(&dateEditor, 0) * 1000 +
                  Editor_GetDigit(&dateEditor, 1) * 100 +
                  Editor_GetDigit(&dateEditor, 2) * 10 +
                  Editor_GetDigit(&dateEditor, 3);
}

static uint16_t Date_GetDateValue(uint8_t day, uint8_t month) {
  uint8_t dayTens = day / 10;
  uint8_t dayOnes = day % 10;
  uint8_t monthTens = month / 10;
  uint8_t monthOnes = month % 10;

  return (monthOnes << 12) | (monthTens << 8) | (dayOnes << 4) | dayTens;
}

static uint16_t Date_GetYearValue(uint16_t year) {
  uint8_t thousands = year / 1000;
  uint8_t hundreds = (year / 100) % 10;
  uint8_t tens = (year / 10) % 10;
  uint8_t ones = year % 10;

  return (ones << 12) | (tens << 8) | (hundreds << 4) | thousands;
}

static uint8_t IsLeapYear(uint16_t year) {
  if (year % 400 == 0)
    return 1;
  if (year % 100 == 0)
    return 0;
  return year % 4 == 0;
}

static DisplayColumnMask Date_ValidateDayMonth(void) {
  uint8_t day = editDate.date;
  uint8_t month = editDate.month;

  if (day < 1)
    return DISPLAY_COLUMN_2;
  if (day > 31)
    return DISPLAY_COLUMN_2;

  if (month < 1)
    return DISPLAY_COLUMN_4;
  if (month > 12)
    return DISPLAY_COLUMN_4;

  uint8_t maxDays;
  switch (month) {
  case 2:
    maxDays = 29;
    break;
  case 4:
  case 6:
  case 9:
  case 11:
    maxDays = 30;
    break;
  default:
    maxDays = 31;
    break;
  }
  if (day > maxDays)
    return DISPLAY_COLUMN_2;
  return DISPLAY_COLUMN_NONE;
}