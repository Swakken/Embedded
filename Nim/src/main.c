#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <usart.h>
#include <pot.h>
#include "display.h"
#include <button.h>

#define BUTTON_PIN PCINT1
#define BUTTON_VERHOOG PCINT2
#define BUTTON_VERLAAG PCINT0

void initButton() {
    DDRC &= ~(1 << BUTTON_PIN);
    PORTC |= (1 << BUTTON_PIN);
}

int buttonPressed() {
    return (PINC & (1 << BUTTON_PIN)) == 0;
}

void verhoogKeuze(int *value, int maxValue) {
    if (buttonPressed(BUTTON_VERHOOG) && *value < maxValue) {
        (*value)++;
        writeNumberToSegment(1, *value);
        _delay_ms(200);
    }
}

void verlaagKeuze(int *value) {
    if (buttonPressed(BUTTON_VERLAAG) && *value > 1) {
        (*value)--;
        writeNumberToSegment(1, *value);
        _delay_ms(200);
    }
}

void bevestigKeuze(int *total, int value) {
    if (buttonPressed(BUTTON_PIN)) {
        *total -= value;
        writeNumberToSegment(2, *total / 10);
        writeNumberToSegment(3, *total % 10);
        _delay_ms(200);
    }
}

void spelerBeurt(int *startAantal, int maxAantal) {
    int aantalGekozen = 1; // Begin met het standaard aantal lucifers

    // Blijf in de lus totdat de speler een geldig aantal lucifers kiest
    while (1) {
        // Toon het gekozen aantal lucifers op het display
        writeNumberToSegment(1, aantalGekozen);

        // Knop 3 verhoogt het gekozen aantal lucifers (niet hoger dan maxAantal)
        if (buttonPressed(BUTTON_VERHOOG) && aantalGekozen < maxAantal) {
            aantalGekozen++;
            _delay_ms(200);
        }

        // Knop 1 verlaagt het gekozen aantal lucifers (niet lager dan 1)
        if (buttonPressed(BUTTON_VERLAAG) && aantalGekozen > 1) {
            aantalGekozen--;
            _delay_ms(200);
        }

        // Wacht op de bevestiging van de speler door op knop 2 te drukken
        if (buttonPressed(BUTTON_PIN)) {
            // Pas het aantal lucifers aan door het gekozen aantal weg te nemen
            *startAantal -= aantalGekozen;
            // Toon het nieuwe aantal lucifers op het display
            writeNumberToSegment(2, *startAantal / 10);
            writeNumberToSegment(3, *startAantal % 10);
            _delay_ms(200);
            break; // Verlaat de lus zodra de keuze is bevestigd
        }
    }
}

void computerBeurt(int *startAantal, int maxAantal) {
    int aantalGekozen;

    // Bereken het aantal lucifers dat de computer neemt volgens de strategie van Nim
    if ((*startAantal - 1) % (maxAantal + 1) == 0) {
        // Als de huidige situatie een winnende positie is voor de computer, neem één lucifer
        aantalGekozen = 1;
    } else {
        // Zo niet, neem een willekeurig aantal tussen 1 en maxAantal
        aantalGekozen = rand() % maxAantal + 1;
    }

    // Toon het aantal lucifers dat de computer neemt op het meest linkse display
    writeNumberToSegment(0, aantalGekozen);

    // Wacht op bevestiging van de speler door op knop 2 te drukken
    while (!buttonPressed(BUTTON_PIN)) {
        // Wacht tot de speler de bevestigingsknop indrukt
    }

    // Toon in de terminal dat het de beurt is van de computer
    printf("De beurt is aan de computer.\n");

    // Toon het aantal lucifers dat de computer heeft gekozen in de terminal
    printf("De computer kiest %d lucifers.\n", aantalGekozen);

    // Wacht opnieuw op bevestiging van de speler door op knop 2 te drukken
    while (!buttonPressed(BUTTON_PIN)) {
        // Wacht tot de speler de bevestigingsknop indrukt
    }

    // Pas het aantal lucifers aan door het genomen aantal weg te nemen
    *startAantal -= aantalGekozen;

    // Toon het nieuwe aantal lucifers op het display
    writeNumberToSegment(2, *startAantal / 10);
    writeNumberToSegment(3, *startAantal % 10);
    _delay_ms(200);
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
        writeNumberAndWait(value % 10000, 200);
    }

    seed = value % 10000;
    srand(seed);
    printf("Game started with seed: %u\n", seed);

    int startAantal = 21;
    int maxAantal = (rand() % 3) + 1;
    int aantalGekozen = 1;
    // 0 voor C en 1 voor P
    int speler = rand() % 2;

    // Toon het startaantal en de letter die de beurt aangeeft op het display
    writeNumberToSegment(2, startAantal / 10);
    writeNumberToSegment(3, startAantal % 10);
    if (speler == 1) {
        writeCharToSegment(1, 'p');
    } else {
        writeCharToSegment(1, 'c');
    }

    while (1) {
        if (speler == 1) {
            writeCharToSegment(1, 'p');
            writeNumberToSegment(0, maxAantal);
            spelerBeurt(&startAantal, maxAantal);
            speler = 0; // Verander de beurt naar de computer na de speler's beurt
        } else {
            writeCharToSegment(1, 'c');
            computerBeurt(&startAantal, maxAantal);
            speler = 1; // Verander de beurt naar de speler na de computer's beurt
        }
    }

    return 0;
}
