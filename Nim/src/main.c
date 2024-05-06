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

    while (!buttonPressed()) {
        value = readPotentiometer();
        seed = value % 10000;
        writeNumberAndWait(seed, 200);
    }

    srand(seed);
    printf("Game started with seed: %u\n", seed);

    int currentPlayer = rand() % 2;  // 0 voor speler, 1 voor computer
    int displayStarter = currentPlayer + 1;  // '1' voor speler, '2' voor computer

    // Knipper de startspeler indicator (numeriek, niet als letter)
    for (int i = 0; i < 10; ++i) {
        writeNumber(0); // Wis display
        _delay_ms(500);
        writeNumberToSegment(0, displayStarter); // Toon '1' of '2' op de eerste positie
        _delay_ms(500);
    }

    writeNumber(startAantal); // Toon startaantal lucifers

    while (startAantal > 0) {
        if (currentPlayer == 0) { // Speler's beurt
            printf("Speler's beurt. Kies het aantal lucifers om te nemen (1 tot %d):\n", maxAantal);
            int gekozenAantal;
            scanf("%d", &gekozenAantal);
            if (gekozenAantal >= 1 && gekozenAantal <= maxAantal && gekozenAantal <= startAantal) {
                startAantal -= gekozenAantal;
            } else {
                printf("Ongeldige invoer. Probeer opnieuw.\n");
                continue; // Overslaan naar volgende iteratie van de lus
            }
        } else { // Computer's beurt
            int genomenAantal = rand() % maxAantal + 1; // Kies een willekeurig aantal lucifers
            printf("De computer neemt %d lucifers.\n", genomenAantal);
            startAantal -= genomenAantal;
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
