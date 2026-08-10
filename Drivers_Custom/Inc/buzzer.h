#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f4xx_hal.h"

typedef enum {
  BUZZER_NOTE_C4,
  BUZZER_NOTE_D4,
  BUZZER_NOTE_E4,
  BUZZER_NOTE_F4,
  BUZZER_NOTE_G4,
  BUZZER_NOTE_A4,
  BUZZER_NOTE_B4,
  BUZZER_NOTE_C5,
} BuzzerNote;

void Buzzer_Init(void);
void Buzzer_PlayMelody(void);
void Buzzer_On(void);
void Buzzer_Off(void);

#endif