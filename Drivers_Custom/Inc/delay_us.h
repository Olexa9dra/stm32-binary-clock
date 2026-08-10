#ifndef DELAY_US_H
#define DELAY_US_H

#include "stm32f4xx_hal.h"

#define DELAY_TIMER TIM3
#define DELAY_TIMER_PRESCALER_HZ 1000000U
#define DELAY_TIMER_PERIOD 0xFFFF

void DelayUs_Init(void);
void Delay_us(uint16_t us);
uint16_t Micros(void);

#endif