#ifndef __APP_H
#define __APP_H

#include "display.h"
#include "stm32f4xx_hal.h"

typedef enum {
  UI_STATE_NORMAL,
  UI_STATE_EDIT_TIME,
  UI_STATE_EDIT_DATE,
} UiState;

void App_Init(void);
void App_Run(void);

DisplayMode App_GetDisplayMode(void);
void App_SetDisplayMode(DisplayMode mode);

UiState App_GetUiState(void);
void App_SetUiState(UiState state);

#endif
