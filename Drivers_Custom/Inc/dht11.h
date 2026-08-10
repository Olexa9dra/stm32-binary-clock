#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f4xx_hal.h"

#define DHT_PORT GPIOA
#define DHT_PIN GPIO_PIN_1
#define DHT_RESPONSE_TIMEOUT_US 250U
#define DHT_BIT_TIMEOUT_US 120U
#define DHT_START_LOW_US 20000U
#define DHT_DATA_SIZE 5U
#define DHT_DATA_BITS 40U
#define DHT_CHECKSUM_INDEX 4U
#define DHT_BIT_ONE_THRESHOLD_US 50U

typedef struct {
  uint8_t humidity;
  uint8_t temperature;
} DHT11_Data;

void DHT11_Init(void);
uint8_t DHT11_Read(DHT11_Data *data);

#endif