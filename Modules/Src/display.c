#include "display.h"
#include "ws2812.h"

static DisplayColumn blinkColumn = DISPLAY_COLUMN_NONE;
static uint8_t blinkVisible = 1;
static uint32_t lastBlinkTick = 0;

static const uint8_t ledMask[4][4] = {
    {12, 11, 4, 3},
    {13, 10, 5, 2},
    {14, 9, 6, 1},
    {15, 8, 7, 0},
};

static uint16_t Display_GetBlinkMask(void);
static void Display_SetLedColor(DisplayMode mode, uint8_t column, uint8_t led);

void Display_Show(uint16_t value, DisplayMode mode) {
  if (HAL_GetTick() - lastBlinkTick >= 500) {
    lastBlinkTick = HAL_GetTick();
    blinkVisible ^= 1;
  }

  if (!blinkVisible && blinkColumn != DISPLAY_COLUMN_NONE)
    value &= ~Display_GetBlinkMask();

  WS2812_Clear();

  for (uint8_t column = 0; column < 4; column++) {
    uint8_t nibble = (value >> (column * 4)) & 0x0F;
    for (uint8_t row = 0; row < 4; row++) {
      if (nibble & (1 << row)) {
        uint8_t led = ledMask[3 - row][column];
        Display_SetLedColor(mode, column, led);
      }
    }
  }

  WS2812_Show();
}

void Display_SetBlinkColumn(DisplayColumn column) { blinkColumn = column; }

static uint16_t Display_GetBlinkMask(void) {
  switch (blinkColumn) {
  case DISPLAY_COLUMN_1:
    return 0x000F;
  case DISPLAY_COLUMN_2:
    return 0x00F0;
  case DISPLAY_COLUMN_3:
    return 0x0F00;
  case DISPLAY_COLUMN_4:
    return 0xF000;
  default:
    return 0xFFFF;
  }
}

static void Display_SetLedColor(DisplayMode mode, uint8_t column, uint8_t led) {
  switch (mode) {
  case DISPLAY_MODE_TIME:
    WS2812_SetPixel(led, COLOR_YELLOW);
    break;
  case DISPLAY_MODE_DATE:
    WS2812_SetPixel(led, COLOR_GREEN);
    break;
  case DISPLAY_MODE_ENVIRONMENT:
    // columns 0-1 = humidity
    if (column < 2)
      WS2812_SetPixel(led, COLOR_BLUE);
    else
      WS2812_SetPixel(led, COLOR_RED);
    break;
  default:
    WS2812_SetPixel(led, COLOR_YELLOW);
  }
}