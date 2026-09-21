#include "rtc.h"

extern I2C_HandleTypeDef hi2c1;

static uint8_t DecToBcd(uint8_t value);
static uint8_t BcdToDec(uint8_t value);

HAL_StatusTypeDef RTC_GetTime(RTC_Time *time) {
  uint8_t data[3];
  HAL_StatusTypeDef status =
      HAL_I2C_Mem_Read(&hi2c1, RTC_ADDRESS, RTC_REG_SECONDS,
                       I2C_MEMADD_SIZE_8BIT, data, 3, RTC_I2C_TIMEOUT_MS);
  if (status != HAL_OK)
    return status;

  time->second = BcdToDec(data[0] & 0x7F);
  time->minute = BcdToDec(data[1]);
  time->hour = BcdToDec(data[2] & 0x3F);

  return HAL_OK;
}

HAL_StatusTypeDef RTC_SetTime(const RTC_Time *time) {
  uint8_t data[3];

  data[0] = DecToBcd(time->second);
  data[1] = DecToBcd(time->minute);
  data[2] = DecToBcd(time->hour);

  return HAL_I2C_Mem_Write(&hi2c1, RTC_ADDRESS, RTC_REG_SECONDS,
                           I2C_MEMADD_SIZE_8BIT, data, 3, RTC_I2C_TIMEOUT_MS);
}

HAL_StatusTypeDef RTC_GetDate(RTC_Date *date) {
  uint8_t data[4];
  HAL_StatusTypeDef status =
      HAL_I2C_Mem_Read(&hi2c1, RTC_ADDRESS, RTC_REG_DAY, I2C_MEMADD_SIZE_8BIT,
                       data, 4, RTC_I2C_TIMEOUT_MS);
  if (status != HAL_OK)
    return status;

  date->day = BcdToDec(data[0]);
  date->date = BcdToDec(data[1]);
  date->month = BcdToDec(data[2] & 0x1F);
  date->year = RTC_YEAR_OFFSET + BcdToDec(data[3]);

  return HAL_OK;
}

HAL_StatusTypeDef RTC_SetDate(const RTC_Date *date) {
  uint8_t data[4];

  data[0] = DecToBcd(date->day);
  data[1] = DecToBcd(date->date);
  data[2] = DecToBcd(date->month);
  data[3] = DecToBcd(date->year % 100);

  return HAL_I2C_Mem_Write(&hi2c1, RTC_ADDRESS, RTC_REG_DAY,
                           I2C_MEMADD_SIZE_8BIT, data, 4, RTC_I2C_TIMEOUT_MS);
}

static uint8_t DecToBcd(uint8_t value) {
  return ((value / 10) << 4) | (value % 10);
}

static uint8_t BcdToDec(uint8_t value) {
  return ((value >> 4) * 10) + (value & 0x0F);
}