#ifndef __BUTTONS_H
#define __BUTTONS_H

#include "stm32f4xx_hal.h"

#define BUTTON_GPIO_PORT GPIOC
#define BUTTON1_PIN GPIO_PIN_0
#define BUTTON2_PIN GPIO_PIN_1
#define BUTTON3_PIN GPIO_PIN_2
#define BUTTON4_PIN GPIO_PIN_3
#define BUTTON_COUNT 4U
#define BUTTON_DEBOUNCE_MS 20U

typedef struct {
  uint16_t pin;
  GPIO_PinState prevState;
  uint32_t lastPressTime;
} Button;

void Buttons_Init(void);
void Buttons_Check(void);

#endif