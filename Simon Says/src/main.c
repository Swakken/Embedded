#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "usart.h"
#include <led.h>
#include <button.h>

#define LED1 0
#define LED2 1
#define LED3 2
#define LED4 3

#define BUTTON_PIN PCINT1
#define BUTTON_PIN2 PCINT2
#define BUTTON_PIN3 PCINT3

#define LED_COUNT 3
#define MAX_LEVEL 10
#define DELAY 500
#define BUTTON_PRESS_DELAY 200

volatile int not_started = 1;
volatile int game_over = 0;

ISR(PCINT1_vect) {
    if (buttonPushed(BUTTON_PIN)) {
        not_started = 0;
    }
}

ISR(TIMER1_COMPA_vect) {
    game_over = 1;
}

int seedTellerLed4 = 0;

void initRandomGenerator() {
    srand(seedTellerLed4);
}

int getRandomNumber() {
    return rand() % 3;
}

void generatePuzzle(uint8_t puzzle[], int length) {
    for (int i = 0; i < length; i++) {
        puzzle[i] = getRandomNumber();
    }
}

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

void playPuzzle(uint8_t puzzle[], int size) {
    for (uint8_t i = 0; i < size; i++) {
        lightUpLed(puzzle[i]);
        _delay_ms(DELAY);
        lightDownLed(puzzle[i]);
        _delay_ms(DELAY);
    }
}

uint8_t readInput(uint8_t puzzle[], int size) {
    int position = 0;
    game_over = 0; // Reset game over status

    while (position < size && !game_over) {
        int button = readButton();
        if (button != -1 && button != puzzle[position]) {
            printf("Pushed button %d, Not correct!\n", button);
            return 0;
        } else if (button == puzzle[position]) {
            printf("Pushed button %d, correct!\n", button);
            position++;
        }
    }

    if (position == size) {
        return 1; // Success
    } else {
        printf("Timeout! No input received.\n");
        return 0; // Timeout
    }
}

void enableInterrupts() {
    sei();
}

int readButton() {
    if (buttonPushed(BUTTON_PIN)) {
        _delay_ms(BUTTON_PRESS_DELAY);
        if (buttonPushed(BUTTON_PIN)) {
            return 0; // Retourneert index voor LED1/puzzel[0]
        }
    } else if (buttonPushed(BUTTON_PIN2)) {
        _delay_ms(BUTTON_PRESS_DELAY);
        if (buttonPushed(BUTTON_PIN2)) {
            return 1; // Retourneert index voor LED2/puzzel[1]
        }
    } else if (buttonPushed(BUTTON_PIN3)) {
        _delay_ms(BUTTON_PRESS_DELAY);
        if (buttonPushed(BUTTON_PIN3)) {
            return 2; // Retourneert index voor LED3/puzzel[2]
        }
    }
    return -1; // Geen geldige input
}

void flashAllLeds() {
    for (int i = 0; i < LED_COUNT; i++) {
        lightUpLed(i);
    }
    _delay_ms(200); // Vertraging voor aan
    for (int i = 0; i < LED_COUNT; i++) {
        lightDownLed(i);
    }
}


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

    uint8_t puzzle[MAX_LEVEL];

    for (int level = 1; level <= MAX_LEVEL; level++) {
        generatePuzzle(puzzle, level);

        printf("Level %d: Het gegenereerde patroon is: ", level);
        printPuzzle(puzzle, level);
        _delay_ms(1000);

        playPuzzle(puzzle, level);

        uint8_t success = readInput(puzzle, level);
        if (!success) {
            printf("Helaas, je hebt een fout gemaakt en het patroon niet correct herhaald.\n");
            _delay_ms(500);
            flashAllLeds();
            return;
        }
        printf("Correct! Ga door naar het volgende level.\n");
        _delay_ms(2000);
    }

    printf("Gefeliciteerd! Je hebt alle levels doorlopen en het spel gewonnen!\n");
}

int main(void) {
    initUSART();
    enableAllLeds();
    lightDownAllLeds();
    lightUpLed(LED4);

    enableInterrupts();

    enableButton(BUTTON_PIN);
    enableButton(BUTTON_PIN2);
    enableButton(BUTTON_PIN3);

    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(BUTTON_PIN) | _BV(BUTTON_PIN2) | _BV(BUTTON_PIN3);

    sei();

    startGame();

    return 0;
}
