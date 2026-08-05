#include "environment.h"
#include "dht11.h"

static DHT11_Data data;
static uint32_t lastUpdate = 0;

void Environment_Init(void) {
  DHT11_Init();

  // Init value
  data.humidity = 0;
  data.temperature = 0;

  DHT11_Read(&data);
  lastUpdate = HAL_GetTick();
}

void Environment_Update(void) {
  if (HAL_GetTick() - lastUpdate >= UPDATE_TIME) {
    lastUpdate = HAL_GetTick();
    DHT11_Read(&data);
  }
}

uint16_t Environment_GetDisplayValue(void) {
  uint16_t h = data.humidity;
  uint16_t t = data.temperature;
  uint8_t ht = h / 10;
  uint8_t ho = h % 10;
  uint8_t tt = t / 10;
  uint8_t to = t % 10;

  return (to << 12) | (tt << 8) | (ho << 4) | ht;
}
