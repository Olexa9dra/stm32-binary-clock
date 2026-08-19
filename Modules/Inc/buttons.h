#ifndef __BUTTONS_H
#define __BUTTONS_H

#include "stm32f1xx_hal.h"

#define BUTTON_GPIO_PORT GPIOA
#define BUTTON1_PIN GPIO_PIN_1
#define BUTTON2_PIN GPIO_PIN_2
#define BUTTON3_PIN GPIO_PIN_3
#define BUTTON4_PIN GPIO_PIN_4
#define BUTTON_COUNT 4U
#define BUTTON_DEBOUNCE_MS 20U

static const uint8_t BUTTON_MASK_0[BUTTON_COUNT] = {0, 1, 2, 3};
static const uint8_t BUTTON_MASK_90[BUTTON_COUNT] = {0, 1, 2, 3};
static const uint8_t BUTTON_MASK_180[BUTTON_COUNT] = {3, 2, 1, 0};
static const uint8_t BUTTON_MASK_270[BUTTON_COUNT] = {3, 2, 1, 0};

typedef struct {
  uint16_t pin;
  GPIO_PinState prevState;
  uint32_t lastPressTime;
} Button;

void Buttons_Check(void);

#endif