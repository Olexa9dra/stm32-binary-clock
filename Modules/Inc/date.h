#ifndef __DATE_H
#define __DATE_H

#include "display.h"
#include "rtc_ds3231.h"
#include "stm32f4xx_hal.h"

typedef enum {
  EDIT_DAY_TENS,
  EDIT_DAY_ONES,
  EDIT_MONTH_TENS,
  EDIT_MONTH_ONES,
  EDIT_DAY_MONTH_COUNT
} DateField;

typedef enum {
  EDIT_YEAR_THOUSANDS,
  EDIT_YEAR_HUNDREDS,
  EDIT_YEAR_TENS,
  EDIT_YEAR_ONES,
  EDIT_YEAR_COUNT
} YearField;

typedef enum { DATE_EDIT_DAY_MONTH, DATE_EDIT_YEAR } DateEditStage;

void Date_Init(void);
void Date_Update(void);

uint16_t Date_GetDisplayValue(void);
uint16_t Date_GetEditDisplayValue(void);
DisplayColumn Date_GetSelectedColumn(void);

void Date_BeginEdit(void);

void Date_IncrementSelected(void);
void Date_DecrementSelected(void);
void Date_SelectNextField(void);

/* Returns 1 when editing is finished */
uint8_t Date_SaveEdit(void);

#endif