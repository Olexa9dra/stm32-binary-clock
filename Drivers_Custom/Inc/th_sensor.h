#ifndef __TH_SENSOR_H
#define __TH_SENSOR_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

#define TH_SENSOR_ADDRESS (0x38 << 1)
#define TH_SENSOR_CMD_INITIALIZE 0xE1U
#define TH_SENSOR_CMD_MEASURE 0xACU
#define TH_SENSOR_CMD_SOFT_RESET 0xBAU
#define TH_SENSOR_INIT_DELAY_MS 20U
#define TH_SENSOR_MEASURE_DELAY_MS 80U
#define TH_SENSOR_MEASURE_ARG_1 0x33U
#define TH_SENSOR_MEASURE_ARG_2 0x00U
#define TH_SENSOR_INIT_ARG_1 0x08U
#define TH_SENSOR_INIT_ARG_2 0x00U
#define TH_SENSOR_RAW_VALUE_MAX 1048576.0f
#define TH_SENSOR_HUMIDITY_MAX 100.0f
#define TH_SENSOR_TEMPERATURE_RANGE 200.0f
#define TH_SENSOR_TEMPERATURE_OFFSET 50.0f

typedef struct {
  float humidity;
  float temperature;
} TH_SENSOR_Data;

HAL_StatusTypeDef TH_Sensor_Init(void);
HAL_StatusTypeDef TH_Sensor_Read(TH_SENSOR_Data *data);

#endif