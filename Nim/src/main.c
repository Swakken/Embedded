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






/*
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "usart.h"
#include "pot.h"
#include "display.h"

#define BUTTON_PIN PCINT1

void initButton() {
    DDRD &= ~(1 << BUTTON_PIN);
    PORTD |= (1 << BUTTON_PIN);
}

int buttonPressed() {
    return (PIND & (1 << BUTTON_PIN)) == 0;
}

void blinkLetter(char letter, int delayMs) {
    for (int i = 0; i < 10; ++i) {
        clearDisplay();
        _delay_ms(delayMs);
        writeCharAtPos(letter, 1);
        _delay_ms(delayMs);
    }
}

void persoon(int* startAantal, int maxAantal) {
    printf("Speler's beurt. Kies het aantal lucifers om te nemen (1 tot %d):\n", maxAantal);
    int gekozenAantal;
    scanf("%d", &gekozenAantal);
    if (gekozenAantal >= 1 && gekozenAantal <= maxAantal && gekozenAantal <= *startAantal) {
        *startAantal -= gekozenAantal;
    } else {
        printf("Ongeldige invoer. Probeer opnieuw.\n");
        persoon(startAantal, maxAantal); // Roep de functie opnieuw aan voor een nieuwe poging
    }
}

void computer(int* startAantal, int maxAantal) {
    int genomenAantal = rand() % maxAantal + 1; // Kies een willekeurig aantal lucifers
    printf("De computer neemt %d lucifers.\n", genomenAantal);
    *startAantal -= genomenAantal;
}

int main() {
    initUSART();
    startPotentiometer();
    initDisplay();
    initButton();

    uint16_t value = 0;
    unsigned int seed = 0;
    int startAantal = 21;
    int maxAantal = 3;

    printf("Draai aan de potentiometer om de seed te kiezen en druk op de knop om te starten.\n");

    // Wacht tot de gebruiker op de knop drukt
    while (!buttonPressed()) {
        value = readPotentiometer();
        writeNumber(value % 10000); // Toon de waarde van de potentiometer op het display
        _delay_ms(200);
    }

    // Genereer de game seed nadat de gebruiker op de knop heeft gedrukt
    srand(value % 10000);
    seed = value % 10000;
    printf("Game started with seed: %u\n", seed);
    writeNumber(seed); // Toon het seed op het display

    int currentPlayer = rand() % 2;  // 0 voor speler, 1 voor computer
    int displayStarter = currentPlayer + 1;  // '1' voor speler, '2' voor computer

    // Knipper de startspeler indicator (numeriek, niet als letter)
    for (int i = 0; i < 10; ++i) {
        writeNumber(0); // Wis display
        _delay_ms(500);
        writeNumberToSegment(0, displayStarter); // Toon '1' of '2' op de eerste positie
        _delay_ms(500);
    }

    if (displayStarter == 1) {
        writeNumber(maxAantal); // Toon maxAantal als de speler begint
    }

    writeNumber(startAantal); // Toon startaantal lucifers

    while (startAantal > 0) {
        if (currentPlayer == 0) { // Speler's beurt
            persoon(&startAantal, maxAantal); // Roep de persoon functie aan
        } else { // Computer's beurt
            computer(&startAantal, maxAantal); // Roep de computer functie aan
        }

        writeNumber(startAantal); // Update het aantal lucifers op het display
        _delay_ms(1000); // Kort wachten voor duidelijke weergave
        currentPlayer = (currentPlayer + 1) % 2; // Wissel de huidige speler
    }

    if (currentPlayer == 1) {
        printf("De speler wint!\n");
    } else {
        printf("De computer wint!\n");
    }

    return 0;
}


*/