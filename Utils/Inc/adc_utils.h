#ifndef ADC_UTILS_H
#define ADC_UTILS_H

#include "main.h"

#define ADC_UPDATE_INTERVAL_MS 100U
#define ADC_DMA_TIMEOUT_MS 100U

uint16_t ADC_ReadLight(void);
uint16_t ADC_ReadBattery(void);

#endif