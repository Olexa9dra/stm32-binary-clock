#ifndef __AHT10_H
#define __AHT10_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define AHT10_ADDRESS (0x38 << 1)

#define AHT10_CMD_INITIALIZE 0xE1U
#define AHT10_CMD_MEASURE 0xACU
#define AHT10_CMD_SOFT_RESET 0xBAU

#define AHT10_INIT_DELAY_MS 20U
#define AHT10_MEASURE_DELAY_MS 80U

#define AHT10_MEASURE_ARG_1 0x33U
#define AHT10_MEASURE_ARG_2 0x00U

#define AHT10_INIT_ARG_1 0x08U
#define AHT10_INIT_ARG_2 0x00U

#define AHT10_RAW_VALUE_MAX 1048576.0f
#define AHT10_HUMIDITY_MAX 100.0f
#define AHT10_TEMPERATURE_RANGE 200.0f
#define AHT10_TEMPERATURE_OFFSET 50.0f

typedef struct {
  float humidity;
  float temperature;
} AHT10_Data;

HAL_StatusTypeDef AHT10_Init(void);
HAL_StatusTypeDef AHT10_Read(AHT10_Data *data);

#endif