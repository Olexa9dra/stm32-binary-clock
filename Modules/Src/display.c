#include "display.h"
#include "accelerometer.h"
#include "led.h"
#include "light_sensor.h"

static DisplayColumn blinkColumn = DISPLAY_COLUMN_NONE;
static uint8_t blinkVisible = 1;
static uint32_t lastBlinkTick = 0;

static uint16_t Display_GetBlinkMask(void);
static void Display_SetLedColor(DisplayMode mode, uint8_t column, uint8_t led);
static const uint8_t (
    *Display_GetLedMask(DisplayRotation rotation))[DISPLAY_COLUMN_COUNT];

void Display_Init(void) {
  LED_Init();
  LightSensor_Init();
}

void Display_Show(uint16_t value, DisplayMode mode) {
  if (HAL_GetTick() - lastBlinkTick >= DISPLAY_BLINK_INTERVAL_MS) {
    lastBlinkTick = HAL_GetTick();
    blinkVisible ^= 1;
  }
  if (!blinkVisible && blinkColumn != DISPLAY_COLUMN_NONE)
    value &= ~Display_GetBlinkMask();

  LightSensor_Update();
  LED_SetBrightness(LightSensor_GetBrightness());
  LED_Clear();

  const uint8_t (*mask)[DISPLAY_COLUMN_COUNT] =
      Display_GetLedMask(Accelerometer_GetRotation());

  for (uint8_t column = 0; column < DISPLAY_COLUMN_COUNT; column++) {
    uint8_t nibble = (value >> (column * 4)) & 0x0F;
    for (uint8_t row = 0; row < DISPLAY_ROW_COUNT; row++) {
      if (nibble & (1U << row)) {
        uint8_t led = mask[DISPLAY_ROW_COUNT - 1 - row][column];
        Display_SetLedColor(mode, column, led);
      }
    }
  }

  LED_Show();
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
