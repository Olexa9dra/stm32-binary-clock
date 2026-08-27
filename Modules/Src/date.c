#include "date.h"

static RTC_Date date;
static RTC_Date editDate;
static DateEditStage editStage;
static DateField dateField;
static YearField yearField;
static uint32_t lastUpdate = 0;

static uint16_t Date_GetDateValue(uint8_t day, uint8_t month);
static uint16_t Date_GetYearValue(uint16_t year);
static uint8_t IsLeapYear(uint16_t year);

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
  if (editStage == DATE_EDIT_DAY_MONTH)
    return Date_GetDateValue(editDate.date, editDate.month);
  return Date_GetYearValue(editDate.year);
}

void Date_BeginEdit(void) {
  RTC_GetDate(&editDate);

  editStage = DATE_EDIT_DAY_MONTH;
  dateField = EDIT_DAY_TENS;
  yearField = EDIT_YEAR_THOUSANDS;
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

static DisplayColumnMask Date_ValidateYear(void) {
  if (editDate.year > 2099)
    return DISPLAY_COLUMN_2;
  return DISPLAY_COLUMN_NONE;
}

void Date_SelectNextField(void) {
  if (editStage == DATE_EDIT_DAY_MONTH) {
    dateField++;
    if (dateField >= EDIT_DAY_MONTH_COUNT)
      dateField = EDIT_DAY_TENS;
    return;
  }

  yearField++;
  if (yearField >= EDIT_YEAR_COUNT)
    yearField = EDIT_YEAR_THOUSANDS;
}

void Date_IncrementSelected(void) {
  uint8_t tens;
  uint8_t ones;

  if (editStage == DATE_EDIT_DAY_MONTH) {
    switch (dateField) {
    case EDIT_DAY_TENS:
      tens = editDate.date / 10;
      ones = editDate.date % 10;
      tens = (tens + 1) % 4;
      editDate.date = tens * 10 + ones;
      break;
    case EDIT_DAY_ONES:
      tens = editDate.date / 10;
      ones = editDate.date % 10;
      ones = (ones + 1) % 10;
      editDate.date = tens * 10 + ones;
      break;
    case EDIT_MONTH_TENS:
      tens = editDate.month / 10;
      ones = editDate.month % 10;
      tens = (tens + 1) % 2;
      editDate.month = tens * 10 + ones;
      break;
    case EDIT_MONTH_ONES:
      tens = editDate.month / 10;
      ones = editDate.month % 10;
      ones = (ones + 1) % 10;
      editDate.month = tens * 10 + ones;
      break;
    default:
      break;
    }
    return;
  }

  uint8_t thousands = editDate.year / 1000;
  uint8_t hundreds = (editDate.year / 100) % 10;
  uint8_t yearTens = (editDate.year / 10) % 10;
  uint8_t yearOnes = editDate.year % 10;

  switch (yearField) {
  case EDIT_YEAR_THOUSANDS:
    thousands = 2;
    break;
  case EDIT_YEAR_HUNDREDS:
    hundreds = (hundreds + 1) % 10;
    break;
  case EDIT_YEAR_TENS:
    yearTens = (yearTens + 1) % 10;
    break;
  case EDIT_YEAR_ONES:
    yearOnes = (yearOnes + 1) % 10;
    break;
  default:
    break;
  }

  editDate.year = thousands * 1000 + hundreds * 100 + yearTens * 10 + yearOnes;
}

void Date_DecrementSelected(void) {
  uint8_t tens;
  uint8_t ones;

  if (editStage == DATE_EDIT_DAY_MONTH) {
    switch (dateField) {
    case EDIT_DAY_TENS:
      tens = editDate.date / 10;
      ones = editDate.date % 10;
      tens = (tens == 0) ? 3 : tens - 1;
      editDate.date = tens * 10 + ones;
      break;
    case EDIT_DAY_ONES:
      tens = editDate.date / 10;
      ones = editDate.date % 10;
      ones = (ones == 0) ? 9 : ones - 1;
      editDate.date = tens * 10 + ones;
      break;
    case EDIT_MONTH_TENS:
      tens = editDate.month / 10;
      ones = editDate.month % 10;
      tens = (tens == 0) ? 1 : tens - 1;
      editDate.month = tens * 10 + ones;
      break;
    case EDIT_MONTH_ONES:
      tens = editDate.month / 10;
      ones = editDate.month % 10;
      ones = (ones == 0) ? 9 : ones - 1;
      editDate.month = tens * 10 + ones;
      break;
    default:
      break;
    }
    return;
  }

  uint8_t thousands = editDate.year / 1000;
  uint8_t hundreds = (editDate.year / 100) % 10;
  uint8_t yearTens = (editDate.year / 10) % 10;
  uint8_t yearOnes = editDate.year % 10;

  switch (yearField) {
  case EDIT_YEAR_THOUSANDS:
    thousands = 2;
    break;
  case EDIT_YEAR_HUNDREDS:
    hundreds = (hundreds == 0) ? 9 : hundreds - 1;
    break;
  case EDIT_YEAR_TENS:
    yearTens = (yearTens == 0) ? 9 : yearTens - 1;
    break;
  case EDIT_YEAR_ONES:
    yearOnes = (yearOnes == 0) ? 9 : yearOnes - 1;
    break;
  default:
    break;
  }

  editDate.year = thousands * 1000 + hundreds * 100 + yearTens * 10 + yearOnes;
}

DisplayColumnMask Date_SaveEdit(void) {
  if (editStage == DATE_EDIT_DAY_MONTH) {
    DisplayColumnMask errors = Date_ValidateDayMonth();
    if (errors != DISPLAY_COLUMN_NONE)
      return errors;
    editStage = DATE_EDIT_YEAR;
    yearField = EDIT_YEAR_THOUSANDS;
    return DISPLAY_COLUMN_NONE;
  }

  DisplayColumnMask errors = Date_ValidateYear();
  if (errors != DISPLAY_COLUMN_NONE)
    return errors;
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
  if (editStage == DATE_EDIT_DAY_MONTH) {
    switch (dateField) {
    case EDIT_DAY_TENS:
      return DISPLAY_COLUMN_1;
    case EDIT_DAY_ONES:
      return DISPLAY_COLUMN_2;
    case EDIT_MONTH_TENS:
      return DISPLAY_COLUMN_3;
    case EDIT_MONTH_ONES:
      return DISPLAY_COLUMN_4;
    default:
      return DISPLAY_COLUMN_NONE;
    }
  }

  switch (yearField) {
  case EDIT_YEAR_THOUSANDS:
    return DISPLAY_COLUMN_1;
  case EDIT_YEAR_HUNDREDS:
    return DISPLAY_COLUMN_2;
  case EDIT_YEAR_TENS:
    return DISPLAY_COLUMN_3;
  case EDIT_YEAR_ONES:
    return DISPLAY_COLUMN_4;
  default:
    return DISPLAY_COLUMN_NONE;
  }
}

uint8_t Date_IsYearEdit(void) { return editStage == DATE_EDIT_YEAR; }