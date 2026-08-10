#include "dht11.h"
#include "delay_us.h"

static uint8_t dhtRaw[DHT_DATA_SIZE];

static void DHT11_Start(void);
static uint8_t DHT11_CheckResponse(void);
static void DHT11_SetOutput(void);
static void DHT11_SetInput(void);
static uint8_t DHT11_WaitForPinState(GPIO_PinState state, uint32_t timeoutUs);
static inline GPIO_PinState DHT11_ReadPin(void);
static void DHT11_GPIO_Init(void);

void DHT11_Init(void) {
  DelayUs_Init();
  DHT11_GPIO_Init();
}

uint8_t DHT11_Read(DHT11_Data *data) {
  for (uint8_t i = 0; i < DHT_DATA_SIZE; i++)
    dhtRaw[i] = 0;

  DHT11_Start();

  if (!DHT11_CheckResponse())
    return 0;

  for (uint8_t bitIndex = 0; bitIndex < DHT_DATA_BITS; bitIndex++) {
    if (!DHT11_WaitForPinState(GPIO_PIN_SET, DHT_BIT_TIMEOUT_US))
      return 0;

    uint32_t highStart = Micros();
    if (!DHT11_WaitForPinState(GPIO_PIN_RESET, DHT_BIT_TIMEOUT_US))
      return 0;

    dhtRaw[bitIndex / 8] <<= 1;
    if ((Micros() - highStart) > DHT_BIT_ONE_THRESHOLD_US)
      dhtRaw[bitIndex / 8] |= 1U;
  }

  uint8_t checksum = dhtRaw[0] + dhtRaw[1] + dhtRaw[2] + dhtRaw[3];
  if (dhtRaw[DHT_CHECKSUM_INDEX] != checksum)
    return 0;

  data->humidity = dhtRaw[0];
  data->temperature = dhtRaw[2];

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

static uint8_t DHT11_WaitForPinState(GPIO_PinState state, uint32_t timeoutUs) {
  uint32_t start = Micros();

  while (DHT11_ReadPin() != state) {
    if ((Micros() - start) >= timeoutUs)
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
