#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f1xx_hal.h"

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

typedef struct {
  BuzzerNote note;
  uint16_t duration;
} BuzzerMelodyNote;

void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_StartAlarm(void);
void Buzzer_StopAlarm(void);
uint8_t Buzzer_IsAlarmActive(void);
void Buzzer_Update(void);

#endif