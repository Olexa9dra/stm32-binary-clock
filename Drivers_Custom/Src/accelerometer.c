#include "accelerometer.h"

extern I2C_HandleTypeDef hi2c1;

static DisplayRotation currentRotation = DISPLAY_ROTATION_0;
static DisplayRotation pendingRotation = DISPLAY_ROTATION_0;
static uint32_t rotationStartTime = 0;

HAL_StatusTypeDef Accelerometer_ReadWhoAmI(uint8_t *id) {
  return HAL_I2C_Mem_Read(&hi2c1, ACCELEROMETER_ADDR,
                          ACCELEROMETER_REG_WHO_AM_I, I2C_MEMADD_SIZE_8BIT, id,
                          1, ACCELEROMETER_I2C_TIMEOUT_MS);
}

HAL_StatusTypeDef Accelerometer_Init(void) {
  uint8_t id;
  if (Accelerometer_ReadWhoAmI(&id) != HAL_OK)
    return HAL_ERROR;
  if (id != ACCELEROMETER_WHO_AM_I_VALUE)
    return HAL_ERROR;
  uint8_t active = 0x01;
  return HAL_I2C_Mem_Write(&hi2c1, ACCELEROMETER_ADDR,
                           ACCELEROMETER_REG_CTRL_REG1, I2C_MEMADD_SIZE_8BIT,
                           &active, 1, ACCELEROMETER_I2C_TIMEOUT_MS);
}

HAL_StatusTypeDef Accelerometer_ReadXYZ(int16_t *x, int16_t *y, int16_t *z) {
  uint8_t data[6];
  HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
      &hi2c1, ACCELEROMETER_ADDR, ACCELEROMETER_REG_OUT_X_MSB,
      I2C_MEMADD_SIZE_8BIT, data, 6, ACCELEROMETER_I2C_TIMEOUT_MS);
  if (status != HAL_OK)
    return status;

  *x = (int16_t)((data[0] << 8) | data[1]) >> 2;
  *y = (int16_t)((data[2] << 8) | data[3]) >> 2;
  *z = (int16_t)((data[4] << 8) | data[5]) >> 2;

  return HAL_OK;
}

DisplayRotation Accelerometer_GetRotation(void) {
  int16_t x, y, z;

  if (Accelerometer_ReadXYZ(&x, &y, &z) != HAL_OK)
    return currentRotation;

  DisplayRotation candidate;

  if (z > ACCELEROMETER_AXIS_THRESHOLD)
    candidate = DISPLAY_ROTATION_0;
  else if (z < -ACCELEROMETER_AXIS_THRESHOLD)
    candidate = DISPLAY_ROTATION_180;
  else if (x > ACCELEROMETER_AXIS_THRESHOLD)
    candidate = DISPLAY_ROTATION_270;
  else if (x < -ACCELEROMETER_AXIS_THRESHOLD)
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

  if (HAL_GetTick() - rotationStartTime >=
      ACCELEROMETER_ROTATION_STABLE_TIME_MS) {
    currentRotation = pendingRotation;
  }

  return currentRotation;
}

HAL_StatusTypeDef Accelerometer_EnableTapDetection(void) {
  uint8_t ctrl_reg1 = 0x00;

  // Put device into standby before changing interrupt configuration.
  if (HAL_I2C_Mem_Write(&hi2c1, ACCELEROMETER_ADDR, ACCELEROMETER_REG_CTRL_REG1,
                        I2C_MEMADD_SIZE_8BIT, &ctrl_reg1, 1,
                        ACCELEROMETER_I2C_TIMEOUT_MS) != HAL_OK) {
    return HAL_ERROR;
  }

  // Enable single/double pulse detection on X/Y/Z.
  uint8_t pulse_cfg = 0x3F;

  if (HAL_I2C_Mem_Write(&hi2c1, ACCELEROMETER_ADDR, ACCELEROMETER_REG_PULSE_CFG,
                        I2C_MEMADD_SIZE_8BIT, &pulse_cfg, 1,
                        ACCELEROMETER_I2C_TIMEOUT_MS) != HAL_OK) {
    return HAL_ERROR;
  }

  // Threshold.
  uint8_t pulse_ths = 0x20;

  if (HAL_I2C_Mem_Write(&hi2c1, ACCELEROMETER_ADDR,
                        ACCELEROMETER_REG_PULSE_THSX, I2C_MEMADD_SIZE_8BIT,
                        &pulse_ths, 1,
                        ACCELEROMETER_I2C_TIMEOUT_MS) != HAL_OK) {
    return HAL_ERROR;
  }

  if (HAL_I2C_Mem_Write(&hi2c1, ACCELEROMETER_ADDR,
                        ACCELEROMETER_REG_PULSE_THSY, I2C_MEMADD_SIZE_8BIT,
                        &pulse_ths, 1,
                        ACCELEROMETER_I2C_TIMEOUT_MS) != HAL_OK) {
    return HAL_ERROR;
  }

  if (HAL_I2C_Mem_Write(&hi2c1, ACCELEROMETER_ADDR,
                        ACCELEROMETER_REG_PULSE_THSZ, I2C_MEMADD_SIZE_8BIT,
                        &pulse_ths, 1,
                        ACCELEROMETER_I2C_TIMEOUT_MS) != HAL_OK) {
    return HAL_ERROR;
  }

  // Pulse timing.
  uint8_t pulse_tmlt = 0x20;
  uint8_t pulse_ltcy = 0x20;
  uint8_t pulse_wind = 0x40;

  HAL_I2C_Mem_Write(&hi2c1, ACCELEROMETER_ADDR, ACCELEROMETER_REG_PULSE_TMLT,
                    I2C_MEMADD_SIZE_8BIT, &pulse_tmlt, 1,
                    ACCELEROMETER_I2C_TIMEOUT_MS);

  HAL_I2C_Mem_Write(&hi2c1, ACCELEROMETER_ADDR, ACCELEROMETER_REG_PULSE_LTCY,
                    I2C_MEMADD_SIZE_8BIT, &pulse_ltcy, 1,
                    ACCELEROMETER_I2C_TIMEOUT_MS);

  HAL_I2C_Mem_Write(&hi2c1, ACCELEROMETER_ADDR, ACCELEROMETER_REG_PULSE_WIND,
                    I2C_MEMADD_SIZE_8BIT, &pulse_wind, 1,
                    ACCELEROMETER_I2C_TIMEOUT_MS);

  // Return to active mode.
  ctrl_reg1 = 0x01;

  return HAL_I2C_Mem_Write(&hi2c1, ACCELEROMETER_ADDR,
                           ACCELEROMETER_REG_CTRL_REG1, I2C_MEMADD_SIZE_8BIT,
                           &ctrl_reg1, 1, ACCELEROMETER_I2C_TIMEOUT_MS);
}

uint8_t Accelerometer_TapDetected(void) {
  uint8_t pulseSource = 0;

  if (HAL_I2C_Mem_Read(&hi2c1, ACCELEROMETER_ADDR, ACCELEROMETER_REG_PULSE_SRC,
                       I2C_MEMADD_SIZE_8BIT, &pulseSource, 1,
                       ACCELEROMETER_I2C_TIMEOUT_MS) != HAL_OK) {
    return 0;
  }

  return (pulseSource & 0x40U) != 0 ? 1 : 0;
}