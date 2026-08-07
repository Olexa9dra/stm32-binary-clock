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

#define COLOR_OFF 0, 0, 0
#define COLOR_YELLOW 6, 6, 0
#define COLOR_GREEN 0, 6, 0
#define COLOR_RED 6, 0, 0
#define COLOR_BLUE 0, 0, 6

void Display_Show(uint16_t value, DisplayMode mode);
void Display_SetBlinkColumn(DisplayColumn column);

#endif