#include "environment.h"
#include "dht11.h"

static DHT11_Data data = {0};
static uint32_t lastUpdate = 0;

void Environment_Init(void) {
  DHT11_Init();
  DHT11_Read(&data);
  lastUpdate = HAL_GetTick();
}

void Environment_Update(void) {
  if (HAL_GetTick() - lastUpdate < ENVIRONMENT_UPDATE_INTERVAL_MS)
    return;

  lastUpdate = HAL_GetTick();
  DHT11_Read(&data);
}

uint16_t Environment_GetDisplayValue(void) {
  uint8_t humidityTens = data.humidity / 10;
  uint8_t humidityOnes = data.humidity % 10;
  uint8_t temperatureTens = data.temperature / 10;
  uint8_t temperatureOnes = data.temperature % 10;

  return (temperatureOnes << 12) | (temperatureTens << 8) |
         (humidityOnes << 4) | humidityTens;
}