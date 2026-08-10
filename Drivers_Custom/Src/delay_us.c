#include "delay_us.h"

static TIM_HandleTypeDef htim3;

void DelayUs_Init(void) {
  __HAL_RCC_TIM3_CLK_ENABLE();

  uint32_t timer_clk = HAL_RCC_GetPCLK1Freq();

  if (HAL_RCC_GetHCLKFreq() != timer_clk)
    timer_clk *= 2;

  htim3.Instance = DELAY_TIMER;
  htim3.Init.Prescaler = timer_clk / DELAY_TIMER_PRESCALER_HZ - 1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = DELAY_TIMER_PERIOD;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  HAL_TIM_Base_Init(&htim3);
  HAL_TIM_Base_Start(&htim3);
}

void Delay_us(uint16_t us) {
  __HAL_TIM_SET_COUNTER(&htim3, 0);

  while (__HAL_TIM_GET_COUNTER(&htim3) < us)
    ;
}

uint16_t Micros(void) { return __HAL_TIM_GET_COUNTER(&htim3); }