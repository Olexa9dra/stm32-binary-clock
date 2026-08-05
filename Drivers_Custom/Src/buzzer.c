#include "buzzer.h"

static void Buzzer_GPIO_Init(void);

void Buzzer_Init(void) {
  Buzzer_GPIO_Init();
  Buzzer_Off();
}

void Buzzer_On(void) {
  HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
}

void Buzzer_Off(void) {
  HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
}

void Buzzer_Toggle(void) { HAL_GPIO_TogglePin(BUZZER_PORT, BUZZER_PIN); }

static void Buzzer_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = BUZZER_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);
}