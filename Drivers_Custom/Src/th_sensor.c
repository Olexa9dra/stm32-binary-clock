#include "th_sensor.h"

extern I2C_HandleTypeDef hi2c1;

static HAL_StatusTypeDef TH_Sensor_SendCommand(uint8_t *command, uint16_t size);

HAL_StatusTypeDef TH_Sensor_Init(void) {
  uint8_t command = TH_SENSOR_CMD_SOFT_RESET;
  HAL_StatusTypeDef status = TH_Sensor_SendCommand(&command, 1);
  if (status != HAL_OK)
    return status;

  HAL_Delay(TH_SENSOR_INIT_DELAY_MS);

  uint8_t initCommand[] = {TH_SENSOR_CMD_INITIALIZE, TH_SENSOR_INIT_ARG_1,
                           TH_SENSOR_INIT_ARG_2};
  status = TH_Sensor_SendCommand(initCommand, 3);

  HAL_Delay(TH_SENSOR_INIT_DELAY_MS);

  return status;
}

HAL_StatusTypeDef TH_Sensor_Read(TH_SENSOR_Data *data) {
  if (data == NULL)
    return HAL_ERROR;

  uint8_t command[] = {TH_SENSOR_CMD_MEASURE, TH_SENSOR_MEASURE_ARG_1,
                       TH_SENSOR_MEASURE_ARG_2};
  HAL_StatusTypeDef status = TH_Sensor_SendCommand(command, 3);
  if (status != HAL_OK)
    return status;

  HAL_Delay(TH_SENSOR_MEASURE_DELAY_MS);

  uint8_t buffer[6];
  status = HAL_I2C_Master_Receive(&hi2c1, TH_SENSOR_ADDRESS, buffer, 6,
                                  HAL_MAX_DELAY);
  if (status != HAL_OK)
    return status;
  // Sensor is still busy.
  if (buffer[0] & 0x80U)
    return HAL_BUSY;

  uint32_t rawHumidity = ((uint32_t)buffer[1] << 12) |
                         ((uint32_t)buffer[2] << 4) |
                         ((buffer[3] >> 4) & 0x0FU);
  uint32_t rawTemperature = ((uint32_t)(buffer[3] & 0x0FU) << 16) |
                            ((uint32_t)buffer[4] << 8) | buffer[5];

  data->humidity =
      ((float)rawHumidity * TH_SENSOR_HUMIDITY_MAX) / TH_SENSOR_RAW_VALUE_MAX;
  data->temperature = ((float)rawTemperature * TH_SENSOR_TEMPERATURE_RANGE) /
                          TH_SENSOR_RAW_VALUE_MAX -
                      TH_SENSOR_TEMPERATURE_OFFSET;

  return HAL_OK;
}

static HAL_StatusTypeDef TH_Sensor_SendCommand(uint8_t *command,
                                               uint16_t size) {
  return HAL_I2C_Master_Transmit(&hi2c1, TH_SENSOR_ADDRESS, command, size,
                                 HAL_MAX_DELAY);
}
