#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <usart.h>
#include <pot.h>
#include <stdlib.h>
#include "display.h"

#define BUTTON_PIN PIND2

void initButton() {
    DDRD &= ~(1 << BUTTON_PIN);
    PORTD |= (1 << BUTTON_PIN);
}

int buttonPressed() {
    return (PIND & (1 << BUTTON_PIN)) == 0;
}

int main() {
    initUSART();
    startPotentiometer();
    initDisplay(); 
    initButton();

    uint16_t value = 0;
    unsigned int seed = 0;

    printf("Draai aan de potentiometer om de seed te kiezen en druk op de knop om te starten.\n");

    while (!buttonPressed()) {
        value = readPotentiometer();
        seed = value % 10000;
        writeNumberAndWait(seed, 200);
    }

    srand(seed);
    printf("Game started with seed: %u\n", seed);

    // Start hier het spel ...

    return 0;
}
