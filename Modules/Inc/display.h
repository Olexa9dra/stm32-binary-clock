#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <stdint.h>

#define DISPLAY_COLUMN_COUNT 4
#define DISPLAY_ROW_COUNT 4
#define DISPLAY_BLINK_INTERVAL_MS 500U
#define COLOR_YELLOW 255, 255, 0
#define COLOR_GREEN 0, 255, 0
#define COLOR_RED 255, 0, 0
#define COLOR_BLUE 0, 0, 255
#define COLOR_PURPLE 255, 0, 255

static const uint8_t LED_MASK_0[DISPLAY_ROW_COUNT][DISPLAY_COLUMN_COUNT] = {
    {12, 11, 4, 3},
    {13, 10, 5, 2},
    {14, 9, 6, 1},
    {15, 8, 7, 0},
};
static const uint8_t LED_MASK_90[DISPLAY_ROW_COUNT][DISPLAY_COLUMN_COUNT] = {
    {15, 14, 13, 12},
    {8, 9, 10, 11},
    {7, 6, 5, 4},
    {0, 1, 2, 3},
};
static const uint8_t LED_MASK_180[DISPLAY_ROW_COUNT][DISPLAY_COLUMN_COUNT] = {
    {0, 7, 8, 15},
    {1, 6, 9, 14},
    {2, 5, 10, 13},
    {3, 4, 11, 12},
};
static const uint8_t LED_MASK_270[DISPLAY_ROW_COUNT][DISPLAY_COLUMN_COUNT] = {
    {3, 2, 1, 0},
    {4, 5, 6, 7},
    {11, 10, 9, 8},
    {12, 13, 14, 15},
};

typedef enum {
  DISPLAY_MODE_TIME,
  DISPLAY_MODE_DATE,
  DISPLAY_MODE_ENVIRONMENT,
  DISPLAY_MODE_ALARM,
  DISPLAY_MODE_COUNT,
} DisplayMode;

typedef enum {
  DISPLAY_COLUMN_1,
  DISPLAY_COLUMN_2,
  DISPLAY_COLUMN_3,
  DISPLAY_COLUMN_4,
  DISPLAY_COLUMN_NONE,
} DisplayColumn;

void Display_Init(void);
void Display_Show(uint16_t value, DisplayMode mode);
void Display_SetBlinkColumn(DisplayColumn column);

#endif