#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f1xx_hal.h"

#define BUZZER_ALARM_PAUSE_MS 25U
#define BUZZER_NOTE_C2_PERIOD 2909U
#define BUZZER_NOTE_D2_PERIOD 2591U
#define BUZZER_NOTE_E2_PERIOD 2309U
#define BUZZER_NOTE_F2_PERIOD 2180U
#define BUZZER_NOTE_G2_PERIOD 1941U
#define BUZZER_NOTE_A2_PERIOD 1729U
#define BUZZER_NOTE_B2_PERIOD 1541U
#define BUZZER_NOTE_C3_PERIOD 1455U

typedef enum {
  BUZZER_NOTE_C2,
  BUZZER_NOTE_D2,
  BUZZER_NOTE_E2,
  BUZZER_NOTE_F2,
  BUZZER_NOTE_G2,
  BUZZER_NOTE_A2,
  BUZZER_NOTE_B2,
  BUZZER_NOTE_C3,
  BUZZER_NOTE_COUNT,
} BuzzerNote;

typedef struct {
  BuzzerNote note;
  uint16_t duration;
} BuzzerMelodyNote;

// 'Conserning Hobbits'
static const BuzzerMelodyNote BUZZER_ALARM_MELODY[] = {
    {BUZZER_NOTE_C2, 300}, // 1
    {BUZZER_NOTE_D2, 300}, // 2
    {BUZZER_NOTE_E2, 450}, // 3
    {BUZZER_NOTE_G2, 600}, // 5
    {BUZZER_NOTE_E2, 450}, // 3
    {BUZZER_NOTE_D2, 450}, // 2
    {BUZZER_NOTE_C2, 600}, // 1

    {BUZZER_NOTE_E2, 300}, // 3
    {BUZZER_NOTE_G2, 300}, // 5
    {BUZZER_NOTE_A2, 450}, // 6
    {BUZZER_NOTE_C3, 600}, // 1°
    {BUZZER_NOTE_B2, 450}, // 7
    {BUZZER_NOTE_G2, 450}, // 5
    {BUZZER_NOTE_E2, 600}, // 3
    {BUZZER_NOTE_F2, 150}, // 4
    {BUZZER_NOTE_E2, 150}, // 3
    {BUZZER_NOTE_D2, 600}, // 2

    {BUZZER_NOTE_C2, 300}, // 1
    {BUZZER_NOTE_D2, 300}, // 2
    {BUZZER_NOTE_E2, 450}, // 3
    {BUZZER_NOTE_G2, 600}, // 5
    {BUZZER_NOTE_E2, 300}, // 3
    {BUZZER_NOTE_D2, 300}, // 2
    {BUZZER_NOTE_C2, 450}, // 1
    {BUZZER_NOTE_E2, 300}, // 3
    {BUZZER_NOTE_C2, 600}, // 1

    {BUZZER_NOTE_E2, 300}, // 3
    {BUZZER_NOTE_A2, 300}, // 6
    {BUZZER_NOTE_B2, 450}, // 7
    {BUZZER_NOTE_C3, 600}, // 1°
    {BUZZER_NOTE_B2, 450}, // 7
    {BUZZER_NOTE_G2, 450}, // 5
    {BUZZER_NOTE_E2, 600}, // 3
    {BUZZER_NOTE_F2, 150}, // 4
    {BUZZER_NOTE_E2, 150}, // 3
    {BUZZER_NOTE_D2, 600}, // 2

    {BUZZER_NOTE_C2, 300}, // 1
    {BUZZER_NOTE_D2, 300}, // 2
    {BUZZER_NOTE_E2, 450}, // 3
    {BUZZER_NOTE_G2, 600}, // 5
    {BUZZER_NOTE_E2, 450}, // 3
    {BUZZER_NOTE_D2, 450}, // 2
    {BUZZER_NOTE_C2, 900}, // 1
};

#define BUZZER_ALARM_MELODY_LENGTH                                             \
  (sizeof(BUZZER_ALARM_MELODY) / sizeof(BUZZER_ALARM_MELODY[0]))

void Buzzer_Init(void);
void Buzzer_Update(void);
void Buzzer_StartAlarm(void);
void Buzzer_StopAlarm(void);

#endif