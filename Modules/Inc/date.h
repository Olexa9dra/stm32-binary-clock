#ifndef __DATE_H
#define __DATE_H

#include "display.h"
#include "editor.h"
#include "rtc.h"

#define DATE_UPDATE_DELAY_MS 250U

typedef enum { DATE_EDIT_DAY_MONTH, DATE_EDIT_YEAR } DateEditStage;

void Date_Init(void);
void Date_Update(void);
uint16_t Date_GetDisplayValue(void);
uint16_t Date_GetEditDisplayValue(void);
DisplayColumnMask Date_GetSelectedColumn(void);
void Date_BeginEdit(void);
void Date_IncrementSelected(void);
void Date_DecrementSelected(void);
void Date_SelectNextField(void);
DisplayColumnMask Date_SaveEdit(void);
uint8_t Date_IsYearEdit(void);

#endif