#include "buzzer.h"
#include "main.h"

extern TIM_HandleTypeDef htim4;

void Buzzer_Init(void) {
  if (HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2) != HAL_OK) {
    Error_Handler();
  }

  Buzzer_Off();
}

void Buzzer_On(void) { __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 50); }

void Buzzer_Off(void) { __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 0); }