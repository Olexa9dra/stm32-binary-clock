#include "display.h"
#include "shift_register.h"
#include <stdint.h>

static DisplayColumn blinkColumn = DISPLAY_COLUMN_NONE;
static uint8_t blinkVisible = 1;
static uint32_t lastBlinkTick = 0;

static uint16_t Display_GetBlinkMask(void);

void Display_Show(uint16_t value) {
  if (HAL_GetTick() - lastBlinkTick >= 500) {
    lastBlinkTick = HAL_GetTick();
    blinkVisible ^= 1;
  }

  if (!blinkVisible && blinkColumn != DISPLAY_COLUMN_NONE)
    value &= ~Display_GetBlinkMask();

  ShiftRegister_Write(value);
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
