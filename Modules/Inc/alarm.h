#ifndef __ALARM_H
#define __ALARM_H

#include "display.h"
#include "rtc_ds3231.h"

#define ALARM_UPDATE_DELAY_MS 250U
#define ALARM_HOURS_MAX 24U
#define ALARM_HOURS_ONES_MAX 9U
#define ALARM_HOURS_ONES_MAX_20 9U
#define ALARM_MINUTES_TENS_MAX 5U
#define ALARM_MINUTES_ONES_MAX 9U

typedef enum {
  ALARM_EDIT_HOURS_TENS,
  ALARM_EDIT_HOURS_ONES,
  ALARM_EDIT_MINUTES_TENS,
  ALARM_EDIT_MINUTES_ONES,
  ALARM_EDIT_COUNT
} AlarmEditField;

void Alarm_Update(void);
uint16_t Alarm_GetDisplayValue(void);
uint16_t Alarm_GetEditDisplayValue(void);
DisplayColumn Alarm_GetSelectedColumn(void);
void Alarm_BeginEdit(void);
void Alarm_SelectNextField(void);
void Alarm_IncrementSelected(void);
void Alarm_DecrementSelected(void);
void Alarm_SaveEdit(void);
uint8_t Alarm_IsEnabled(void);
void Alarm_Disable(void);

#endif
