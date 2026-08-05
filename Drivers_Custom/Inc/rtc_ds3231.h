#ifndef __RTC_DS3231_H
#define __RTC_DS3231_H

#include "stm32f4xx_hal.h"

#define DS3231_I2C hi2c1
#define DS3231_ADDRESS (0x68 << 1)

typedef struct {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
} RTC_Time;

typedef struct {
  uint8_t day;
  uint8_t date;
  uint8_t month;
  uint16_t year;
} RTC_Date;

void RTC3231_Init(void);

HAL_StatusTypeDef RTC3231_GetTime(RTC_Time *time);
HAL_StatusTypeDef RTC3231_SetTime(const RTC_Time *time);

HAL_StatusTypeDef RTC3231_GetDate(RTC_Date *date);
HAL_StatusTypeDef RTC3231_SetDate(const RTC_Date *date);

#endif