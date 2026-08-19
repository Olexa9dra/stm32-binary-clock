#include "buzzer.h"
#include "main.h"

extern TIM_HandleTypeDef htim2;

static uint8_t alarmActive = 0;
static uint8_t alarmInPause = 0;
static uint32_t alarmLastNoteTime = 0;
static uint8_t alarmNote = 0;

static const uint16_t buzzerNotePeriods[BUZZER_NOTE_COUNT] = {
    [BUZZER_NOTE_C2] = BUZZER_NOTE_C2_PERIOD,
    [BUZZER_NOTE_D2] = BUZZER_NOTE_D2_PERIOD,
    [BUZZER_NOTE_E2] = BUZZER_NOTE_E2_PERIOD,
    [BUZZER_NOTE_F2] = BUZZER_NOTE_F2_PERIOD,
    [BUZZER_NOTE_G2] = BUZZER_NOTE_G2_PERIOD,
    [BUZZER_NOTE_A2] = BUZZER_NOTE_A2_PERIOD,
    [BUZZER_NOTE_B2] = BUZZER_NOTE_B2_PERIOD,
    [BUZZER_NOTE_C3] = BUZZER_NOTE_C3_PERIOD,
};

void Buzzer_Off(void);

void Buzzer_Init(void) {
  if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }

  Buzzer_Off();
}

void Buzzer_Off(void) { __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0); }

void Buzzer_PlayNote(BuzzerNote note) {
  if (note >= BUZZER_NOTE_COUNT)
    return;

  uint16_t period = buzzerNotePeriods[note];

  __HAL_TIM_SET_AUTORELOAD(&htim2, period);
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, period / 2);
  __HAL_TIM_SET_COUNTER(&htim2, 0);
}

void Buzzer_StartAlarm(void) {
  alarmActive = 1;
  alarmNote = 0;
  alarmInPause = 0;
  alarmLastNoteTime = HAL_GetTick();

  Buzzer_PlayNote(BUZZER_ALARM_MELODY[alarmNote].note);
}

void Buzzer_Update(void) {
  if (!alarmActive)
    return;

  uint32_t now = HAL_GetTick();

  if (!alarmInPause) {
    // Current note is playing.
    if (now - alarmLastNoteTime >= BUZZER_ALARM_MELODY[alarmNote].duration) {
      Buzzer_Off();
      alarmInPause = 1;
      alarmLastNoteTime = now;
    }
  } else {
    // Short silence between notes.
    if (now - alarmLastNoteTime >= BUZZER_ALARM_PAUSE_MS) {
      alarmNote++;
      if (alarmNote >= BUZZER_ALARM_MELODY_LENGTH) {
        alarmNote = 0;
      }
      Buzzer_PlayNote(BUZZER_ALARM_MELODY[alarmNote].note);
      alarmInPause = 0;
      alarmLastNoteTime = now;
    }
  }
}

void Buzzer_StopAlarm(void) {
  alarmActive = 0;
  alarmInPause = 0;
  Buzzer_Off();
}