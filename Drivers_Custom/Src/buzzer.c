#include "buzzer.h"
#include "main.h"

extern TIM_HandleTypeDef htim2;

void Buzzer_Init(void) {
  if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }

  Buzzer_Off();
}

void Buzzer_PlayNote(BuzzerNote note) {
  static const uint16_t periods[] = {
      727, // C4
      648, // D4
      577, // E4
      545, // F4
      485, // G4
      432, // A4
      385, // B4
      363  // C5
  };

  if (note >= BUZZER_NOTE_C4 && note <= BUZZER_NOTE_C5) {
    uint16_t period = periods[note];

    __HAL_TIM_SET_AUTORELOAD(&htim2, period);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, period / 2);
    __HAL_TIM_SET_COUNTER(&htim2, 0);
  }
}

static void Buzzer_PlayNoteFor(uint8_t note, uint16_t durationMs) {
  Buzzer_PlayNote(note);
  HAL_Delay(durationMs);

  // Short silence between notes
  Buzzer_Off();
  HAL_Delay(25);
}

void Buzzer_PlayMelody(void) {
  // D A G A D A G A D A G E F
  Buzzer_PlayNoteFor(BUZZER_NOTE_D4, 225);
  // Buzzer_PlayNoteFor(BUZZER_NOTE_A4, 225);
  // Buzzer_PlayNoteFor(BUZZER_NOTE_G4, 225);
  // Buzzer_PlayNoteFor(BUZZER_NOTE_A4, 275);
  // HAL_Delay(100);
  // Buzzer_PlayNoteFor(BUZZER_NOTE_D4, 225);
  // Buzzer_PlayNoteFor(BUZZER_NOTE_A4, 225);
  // Buzzer_PlayNoteFor(BUZZER_NOTE_G4, 225);
  // Buzzer_PlayNoteFor(BUZZER_NOTE_A4, 275);
  // HAL_Delay(100);
  // Buzzer_PlayNoteFor(BUZZER_NOTE_D4, 275);
  // Buzzer_PlayNoteFor(BUZZER_NOTE_A4, 275);
  // Buzzer_PlayNoteFor(BUZZER_NOTE_G4, 275);
  // Buzzer_PlayNoteFor(BUZZER_NOTE_E4, 375);
  // Buzzer_PlayNoteFor(BUZZER_NOTE_F4, 450);

  HAL_Delay(100);

  Buzzer_Off();
}

void Buzzer_On(void) { Buzzer_PlayNote(BUZZER_NOTE_A4); }

void Buzzer_Off(void) { __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0); }