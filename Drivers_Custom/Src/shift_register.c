#include "shift_register.h"

static void WriteData(uint16_t d);
static void SHR_GPIO_Init(void);

void SHR_Init(void) { SHR_GPIO_Init(); }

void ShiftRegister_Write(uint16_t data) {
  WriteData(data);

  HAL_GPIO_WritePin(SHR_GPIO_PORT, SHR_SR_LATCH, GPIO_PIN_SET);
  HAL_GPIO_WritePin(SHR_GPIO_PORT, SHR_SR_LATCH, GPIO_PIN_RESET);
}

static void WriteData(uint16_t d) {
  for (int i = 15; i >= 0; i--) {
    HAL_GPIO_WritePin(SHR_GPIO_PORT, SHR_SR_DATA,
                      (d & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(SHR_GPIO_PORT, SHR_SR_CLK, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SHR_GPIO_PORT, SHR_SR_CLK, GPIO_PIN_RESET);
  }
}

static void SHR_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  HAL_GPIO_WritePin(SHR_GPIO_PORT, SHR_SR_DATA | SHR_SR_CLK | SHR_SR_LATCH,
                    GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = SHR_SR_DATA | SHR_SR_CLK | SHR_SR_LATCH;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SHR_GPIO_PORT, &GPIO_InitStruct);
}
