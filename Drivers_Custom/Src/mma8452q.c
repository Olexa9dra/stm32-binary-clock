#include "mma8452q.h"

extern I2C_HandleTypeDef hi2c1;

static DisplayRotation currentRotation = DISPLAY_ROTATION_0;
static DisplayRotation pendingRotation = DISPLAY_ROTATION_0;
static uint32_t rotationStartTime = 0;

HAL_StatusTypeDef MMA8452Q_ReadWhoAmI(uint8_t *id) {
  return HAL_I2C_Mem_Read(&hi2c1, MMA8452Q_ADDR, MMA8452Q_REG_WHO_AM_I,
                          I2C_MEMADD_SIZE_8BIT, id, 1, MMA8452Q_I2C_TIMEOUT_MS);
}

HAL_StatusTypeDef MMA8452Q_Init(void) {
  uint8_t id;
  if (MMA8452Q_ReadWhoAmI(&id) != HAL_OK)
    return HAL_ERROR;
  if (id != MMA8452Q_WHO_AM_I_VALUE)
    return HAL_ERROR;
  uint8_t active = 0x01;
  return HAL_I2C_Mem_Write(&hi2c1, MMA8452Q_ADDR, MMA8452Q_REG_CTRL_REG1,
                           I2C_MEMADD_SIZE_8BIT, &active, 1,
                           MMA8452Q_I2C_TIMEOUT_MS);
}

HAL_StatusTypeDef MMA8452Q_ReadXYZ(int16_t *x, int16_t *y, int16_t *z) {
  uint8_t data[6];
  HAL_StatusTypeDef status =
      HAL_I2C_Mem_Read(&hi2c1, MMA8452Q_ADDR, MMA8452Q_REG_OUT_X_MSB,
                       I2C_MEMADD_SIZE_8BIT, data, 6, MMA8452Q_I2C_TIMEOUT_MS);
  if (status != HAL_OK)
    return status;

  *x = (int16_t)((data[0] << 8) | data[1]) >> 2;
  *y = (int16_t)((data[2] << 8) | data[3]) >> 2;
  *z = (int16_t)((data[4] << 8) | data[5]) >> 2;

  return HAL_OK;
}

DisplayRotation MMA8452Q_GetRotation(void) {
  int16_t x, y, z;

  if (MMA8452Q_ReadXYZ(&x, &y, &z) != HAL_OK)
    return currentRotation;

  DisplayRotation candidate;

  if (z > MMA8452Q_AXIS_THRESHOLD)
    candidate = DISPLAY_ROTATION_0;
  else if (z < -MMA8452Q_AXIS_THRESHOLD)
    candidate = DISPLAY_ROTATION_180;
  else if (x > MMA8452Q_AXIS_THRESHOLD)
    candidate = DISPLAY_ROTATION_270;
  else if (x < -MMA8452Q_AXIS_THRESHOLD)
    candidate = DISPLAY_ROTATION_90;
  else
    candidate = currentRotation;

  if (candidate == currentRotation) {
    pendingRotation = currentRotation;
    return currentRotation;
  }

  if (candidate != pendingRotation) {
    pendingRotation = candidate;
    rotationStartTime = HAL_GetTick();
    return currentRotation;
  }

  if (HAL_GetTick() - rotationStartTime >= MMA8452Q_ROTATION_STABLE_TIME_MS) {
    currentRotation = pendingRotation;
  }

  return currentRotation;
}