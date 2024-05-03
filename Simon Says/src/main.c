#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <button.h>
#include "usart.h"
#include <led.h>


#define BUTTON_PIN PCINT1
#define BUTTON_PIN2 PCINT2
#define BUTTON_PIN3 PCINT3

#define LED1 0
#define LED2 1
#define LED3 2
#define LED4 3
#define DELAY 500

#define BUTTON1 0
#define BUTTON2 1
#define BUTTON3 2

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

            cli();
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

    startGame();

   

/*
    initRandomGenerator(); 
    for (int i = 0; i < 10; i++) {
        int randomNumber = getRandomNumber();
        printf("Random getal %d: %d\n", i + 1, randomNumber);
    }
*/





    // Maak een functie die gebruik maakt van de waarde van seedTellerLed4 als voor de randomgenerator.
    // Werk dit uit en test door een reeks van 10 random getallen naar de Serial Monitor te sturen. 
    // Check of je reeks inderdaad steeds andere waardes krijgt...


    while (1) {

    }

    return 0;
}
