#ifndef __RTC_DS3231_H
#define __RTC_DS3231_H

#include "stm32f1xx_hal.h"

#define DS3231_ADDRESS (0x68 << 1)
#define DS3231_REG_SECONDS 0x00
#define DS3231_REG_MINUTES 0x01
#define DS3231_REG_HOURS 0x02
#define DS3231_REG_DAY 0x03
#define DS3231_REG_DATE 0x04
#define DS3231_REG_MONTH 0x05
#define DS3231_REG_YEAR 0x06
#define DS3231_I2C_TIMEOUT_MS HAL_MAX_DELAY
#define DS3231_YEAR_OFFSET 2000U

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

HAL_StatusTypeDef RTC3231_GetTime(RTC_Time *time);
HAL_StatusTypeDef RTC3231_SetTime(const RTC_Time *time);
HAL_StatusTypeDef RTC3231_GetDate(RTC_Date *date);
HAL_StatusTypeDef RTC3231_SetDate(const RTC_Date *date);

#endif