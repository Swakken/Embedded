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
        puzzle[i] = rand() %3;
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

void playPuzzle(uint8_t puzzle[], int size) {

    for (uint8_t i = 0; i < size; i++)
    {
         lightUpLed(puzzle[i]);
         _delay_ms(DELAY);
         lightDownLed(puzzle[i]);
         _delay_ms(DELAY);
    }
}

// Functie readInput
uint8_t readInput(uint8_t puzzle[], int size) {
    
    int position = 0;

    while (1)
    {
        for (uint8_t i = 0; i < 3; i++)
        {
            if (buttonPushed(i) && puzzle[position] != i)
            {
                _delay_ms(1000);

                if (buttonPushed(i) && puzzle[position] != i)
                {
                    printf("Pushed button %d, Not correct!\n", i);
                    return 0;
                }
            }
        }
        if (buttonPushed(puzzle[position]))
        {
            _delay_ms(1000);

            if (buttonPushed(puzzle[position]))
            {
                    printf("Pushed button %d, correct!\n", puzzle[position]);
                    position++;
                    if (position == size) {
                        return 1;
                    }
            }
        }
    }
}

// Functie om interrupts in te schakelen
void enableInterrupts() {
    sei();
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

    // Laat de sequentie zien
    for (int i = 0; i < 10; i++) {
        playPuzzle(puzzle[i], 1);
        _delay_ms(1000); // Een korte pauze tussen elke knipperende LED
    }

    // Lees de invoer van de speler en controleer of deze correct is
    uint8_t success = readInput(puzzle, 10);
    if (success) {
        printf("Gefeliciteerd! Je hebt het patroon correct herhaald!\n");
    } else {
        printf("Helaas, je hebt een fout gemaakt en het patroon niet correct herhaald.\n");
    }
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

    // Knoppen initialiseren
    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(BUTTON_PIN) | _BV(BUTTON_PIN2) | _BV(BUTTON_PIN3);

    sei();

    startGame();

    return 0;
}
