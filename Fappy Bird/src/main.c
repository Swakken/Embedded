// Flappy Bird Project 
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#include <usart.h>

#include "pot.h"
#include "display.h"
#include "button.h"
#include "led.h"

#define BUTTON_PIN0 PC0
#define BUTTON_PIN1 PC1
#define BUTTON_PIN2 PC2

#define LED1 0
#define LED2 1
#define LED3 2
#define LED4 3


#define DELAY_TIME 200
#define NUM_LEDS 4


int gameStarted = 0;

// Functie om te controleren of een van de knoppen is ingedrukt
int isAnyButtonPressed() {
    return (buttonPushed(BUTTON_PIN0) || buttonPushed(BUTTON_PIN1) || buttonPushed(BUTTON_PIN2));
}

// Wacht totdat een van de knoppen wordt ingedrukt
void waitForButtonPress() {
    while (!isAnyButtonPressed()) {
        // Doe niets en wacht
    }
}


void lightShow() {
    printf("Druk op een willekeurige knop om het spel te starten\n");

    while (!gameStarted) {
        // Blijf de lichtshow continu herhalen totdat het spel is gestart door de gebruiker
        while (1) {
            // Licht elke LED op volgorde op
            for (int i = 0; 1; i = (i + 1) % NUM_LEDS) {
                lightUpLed(i);
                _delay_ms(500);    // Wacht 500ms
                lightDownLed(i);

                // Controleer of een van de knoppen is ingedrukt om het spel te starten
                if (isAnyButtonPressed()) {
                    gameStarted = 1;
                    printf("Het spel is gestart!\n");
                    break; // Verlaat de binnenste lus als het spel is gestart
                }
            }
            
            // Verlaat de binnenste lus als het spel is gestart
            if (gameStarted) {
                break;
            }
        }
    }
}


// Functie om de gebruiker de snelheid van de obstakels te laten kiezen
void spelerNiveau() {
    printf("Draai aan de potentiometer\n");

    int level = 0; // Variabele om het gekozen niveau op te slaan

    while (!isAnyButtonPressed()) {
        // Lees de waarde van de potentiometer en converteer deze naar een niveau (1-10)
        level = (readPotentiometer() * 10) / 1024; // Converteer de waarde naar een niveau tussen 0 en 10
        // Toon het niveau op het LED-display
        writeNumber(level);
    }

    // Wacht op knopdruk om het spel te starten
    waitForButtonPress();

    gameStarted = 1; // Markeer het spel als gestart
    // Ga verder met het spel...
}








int main() {
    initUSART();
    initDisplay(); 

    enableAllLeds();
    lightDownAllLeds();
    enableAllButtons();

    spelerNiveau();

    lightShow();

    // Voer de rest van het spel uit
    // ...

    return 0;
}