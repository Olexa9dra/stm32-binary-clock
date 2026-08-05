#ifndef DELAY_US_H
#define DELAY_US_H

#include "stm32f4xx_hal.h"

void DelayUs_Init(void);
void Delay_us(uint16_t us);
uint16_t Micros(void);

#endif