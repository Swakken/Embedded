#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <button.h>
#include "usart.h"
#include <led.h>
#include <stdlib.h>

#define LED1 0
#define LED2 1
#define LED3 2
#define LED4 3

#define DELAY 500

#define BUTTON_PIN PCINT1
#define BUTTON_PIN2 PCINT2
#define BUTTON_PIN3 PCINT3

#define LED_COUNT 3
#define MAX_LEVEL 10
#define DELAY_MS 500
#define BUTTON_PRESS_DELAY 200

volatile int not_started = 1;

ISR(PCINT1_vect) {
    if (buttonPushed(BUTTON_PIN)) {
        not_started = 0;
    }
}

int seedTellerLed4 = 0;

void initRandomGenerator() {
    srand(seedTellerLed4);
}

int getRandomNumber() {
    return rand() % 3; 
}

// Functie om een patroon te genereren
void generatePuzzle(uint8_t puzzle[], int length) {
    for (int i = 0; i < length; i++) {
        puzzle[i] = getRandomNumber();
    }
}

// Functie om het patroon te printen
void printPuzzle(uint8_t puzzle[], int length) {
    printf("[");
    for (int i = 0; i < length; i++) {
        printf("%d", puzzle[i]);
        if (i < length - 1) {
            printf(" ");
        }
    }
    printf("]\n");
}

void flashLed(int num_leds) {
    for (int i = 0; i < num_leds; i++) {
        if (i == 0) {
            continue; // Overslaan van LED1
        } else if (i == 1) {
            lightUpLed(LED2);
        } else if (i == 2) {
            lightUpLed(LED3);
        } else if (i == 3) {
            lightUpLed(LED4);
        }
        _delay_ms(DELAY_MS);
    }
    lightDownAllLeds();
}

int readButton() {
    if (buttonPushed(BUTTON_PIN)) {
        return BUTTON_PIN; 
    } else if (buttonPushed(BUTTON_PIN2)) {
        return BUTTON_PIN2; 
    } else if (buttonPushed(BUTTON_PIN3)) {
        return BUTTON_PIN3;
    }
    return -1;
}


// Functie die het Simon Says spel leidt
void playSimonSays() {
    int level = 1;
    int success = 1;
    int userButton;

    while (level <= MAX_LEVEL && success) {
        int sequence[MAX_LEVEL];
        printf("Level %d: volg de LEDs\n", level);

        for (int i = 0; i < level; i++) {
            sequence[i] = rand() % 3;
            flashLed(i + 1);
        }

        // Vraag de gebruiker om de sequentie na te bootsen
        for (int i = 0; i < level; i++) {
            userButton = readButton(sequence[i]);
            while (userButton == -1) { // Polling tot een knop wordt ingedrukt
                userButton = readButton(sequence[i]);
                _delay_ms(BUTTON_PRESS_DELAY); // Kleine vertraging om debouncing en overvragen te vermijden
            }

            if (userButton != sequence[i]) {
                success = 0; // De gebruiker heeft een fout gemaakt
                printf("Fout gemaakt! Spel is over bij level %d.\n", level);
                break;
            }
        }


        if (success && level == MAX_LEVEL) {
            printf("Gefeliciteerd! Je hebt het spel voltooid en bent de Simon Says Master!\n");
        }

        level++; // Ga naar het volgende level
    }
}


// Functie om het spel te starten
void startGame() {
        printf("Druk op knop1 om het spel te starten\n");
        
        while (not_started) {
            lightUpLed(LED4);
            _delay_ms(300);
            lightDownLed(LED4);
            _delay_ms(300);
            seedTellerLed4++;
        }

            lightDownLed(LED4);
            initRandomGenerator();

            printf("Het spel is gestart!\n");
            printf("De waarde van seedTellerLed4 is: %d\n", seedTellerLed4);

            uint8_t puzzle[10];
            generatePuzzle(puzzle, 10);
            printf("Het gegenereerde patroon is: ");
            printPuzzle(puzzle, 10);
            _delay_ms(250);

            cli();
}


int main(void) {
    initUSART();
    enableAllLeds();
    lightDownAllLeds();
    lightUpLed(LED4);

    enableButton(BUTTON_PIN);
    enableButton(BUTTON_PIN2); 
    enableButton(BUTTON_PIN3);

    // Knoppen initialiseren
    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(BUTTON_PIN) | _BV(BUTTON_PIN2) | _BV(BUTTON_PIN3); // Activeer interrupts voor alle knoppen als ze nog steeds nodig zijn.

    sei();

    startGame();

    playSimonSays(); // Start het Simon Says spel.




    // Maak een functie die gebruik maakt van de waarde van seedTellerLed4 als voor de randomgenerator.
    // Werk dit uit en test door een reeks van 10 random getallen naar de Serial Monitor te sturen. 
    // Check of je reeks inderdaad steeds andere waardes krijgt...


    while (1) {

    }

    return 0;
}
