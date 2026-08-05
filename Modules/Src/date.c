
#include "date.h"
#include "display.h"
#include "rtc_ds3231.h"

static RTC_Date date;
static RTC_Date editDate;

static DateEditStage editStage;
static DateField dateField;
static YearField yearField;

static uint32_t lastUpdate = 0;

static uint16_t Date_GetDateValue(uint8_t day, uint8_t month);
static uint16_t Date_GetYearValue(uint16_t year);

static uint8_t IsLeapYear(uint16_t year);
static uint8_t DaysInMonth(uint8_t month, uint16_t year);
static void ClampDate(RTC_Date *date);

void Date_Init(void) { RTC3231_GetDate(&date); }

void Date_Update(void) {
  if (HAL_GetTick() - lastUpdate >= 250) {
    lastUpdate = HAL_GetTick();
    RTC3231_GetDate(&date);
  }
}

uint16_t Date_GetDisplayValue(void) {
  return Date_GetDateValue(date.day, date.month);
}

uint16_t Date_GetEditDisplayValue(void) {
  if (editStage == DATE_EDIT_DAY_MONTH)
    return Date_GetDateValue(editDate.day, editDate.month);

  return Date_GetYearValue(editDate.year);
}

void Date_BeginEdit(void) {
  RTC3231_GetDate(&editDate);

  editStage = DATE_EDIT_DAY_MONTH;
  dateField = EDIT_DAY_TENS;
  yearField = EDIT_YEAR_THOUSANDS;

  ClampDate(&editDate);
}

static uint16_t Date_GetDateValue(uint8_t day, uint8_t month) {
  uint8_t dt = day / 10;
  uint8_t d1 = day % 10;

  uint8_t mt = month / 10;
  uint8_t m1 = month % 10;

  return (m1 << 12) | (mt << 8) | (d1 << 4) | dt;
}

static uint16_t Date_GetYearValue(uint16_t year) {
  uint8_t th = year / 1000;
  uint8_t hu = (year / 100) % 10;
  uint8_t te = (year / 10) % 10;
  uint8_t on = year % 10;

  return (on << 12) | (te << 8) | (hu << 4) | th;
}

static uint8_t IsLeapYear(uint16_t year) {
  if ((year % 400) == 0)
    return 1;

  if ((year % 100) == 0)
    return 0;

  return (year % 4) == 0;
}

static uint8_t DaysInMonth(uint8_t month, uint16_t year) {
  switch (month) {
  case 1:
  case 3:
  case 5:
  case 7:
  case 8:
  case 10:
  case 12:
    return 31;

  case 4:
  case 6:
  case 9:
  case 11:
    return 30;

  case 2:
    return IsLeapYear(year) ? 29 : 28;

  default:
    return 31;
  }
}

static void ClampDate(RTC_Date *date) {
  if (date->year < 2000)
    date->year = 2000;

  if (date->year > 2099)
    date->year = 2099;

  if (date->month < 1)
    date->month = 1;

  if (date->month > 12)
    date->month = 12;

  uint8_t maxDays = DaysInMonth(date->month, date->year);

  if (date->day < 1)
    date->day = 1;

  if (date->day > maxDays)
    date->day = maxDays;
}

void Date_SelectNextField(void) {
  if (editStage == DATE_EDIT_DAY_MONTH) {
    dateField++;

    if (dateField >= EDIT_DAY_MONTH_COUNT)
      dateField = EDIT_DAY_TENS;
  } else {
    yearField++;

    if (yearField >= EDIT_YEAR_COUNT)
      yearField = EDIT_YEAR_THOUSANDS;
  }
}

void Date_IncrementSelected(void) {
  uint8_t tens, ones;
  uint8_t th, hu, te, on;

  if (editStage == DATE_EDIT_DAY_MONTH) {
    switch (dateField) {
    case EDIT_DAY_TENS:
      tens = editDate.day / 10;
      ones = editDate.day % 10;

      tens = (tens + 1) % 4;
      editDate.day = tens * 10 + ones;
      break;

    case EDIT_DAY_ONES:
      tens = editDate.day / 10;
      ones = editDate.day % 10;

      ones = (ones + 1) % 10;
      editDate.day = tens * 10 + ones;
      break;

    case EDIT_MONTH_TENS:
      tens = editDate.month / 10;
      ones = editDate.month % 10;

      tens = (tens == 0) ? 1 : 0;

      if (tens == 0 && ones == 0)
        ones = 1;

      if (tens == 1 && ones > 2)
        ones = 2;

      editDate.month = tens * 10 + ones;
      break;

    case EDIT_MONTH_ONES:
      tens = editDate.month / 10;
      ones = editDate.month % 10;

      if (tens == 0)
        ones = (ones == 9) ? 1 : ones + 1;
      else
        ones = (ones + 1) % 3;

      editDate.month = tens * 10 + ones;
      break;

    default:
      break;
    }

    ClampDate(&editDate);
    return;
  }

  th = editDate.year / 1000;
  hu = (editDate.year / 100) % 10;
  te = (editDate.year / 10) % 10;
  on = editDate.year % 10;

  switch (yearField) {
  case EDIT_YEAR_THOUSANDS:
    th = 2;
    break;

  case EDIT_YEAR_HUNDREDS:
    hu = (hu + 1) % 10;
    break;

  case EDIT_YEAR_TENS:
    te = (te + 1) % 10;
    break;

  case EDIT_YEAR_ONES:
    on = (on + 1) % 10;
    break;

  default:
    break;
  }

  editDate.year = th * 1000 + hu * 100 + te * 10 + on;

  ClampDate(&editDate);
}

void Date_DecrementSelected(void) {
  uint8_t tens, ones;
  uint8_t th, hu, te, on;

  if (editStage == DATE_EDIT_DAY_MONTH) {

    switch (dateField) {
    case EDIT_DAY_TENS:
      tens = editDate.day / 10;
      ones = editDate.day % 10;

      tens = (tens == 0) ? 3 : tens - 1;
      editDate.day = tens * 10 + ones;
      break;

    case EDIT_DAY_ONES:
      tens = editDate.day / 10;
      ones = editDate.day % 10;

      ones = (ones == 0) ? 9 : ones - 1;
      editDate.day = tens * 10 + ones;
      break;

    case EDIT_MONTH_TENS:
      tens = editDate.month / 10;
      ones = editDate.month % 10;

      tens = (tens == 0) ? 1 : 0;

      if (tens == 0 && ones == 0)
        ones = 1;

      if (tens == 1 && ones > 2)
        ones = 2;

      editDate.month = tens * 10 + ones;
      break;

    case EDIT_MONTH_ONES:
      tens = editDate.month / 10;
      ones = editDate.month % 10;

      if (tens == 0)
        ones = (ones == 1) ? 9 : ones - 1;
      else
        ones = (ones == 0) ? 2 : ones - 1;

      editDate.month = tens * 10 + ones;
      break;

    default:
      break;
    }

    ClampDate(&editDate);
    return;
  }

  th = editDate.year / 1000;
  hu = (editDate.year / 100) % 10;
  te = (editDate.year / 10) % 10;
  on = editDate.year % 10;

  switch (yearField) {
  case EDIT_YEAR_THOUSANDS:
    th = 2;
    break;

  case EDIT_YEAR_HUNDREDS:
    hu = (hu == 0) ? 9 : hu - 1;
    break;

  case EDIT_YEAR_TENS:
    te = (te == 0) ? 9 : te - 1;
    break;

  case EDIT_YEAR_ONES:
    on = (on == 0) ? 9 : on - 1;
    break;

  default:
    break;
  }

  editDate.year = th * 1000 + hu * 100 + te * 10 + on;

  ClampDate(&editDate);
}

uint8_t Date_SaveEdit(void) {
  if (editStage == DATE_EDIT_DAY_MONTH) {
    editStage = DATE_EDIT_YEAR;
    yearField = EDIT_YEAR_THOUSANDS;
    ClampDate(&editDate);
    return 0;
  }

  ClampDate(&editDate);

  if (RTC3231_SetDate(&editDate) == HAL_OK)
    RTC3231_GetDate(&date);

  return 1;
}

DisplayColumn Date_GetSelectedColumn(void) {
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