#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>


void enableBuzzer();
void playTone(float frequency, uint32_t duration);
void buzzerStop();

#endif