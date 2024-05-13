#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <usart.h>
#include <pot.h>
#include "display.h"
#include <button.h>

#define BUTTON_PIN PC1
#define BUTTON_VERHOOG PC2
#define BUTTON_VERLAAG PC0

void initButton() {
    DDRC &= ~((1 << BUTTON_PIN) | (1 << BUTTON_VERHOOG) | (1 << BUTTON_VERLAAG));
    PORTC |= (1 << BUTTON_PIN) | (1 << BUTTON_VERHOOG) | (1 << BUTTON_VERLAAG);
}

int buttonPressed(int button) {
    return (PINC & (1 << button)) == 0;
}

void readButton(int *displayValue) {
    while (1) {
        if (buttonPressed(BUTTON_VERHOOG)) {
            (*displayValue)++;
            if (*displayValue > 3) {
                *displayValue = 3; // Zorg ervoor dat het displaywaarde niet hoger gaat dan 3
            }
            writeNumberToSegment(3, *displayValue); // Update het display
            _delay_ms(200);
        }
        
        if (buttonPressed(BUTTON_VERLAAG)) {
            (*displayValue)--;
            if (*displayValue < 1) {
                *displayValue = 1; // Zorg ervoor dat het displaywaarde niet lager gaat dan 1
            }
            writeNumberToSegment(3, *displayValue); // Update het display
            _delay_ms(200);
        }
        
        if (buttonPressed(BUTTON_PIN)) {
            printf("Button value confirmed: %d\n", *displayValue);
            break;
        }
    }
}



void spelerBeurt(int *startAantal, int maxAantal) {
    int aantalGekozen = 1; // Begin met het standaard aantal lucifers

    printf("Speler is aan de beurt!\n");

    // Blijf in de lus totdat de speler een geldig aantal lucifers kiest
    while (1) {
        // Toon het gekozen aantal lucifers op het display
        writeNumberToSegment(0, aantalGekozen);
        writeCharToSegment(1, 'p');
        writeNumberToSegment(2, *startAantal / 10);
        writeNumberToSegment(3, *startAantal % 10);

        // Knop 3 (meest rechtse) verhoogt het gekozen aantal lucifers (niet hoger dan maxAantal)
        if (buttonPressed(BUTTON_VERHOOG) && aantalGekozen < maxAantal) {
            aantalGekozen++;
            _delay_ms(200);
        }

        // Knop 1 (meest linkse) verlaagt het gekozen aantal lucifers (niet lager dan 1)
        if (buttonPressed(BUTTON_VERLAAG) && aantalGekozen > 1) {
            aantalGekozen--;
            _delay_ms(200);
        }

        // Knop 2 (middelste) bevestigt de keuze en gaat naar de beurt van de computer
        if (buttonPressed(BUTTON_PIN)) {
            // Pas het aantal lucifers aan door het gekozen aantal weg te nemen
            *startAantal -= aantalGekozen;
            // Toon het nieuwe aantal lucifers op het display
            writeNumberToSegment(1, *startAantal / 10);
            writeNumberToSegment(2, *startAantal % 10);
            _delay_ms(200);
            break; // Verlaat de lus zodra de keuze is bevestigd
        }
    }
}




void computerBeurt(int *startAantal, int maxAantal) {
    // Blijf in een lus totdat de computerbeurt is voltooid
    while (1) {
        // Toon 'C' en startAantal op het display
        writeCharToSegment(1, 'c');
        writeNumberToSegment(2, *startAantal / 10);
        writeNumberToSegment(3, *startAantal % 10);    

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

        // Controleer of de computerbeurt is voltooid
        if (*startAantal <= 0) {
            break;
        }
    }
}




int main() {
    initUSART();
    startPotentiometer();
    initDisplay(); 
    initButton();

    uint16_t value = 0;
    unsigned int seed = 0;

    printf("Draai aan de potentiometer om de seed te kiezen en druk op de knop om te starten.\n");

    while (!buttonPressed(BUTTON_PIN)) {
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
        if (speler == 1) {
            spelerBeurt(&startAantal, maxAantal);
            speler = 1; // Wissel naar computer na spelerbeurt
        } else {
            computerBeurt(&startAantal, maxAantal);
            speler = 0; // Wissel naar speler na computerbeurt
        }

        // Controleer of het spel moet eindigen
        if (startAantal <= 0) {
            printf("Spel afgelopen\n");
            break; // Beëindig de loop als het spel voorbij is
        }
    }

    return 0;
}
