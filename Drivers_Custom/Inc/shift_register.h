#ifndef SHIFT_REGISTER_H
#define SHIFT_REGISTER_H

#include "stm32f4xx_hal.h"

#define SHR_GPIO_PORT GPIOD
#define SHR_SR_DATA GPIO_PIN_12
#define SHR_SR_CLK GPIO_PIN_13
#define SHR_SR_LATCH GPIO_PIN_14

void SHR_Init(void);
void ShiftRegister_Write(uint16_t data);

#endif