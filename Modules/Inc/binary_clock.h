#ifndef __BINARY_CLOCK_H
#define __BINARY_CLOCK_H

#include "display.h"
#include "rtc.h"

#define CLOCK_UPDATE_DELAY_MS 250U

typedef enum {
  EDIT_HOURS_TENS,
  EDIT_HOURS_ONES,
  EDIT_MINUTES_TENS,
  EDIT_MINUTES_ONES,
  EDIT_COUNT
} EditField;

void BinaryClock_Init(void);
void BinaryClock_Update(void);
uint16_t BinaryClock_GetDisplayValue(void);
uint16_t BinaryClock_GetEditDisplayValue(void);
DisplayColumnMask BinaryClock_GetSelectedColumn(void);
void BinaryClock_BeginEdit(void);
void BinaryClock_SelectNextField(void);
void BinaryClock_IncrementSelected(void);
void BinaryClock_DecrementSelected(void);
DisplayColumnMask BinaryClock_SaveEdit(void);

#endif