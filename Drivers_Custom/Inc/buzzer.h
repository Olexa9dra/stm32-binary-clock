#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f4xx_hal.h"

#define BUZZER_PORT GPIOB
#define BUZZER_PIN GPIO_PIN_0

void Buzzer_Init(void);

void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_Toggle(void);

#endif