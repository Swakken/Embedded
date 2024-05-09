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
    int gekozenAantal = 0;
    
    // Blijf in de lus totdat de speler een geldig aantal lucifers kiest
    while (gekozenAantal < 1 || gekozenAantal > maxAantal || gekozenAantal > *startAantal) {
        // Laat de speler het aantal lucifers kiezen om weg te nemen
        // Implementeer de code om de spelerinput te verkrijgen, bijvoorbeeld door knoppen te gebruiken
        
        // Wacht tot de speler de keuze bevestigt door op de tweede knop te drukken
        if (buttonPressed(BUTTON_VERHOOG)) {
            // Verhoog het aantal lucifers dat de speler wil wegnemen
            if (gekozenAantal < maxAantal) {
                gekozenAantal++;
                writeNumberToSegment(1, gekozenAantal);
                _delay_ms(200);
            }
        }
        
        if (buttonPressed(BUTTON_VERLAAG)) {
            // Verlaag het aantal lucifers dat de speler wil wegnemen
            if (gekozenAantal > 1) {
                gekozenAantal--;
                writeNumberToSegment(1, gekozenAantal);
                _delay_ms(200);
            }
        }
        
        if (buttonPressed(BUTTON_PIN)) {
            // Bevestig de keuze en pas het aantal lucifers aan
            *startAantal -= gekozenAantal;
            writeNumberToSegment(2, *startAantal / 10);
            writeNumberToSegment(3, *startAantal % 10);
            _delay_ms(200);
        }
    }
}


void computerBeurt(int *startAantal, int maxAantal) {
    int genomenAantal;

    // Bereken het aantal lucifers dat de computer neemt volgens de strategie van Nim
    if ((*startAantal - 1) % (maxAantal + 1) == 0) {
        // Als de huidige situatie een winnende positie is voor de computer, neem één lucifer
        genomenAantal = 1;
    } else {
        // Zo niet, neem een willekeurig aantal tussen 1 en maxAantal
        genomenAantal = rand() % maxAantal + 1;
    }

    // Pas het aantal lucifers aan door het genomen aantal weg te nemen
    *startAantal -= genomenAantal;
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

    while (1) {
        writeNumberToSegment(2, startAantal / 10);
        writeNumberToSegment(3, startAantal % 10);

        if (speler == 1) {
            writeCharToSegment(1, 'p');
            writeNumberToSegment(0, maxAantal);
//            printf("De beurt is bij de speler");
        } else {
            writeCharToSegment(1, 'c');
//            printf("De beurt is bij de computer");
        }
    }

    return 0;
}




