#include "dht11.h"
#include "delay_us.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

static uint8_t dht_raw[5];

static void DHT11_Start(void);
static uint8_t DHT11_CheckResponse(void);
static void DHT11_SetOutput(void);
static void DHT11_SetInput(void);
static uint8_t DHT11_WaitForPinState(GPIO_PinState state, uint32_t timeout_us);
static inline GPIO_PinState DHT11_ReadPin(void);
static void DHT11_GPIO_Init(void);

void DHT11_Init(void) { DHT11_GPIO_Init(); }

uint8_t DHT11_Read(DHT11_Data *data) {
  for (uint8_t i = 0; i < 5; i++)
    dht_raw[i] = 0;

  DHT11_Start();

  if (!DHT11_CheckResponse())
    return 0;

  for (uint8_t bit_index = 0; bit_index < 40; bit_index++) {
    if (!DHT11_WaitForPinState(GPIO_PIN_SET, DHT_BIT_TIMEOUT_US))
      return 0;

    uint32_t high_start = Micros();

    if (!DHT11_WaitForPinState(GPIO_PIN_RESET, DHT_BIT_TIMEOUT_US))
      return 0;

    dht_raw[bit_index / 8] <<= 1;

    if ((Micros() - high_start) > 50U)
      dht_raw[bit_index / 8] |= 1U;
  }

  if (dht_raw[4] !=
      (uint8_t)(dht_raw[0] + dht_raw[1] + dht_raw[2] + dht_raw[3]))
    return 0;

  data->humidity = dht_raw[0];
  data->temperature = dht_raw[2];

  return 1;
}

static void DHT11_Start(void) {
  DHT11_SetOutput();
  HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_RESET);
  Delay_us(DHT_START_LOW_US);
  HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_SET);
  DHT11_SetInput();
}

static uint8_t DHT11_CheckResponse(void) {
  if (!DHT11_WaitForPinState(GPIO_PIN_RESET, DHT_RESPONSE_TIMEOUT_US))
    return 0;

  if (!DHT11_WaitForPinState(GPIO_PIN_SET, DHT_RESPONSE_TIMEOUT_US))
    return 0;

  if (!DHT11_WaitForPinState(GPIO_PIN_RESET, DHT_RESPONSE_TIMEOUT_US))
    return 0;

  return 1;
}

static void DHT11_SetOutput(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = DHT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT_PORT, &GPIO_InitStruct);
}

static void DHT11_SetInput(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = DHT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DHT_PORT, &GPIO_InitStruct);
}

static uint8_t DHT11_WaitForPinState(GPIO_PinState state, uint32_t timeout_us) {
  uint32_t start = Micros();

  while (DHT11_ReadPin() != state) {
    if ((Micros() - start) >= timeout_us)
      return 0;
  }

  return 1;
}

static inline GPIO_PinState DHT11_ReadPin(void) {
  return (DHT_PORT->IDR & DHT_PIN) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

static void DHT11_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_SET);
  GPIO_InitStruct.Pin = DHT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT_PORT, &GPIO_InitStruct);
}
