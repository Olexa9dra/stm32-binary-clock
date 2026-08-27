#ifndef __ALARM_H
#define __ALARM_H

#include "display.h"
#include "rtc.h"

#define ALARM_UPDATE_DELAY_MS 250U

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
DisplayColumnMask Alarm_GetSelectedColumn(void);
void Alarm_BeginEdit(void);
void Alarm_SelectNextField(void);
void Alarm_IncrementSelected(void);
void Alarm_DecrementSelected(void);
DisplayColumnMask Alarm_SaveEdit(void);
void Alarm_Disable(void);
uint8_t Alarm_IsRinging(void);

#endif
