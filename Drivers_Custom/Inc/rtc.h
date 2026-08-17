#ifndef __RTC_H
#define __RTC_H

#include "stm32f1xx_hal.h"

#define RTC_ADDRESS (0x68 << 1)
#define RTC_REG_SECONDS 0x00
#define RTC_REG_MINUTES 0x01
#define RTC_REG_HOURS 0x02
#define RTC_REG_DAY 0x03
#define RTC_REG_DATE 0x04
#define RTC_REG_MONTH 0x05
#define RTC_REG_YEAR 0x06
#define RTC_I2C_TIMEOUT_MS HAL_MAX_DELAY
#define RTC_YEAR_OFFSET 2000U

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

HAL_StatusTypeDef RTC_GetTime(RTC_Time *time);
HAL_StatusTypeDef RTC_SetTime(const RTC_Time *time);
HAL_StatusTypeDef RTC_GetDate(RTC_Date *date);
HAL_StatusTypeDef RTC_SetDate(const RTC_Date *date);

#endif