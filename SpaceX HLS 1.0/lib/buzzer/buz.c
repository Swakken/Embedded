#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#include <util/delay.h>

#define BUZZER_PIN PD3

void enableBuzzer() { 
  DDRD |= (1 << PD3);
  }

void playTone(float frequency, uint32_t duration) {
    uint32_t period_in_micro = (uint32_t)(1000000 / frequency);
    uint32_t duration_in_micro = duration * 1000;
    
    for (uint32_t time = 0; time < duration_in_micro; time += period_in_micro * 2) {
        PORTD |= (1 << BUZZER_PIN); // Turn buzzer on
        _delay_us(period_in_micro);
        PORTD &= ~(1 << BUZZER_PIN); // Turn buzzer off
        _delay_us(period_in_micro);
    }
}

void buzzerStop() {
    PORTD &= ~(1 << BUZZER_PIN);
}

