#include "display.h"
#include "accelerometer.h"
#include "led.h"
#include "light_sensor.h"

static DisplayColumnMask blinkColumn = DISPLAY_COLUMN_NONE;
static uint8_t blinkVisible = 1;
static uint32_t lastBlinkTick = 0;
static DisplayColumnMask errorColumns = DISPLAY_COLUMN_NONE;
static uint8_t errorBlinkVisible = 1;
static uint32_t errorBlinkStart = 0;
static uint32_t lastErrorBlinkTick = 0;

static void Display_SetLedColor(DisplayMode mode, uint8_t column, uint8_t led,
                                uint16_t value);
static const uint8_t (
    *Display_GetLedMask(DisplayRotation rotation))[DISPLAY_COLUMN_COUNT];

static uint8_t Display_ColumnToIndex(DisplayColumnMask column);

void Display_Init(void) {
  LED_Init();
  LightSensor_Init();
}

void Display_Show(uint16_t value, DisplayMode mode) {
  uint32_t now = HAL_GetTick();

  if (errorColumns != DISPLAY_COLUMN_NONE) {
    if (now - errorBlinkStart >= DISPLAY_ERROR_BLINK_DURATION_MS) {
      errorColumns = DISPLAY_COLUMN_NONE;
      errorBlinkVisible = 1;
    } else if (now - lastErrorBlinkTick >= DISPLAY_BLINK_INTERVAL_MS) {
      lastErrorBlinkTick = now;
      errorBlinkVisible ^= 1;
    }

  } else {
    if (now - lastBlinkTick >= DISPLAY_BLINK_INTERVAL_MS) {
      lastBlinkTick = now;
      blinkVisible ^= 1;
    }
  }

  LightSensor_Update();
  LED_SetBrightness(LightSensor_GetBrightness());
  LED_Clear();

  const uint8_t (*mask)[DISPLAY_COLUMN_COUNT] =
      Display_GetLedMask(Accelerometer_GetRotation());
  uint8_t selectedColumn = Display_ColumnToIndex(blinkColumn);

  for (uint8_t column = 0; column < DISPLAY_COLUMN_COUNT; column++) {
    uint8_t nibble = (value >> (column * 4)) & 0x0F;

    if (errorColumns & (1U << column)) {
      if (!errorBlinkVisible)
        continue;
      for (uint8_t row = 0; row < DISPLAY_ROW_COUNT; row++) {
        if (nibble & (1U << row)) {
          uint8_t led = mask[DISPLAY_ROW_COUNT - 1 - row][column];
          LED_SetPixel(led, COLOR_RED);
        }
      }
      continue;
    }

    if (errorColumns == DISPLAY_COLUMN_NONE && column == selectedColumn &&
        !blinkVisible) {
      continue;
    }

    for (uint8_t row = 0; row < DISPLAY_ROW_COUNT; row++) {
      if (nibble & (1U << row)) {
        uint8_t led = mask[DISPLAY_ROW_COUNT - 1 - row][column];
        Display_SetLedColor(mode, column, led, value);
      }
    }
  }

  LED_Show();
}

void Display_SetBlinkColumn(DisplayColumnMask column) {
  blinkColumn = column;
  blinkVisible = 1;
  lastBlinkTick = HAL_GetTick();
}

void Display_StartErrorBlink(DisplayColumnMask columns) {
  if (columns == DISPLAY_COLUMN_NONE)
    return;

  uint32_t now = HAL_GetTick();
  errorColumns = columns;
  errorBlinkVisible = 1;
  errorBlinkStart = now;
  lastErrorBlinkTick = now;
}

static uint8_t Display_ColumnToIndex(DisplayColumnMask column) {
  switch (column) {
  case DISPLAY_COLUMN_1:
    return 0;
  case DISPLAY_COLUMN_2:
    return 1;
  case DISPLAY_COLUMN_3:
    return 2;
  case DISPLAY_COLUMN_4:
    return 3;
  default:
    return DISPLAY_COLUMN_COUNT;
  }
}

static void Display_SetLedColor(DisplayMode mode, uint8_t column, uint8_t led,
                                uint16_t value) {
  switch (mode) {
  case DISPLAY_MODE_TIME:
    LED_SetPixel(led, COLOR_YELLOW);
    break;
  case DISPLAY_MODE_DATE:
    LED_SetPixel(led, COLOR_GREEN);
    break;
  case DISPLAY_MODE_ENVIRONMENT:
    if (column < 2)
      LED_SetPixel(led, COLOR_BLUE);
    else
      LED_SetPixel(led, COLOR_RED);
    break;
  case DISPLAY_MODE_ALARM:
    LED_SetPixel(led, COLOR_PURPLE);
    break;
  case DISPLAY_MODE_BATTERY:
    if (value > 15)
      LED_SetPixel(led, COLOR_GREEN);
    else
      LED_SetPixel(led, COLOR_RED);
    break;
  default:
    LED_SetPixel(led, COLOR_YELLOW);
    break;
  }
}

static const uint8_t (*Display_GetLedMask(DisplayRotation rotation))
    [DISPLAY_COLUMN_COUNT] {
      switch (rotation) {
      case DISPLAY_ROTATION_0:
        return LED_MASK_0;
      case DISPLAY_ROTATION_90:
        return LED_MASK_90;
      case DISPLAY_ROTATION_180:
        return LED_MASK_180;
      case DISPLAY_ROTATION_270:
      default:
        return LED_MASK_270;
      }
    }