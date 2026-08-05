#ifndef __BINARY_CLOCK_H
#define __BINARY_CLOCK_H

#include "display.h"
#include "stm32f4xx_hal.h"

typedef enum {
  EDIT_HOURS_TENS,
  EDIT_HOURS_ONES,
  EDIT_MINUTES_TENS,
  EDIT_MINUTES_ONES,
  EDIT_COUNT
} EditField;

void BinaryClock_Init(void);
uint16_t BinaryClock_GetDisplayValue(void);
uint16_t BinaryClock_GetEditDisplayValue(void);
DisplayColumn BinaryClock_GetSelectedColumn(void);

void BinaryClock_BeginEdit(void);
void BinaryClock_SelectNextField(void);
void BinaryClock_IncrementSelected(void);
void BinaryClock_DecrementSelected(void);
void BinaryClock_SaveEdit(void);
void BinaryClock_Update(void);

#endif