#include "rtc_ds3231.h"

extern I2C_HandleTypeDef DS3231_I2C;

#define REG_SECONDS 0x00
#define REG_MINUTES 0x01
#define REG_HOURS 0x02
#define REG_DAY 0x03
#define REG_DATE 0x04
#define REG_MONTH 0x05
#define REG_YEAR 0x06

static uint8_t DecToBcd(uint8_t value);
static uint8_t BcdToDec(uint8_t value);

void RTC3231_Init(void) {
  // I2C is initialized by CubeMX.
}

HAL_StatusTypeDef RTC3231_GetTime(RTC_Time *time) {
  uint8_t data[3];

  HAL_StatusTypeDef status =
      HAL_I2C_Mem_Read(&DS3231_I2C, DS3231_ADDRESS, REG_SECONDS,
                       I2C_MEMADD_SIZE_8BIT, data, 3, HAL_MAX_DELAY);

  if (status != HAL_OK)
    return status;

  time->second = BcdToDec(data[0] & 0x7F);
  time->minute = BcdToDec(data[1]);
  time->hour = BcdToDec(data[2] & 0x3F);

  return HAL_OK;
}

HAL_StatusTypeDef RTC3231_SetTime(const RTC_Time *time) {
  uint8_t data[3];

  data[0] = DecToBcd(time->second);
  data[1] = DecToBcd(time->minute);
  data[2] = DecToBcd(time->hour);

  return HAL_I2C_Mem_Write(&DS3231_I2C, DS3231_ADDRESS, REG_SECONDS,
                           I2C_MEMADD_SIZE_8BIT, data, 3, HAL_MAX_DELAY);
}

HAL_StatusTypeDef RTC3231_GetDate(RTC_Date *date) {
  uint8_t data[4];

  HAL_StatusTypeDef status =
      HAL_I2C_Mem_Read(&DS3231_I2C, DS3231_ADDRESS, REG_DAY,
                       I2C_MEMADD_SIZE_8BIT, data, 4, HAL_MAX_DELAY);

  if (status != HAL_OK)
    return status;

  date->day = BcdToDec(data[0]);
  date->date = BcdToDec(data[1]);
  date->month = BcdToDec(data[2] & 0x1F);
  date->year = 2000 + BcdToDec(data[3]);

  return HAL_OK;
}

HAL_StatusTypeDef RTC3231_SetDate(const RTC_Date *date) {
  uint8_t data[4];

  data[0] = DecToBcd(date->day);
  data[1] = DecToBcd(date->date);
  data[2] = DecToBcd(date->month);
  data[3] = DecToBcd(date->year % 100);

  return HAL_I2C_Mem_Write(&DS3231_I2C, DS3231_ADDRESS, REG_DAY,
                           I2C_MEMADD_SIZE_8BIT, data, 4, HAL_MAX_DELAY);
}

static uint8_t DecToBcd(uint8_t value) {
  return ((value / 10) << 4) | (value % 10);
}

static uint8_t BcdToDec(uint8_t value) {
  return ((value >> 4) * 10) + (value & 0x0F);
}