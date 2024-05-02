#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "led.h"
#include "button.h"
#include "usart.h"

#define BUTTON_PIN PCINT1
#define LED1 0
#define LED2 1
#define LED3 2
#define LED4 3
#define DELAY 500

#define BUTTON1 0
#define BUTTON2 1
#define BUTTON3 2

volatile int button_pushed = 0;

ISR(PCINT1_vect) {
    if (buttonPushed(BUTTON_PIN)) {
        button_pushed = 1;
    }
}

int tellerLed4 = 0;

void initRandomGenerator() {
    srand(tellerLed4);
}

int getRandomNumber() {
    return rand() % 3; 
}



// Functie om een patroon te genereren
void generatePuzzle(uint8_t puzzle[], int length) {
    for (int i = 0; i < length; i++) {
        puzzle[i] = getRandomNumber(); // Vul de puzzel met random getallen tussen 0 en 2
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

// Functie om gebruikersinput te lezen en te verifiëren
int readInput(uint8_t puzzle[], int length) {
    int i = 0;
    while (i < length) {
        if (button_pushed) {
            if (buttonPushed(BUTTON1)) {
                printf("Je drukte op knop 1, ");
                if (puzzle[i] != 0) {
                    printf("fout!\n");
                    return 0; // Gebruiker maakte een fout
                }
                printf("correct!\n");
                i++; // Ga naar het volgende item in de puzzel
                _delay_ms(300); // Debounce delay
            } else if (buttonPushed(BUTTON2)) {
                printf("Je drukte op knop 2, ");
                if (puzzle[i] != 1) {
                    printf("fout!\n");
                    return 0; // Gebruiker maakte een fout
                }
                printf("correct!\n");
                i++; // Ga naar het volgende item in de puzzel
                _delay_ms(300); // Debounce delay
            } else if (buttonPushed(BUTTON3)) {
                printf("Je drukte op knop 3, ");
                if (puzzle[i] != 2) {
                    printf("fout!\n");
                    return 0; // Gebruiker maakte een fout
                }
                printf("correct!\n");
                i++; // Ga naar het volgende item in de puzzel
                _delay_ms(300); // Debounce delay
            }
            button_pushed = 0; // Reset button_pushed
        }
    }
    return 1; // Gebruiker heeft de hele reeks correct ingevoerd
}

// Functie om het spel te starten
void startGame() {
    printf("Het spel is gestart!\n");
    printf("De waarde van tellerLed4 is: %d\n", tellerLed4);
    
    uint8_t puzzle[10];
    generatePuzzle(puzzle, 10);
    printf("Het gegenereerde patroon is: ");
    printPuzzle(puzzle, 10);

}


int main(void) {
    initUSART();
    enableAllLeds();
    lightDownAllLeds();
    lightUpLed(LED4);

    enableButton(BUTTON_PIN);

    // Knoppen initialiseren
    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(BUTTON_PIN);

    sei();

    printf("Druk op knop1 om het spel te starten\n");

    while (1) {
        if (!button_pushed) {
            lightUpLed(LED4);
            _delay_ms(300);
            lightDownLed(LED4);
            _delay_ms(300);
            tellerLed4++;
        } else {
            lightDownLed(LED4);
            initRandomGenerator();
            startGame();
            button_pushed = 0;
            break;
        }
    }

    

/*
    initRandomGenerator(); 
    for (int i = 0; i < 10; i++) {
        int randomNumber = getRandomNumber();
        printf("Random getal %d: %d\n", i + 1, randomNumber);
    }
*/





    // Maak een functie die gebruik maakt van de waarde van tellerLed4 als voor de randomgenerator.
    // Werk dit uit en test door een reeks van 10 random getallen naar de Serial Monitor te sturen. 
    // Check of je reeks inderdaad steeds andere waardes krijgt...


    while (1) {

    }

    return 0;
}
