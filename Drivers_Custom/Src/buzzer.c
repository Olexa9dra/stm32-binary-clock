#include "buzzer.h"
#include "main.h"

extern TIM_HandleTypeDef htim2;

static uint8_t alarmActive = 0;
static uint8_t alarmInPause = 0;
static uint32_t alarmLastNoteTime = 0;
static uint8_t alarmNote = 0;

static const BuzzerMelodyNote alarmMelody[] = {
    {BUZZER_NOTE_C4, 300}, // 1
    {BUZZER_NOTE_D4, 300}, // 2
    {BUZZER_NOTE_E4, 450}, // 3
    {BUZZER_NOTE_G4, 600}, // 5
    {BUZZER_NOTE_E4, 450}, // 3
    {BUZZER_NOTE_D4, 450}, // 2
    {BUZZER_NOTE_C4, 600}, // 1

    {BUZZER_NOTE_E4, 300}, // 3
    {BUZZER_NOTE_G4, 300}, // 5
    {BUZZER_NOTE_A4, 450}, // 6
    {BUZZER_NOTE_C5, 600}, // 1°
    {BUZZER_NOTE_B4, 450}, // 7
    {BUZZER_NOTE_G4, 450}, // 5
    {BUZZER_NOTE_E4, 600}, // 3
    {BUZZER_NOTE_F4, 150}, // 4
    {BUZZER_NOTE_E4, 150}, // 3
    {BUZZER_NOTE_D4, 600}, // 2

    {BUZZER_NOTE_C4, 300}, // 1
    {BUZZER_NOTE_D4, 300}, // 2
    {BUZZER_NOTE_E4, 450}, // 3
    {BUZZER_NOTE_G4, 600}, // 5
    {BUZZER_NOTE_E4, 300}, // 3
    {BUZZER_NOTE_D4, 300}, // 2
    {BUZZER_NOTE_C4, 450}, // 1
    {BUZZER_NOTE_E4, 300}, // 3
    {BUZZER_NOTE_C4, 600}, // 1

    {BUZZER_NOTE_E4, 300}, // 3
    {BUZZER_NOTE_A4, 300}, // 6
    {BUZZER_NOTE_B4, 450}, // 7
    {BUZZER_NOTE_C5, 600}, // 1°
    {BUZZER_NOTE_B4, 450}, // 7
    {BUZZER_NOTE_G4, 450}, // 5
    {BUZZER_NOTE_E4, 600}, // 3
    {BUZZER_NOTE_F4, 150}, // 4
    {BUZZER_NOTE_E4, 150}, // 3
    {BUZZER_NOTE_D4, 600}, // 2

    {BUZZER_NOTE_C4, 300}, // 1
    {BUZZER_NOTE_D4, 300}, // 2
    {BUZZER_NOTE_E4, 450}, // 3
    {BUZZER_NOTE_G4, 600}, // 5
    {BUZZER_NOTE_E4, 450}, // 3
    {BUZZER_NOTE_D4, 450}, // 2
    {BUZZER_NOTE_C4, 900}, // 1
};

#define ALARM_MELODY_LENGTH (sizeof(alarmMelody) / sizeof(alarmMelody[0]))

#define ALARM_PAUSE_MS 25U

void Buzzer_Init(void) {
  if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }

  Buzzer_Off();
}

void Buzzer_PlayNote(BuzzerNote note) {
  static const uint16_t periods[] = {
      2909, // C2  ~65.4 Hz
      2591, // D2  ~73.4 Hz
      2309, // E2  ~82.4 Hz
      2180, // F2  ~87.3 Hz
      1941, // G2  ~98.0 Hz
      1729, // A2  ~110 Hz
      1541, // B2  ~123.5 Hz
      1455, // C3  ~130.8 Hz
  };

  if (note >= BUZZER_NOTE_C4 && note <= BUZZER_NOTE_C5) {
    uint16_t period = periods[note];

    __HAL_TIM_SET_AUTORELOAD(&htim2, period);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, period / 2);
    __HAL_TIM_SET_COUNTER(&htim2, 0);
  }
}

void Buzzer_On(void) { Buzzer_PlayNote(BUZZER_NOTE_A4); }

void Buzzer_Off(void) { __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0); }

void Buzzer_StartAlarm(void) {
  alarmActive = 1;
  alarmNote = 0;
  alarmInPause = 0;
  alarmLastNoteTime = HAL_GetTick();

  Buzzer_PlayNote(alarmMelody[alarmNote].note);
}

void Buzzer_Update(void) {
  if (!alarmActive)
    return;

  uint32_t now = HAL_GetTick();

  if (!alarmInPause) {
    // Current note is playing.
    if (now - alarmLastNoteTime >= alarmMelody[alarmNote].duration) {
      Buzzer_Off();

      alarmInPause = 1;
      alarmLastNoteTime = now;
    }
  } else {
    // Short silence between notes.
    if (now - alarmLastNoteTime >= ALARM_PAUSE_MS) {
      alarmNote++;

      if (alarmNote >= ALARM_MELODY_LENGTH) {
        alarmNote = 0;
      }

      Buzzer_PlayNote(alarmMelody[alarmNote].note);

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

uint8_t Buzzer_IsAlarmActive(void) { return alarmActive; }