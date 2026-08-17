#include "environment.h"
#include "th_sensor.h"

static TH_SENSOR_Data data = {0};
static uint32_t lastUpdate = 0;

void Environment_Init(void) {
  TH_Sensor_Init();
  TH_Sensor_Read(&data);
  lastUpdate = HAL_GetTick();
}

void Environment_Update(void) {
  if (HAL_GetTick() - lastUpdate < ENVIRONMENT_UPDATE_INTERVAL_MS)
    return;
  lastUpdate = HAL_GetTick();
  TH_Sensor_Read(&data);
}

uint16_t Environment_GetDisplayValue(void) {
  uint8_t humidity = (uint8_t)data.humidity;
  uint8_t temperature = (uint8_t)data.temperature;
  uint8_t humidityTens = humidity / 10;
  uint8_t humidityOnes = humidity % 10;
  uint8_t temperatureTens = temperature / 10;
  uint8_t temperatureOnes = temperature % 10;

  return (temperatureOnes << 12) | (temperatureTens << 8) |
         (humidityOnes << 4) | humidityTens;
}