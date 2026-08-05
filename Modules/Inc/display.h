#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "stm32f4xx_hal.h"

typedef enum {
  DISPLAY_MODE_TIME,
  DISPLAY_MODE_DATE,
  DISPLAY_MODE_ENVIRONMENT,
  DISPLAY_MODE_COUNT,
} DisplayMode;

typedef enum {
  DISPLAY_COLUMN_1,
  DISPLAY_COLUMN_2,
  DISPLAY_COLUMN_3,
  DISPLAY_COLUMN_4,
  DISPLAY_COLUMN_NONE,
} DisplayColumn;

void Display_Show(uint16_t value);
void Display_SetBlinkColumn(DisplayColumn column);

#endif