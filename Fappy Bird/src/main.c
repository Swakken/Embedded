// Flappy Bird Project 
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include <usart.h>

#include "pot.h"
#include "display.h"
#include "button.h"
#include "led.h"

#define BUTTON_PIN0 PC0
#define BUTTON_PIN1 PC1  
#define BUTTON_PIN2 PC2

#define BUZZER_PIN PB1
#define DELAY_TIME 200

int level = 1;
volatile int birdPositionIndex = 1; // Start altijd in het midden
const int birdPositions[] = {0, 3, 6};

#define UP_DELAY_MS 200
#define DOWN_DELAY_MS 500

void displayLightShow() {
    printf("Druk op een willekeurige button om het spel te starten.\n");
    
    while (!(buttonPushed(PC1) || buttonPushed(PC2) || buttonPushed(PC3))) {
        drawLine(rand() % 4, rand() % 7);
        _delay_ms(100);
    }
    _delay_ms(500);
}

int kiesLevel() {
    int potValue;

    printf("Draai aan de potentiometer om het niveau te kiezen.\n");

    while (!(buttonPushed(PC1) || buttonPushed(PC2) || buttonPushed(PC3))) {
        potValue = readPotentiometer();

        level = (potValue / 102) + 1;
        if (level > 10) {
            level = 10;
        }

        writeNumber(level);
        _delay_ms(500);
    }
    printf("Het spel begint nu op niveau %d!\n", level);
    return level;
}


void flappyBird(int level, int birdPosition) {
    clearDisplay();  // Maak het display schoon voordat de nieuwe positie getekend wordt
    drawLine(0, birdPosition);
}

// Timer1 overflow interrupt service routine
ISR(TIMER1_COMPA_vect) {
    birdPositionIndex++;
    if (birdPositionIndex > 2) { // Reset de index als deze de lengte van de array overschrijdt
        birdPositionIndex = 0;
    }
    flappyBird(level, birdPositions[birdPositionIndex]);  // Geef zowel level als birdPosition door
}

void checkButtonAndMoveBird() {
    static uint8_t buttonPressed = 0;
    if (buttonPushed(BUTTON_PIN1)) { // Check of de button wordt ingedrukt
        if (buttonPressed == 0) { // Als de knop net is ingedrukt
            birdPositionIndex = (birdPositionIndex > 0) ? birdPositionIndex - 1 : 0; // Beweeg de vogel omhoog, tenzij deze al bovenaan is
            flappyBird(level, birdPositions[birdPositionIndex]);
            printf("De vogel vliegt omhoog!\n"); // Laat een bericht zien in de terminal
            _delay_ms(UP_DELAY_MS); // Debounce delay
            buttonPressed = 1;
        }
    } else {
        if (buttonPressed == 1) { // Als de knop net is losgelaten
            buttonPressed = 0;
        } else {
            _delay_ms(DOWN_DELAY_MS); // Laat de vogel na enige tijd naar beneden vliegen
            birdPositionIndex = (birdPositionIndex < 2) ? birdPositionIndex + 1 : 2; // Beweeg de vogel naar beneden, tenzij deze al onderaan is
            flappyBird(level, birdPositions[birdPositionIndex]);
        }
    }
}

void obstakels() {
    while (1) {
        // Display 1: Bovenste segment
        drawLine(1, 0); // 0 komt overeen met 0xFE voor het bovenste segment
        _delay_ms(500);

        // Display 1: Middelste segment
        drawLine(1, 6); // 6 komt overeen met 0xBF voor het middelste segment
        _delay_ms(500);

        // Display 1: Onderste segment
        drawLine(1, 3); // Aannemend dat 3 het onderste segment is
        _delay_ms(500);

        // Display 2: Bovenste segment
        drawLine(2, 0); // 0 komt overeen met 0xFE voor het bovenste segment
        _delay_ms(500);

        // Display 2: Middelste segment
        drawLine(2, 6); // 6 komt overeen met 0xBF voor het middelste segment
        _delay_ms(500);

        // Display 2: Onderste segment
        drawLine(2, 3); // Aannemend dat 3 het onderste segment is
        _delay_ms(500);

        // Display 3: Bovenste segment
        drawLine(3, 0); // 0 komt overeen met 0xFE voor het bovenste segment
        _delay_ms(500);

        // Display 3: Middelste segment
        drawLine(3, 6); // 6 komt overeen met 0xBF voor het middelste segment
        _delay_ms(500);

        // Display 3: Onderste segment
        drawLine(3, 3); // Aannemend dat 3 het onderste segment is
        _delay_ms(500);
    }
}

void clearDisplay() {
    for (int i = 0; i < 4; i++) {
        drawLine(i, -1);
    }
}


int main(void) {
    initUSART();
    enableAllLeds();
    lightDownAllLeds();
    enableAllButtons();
    startPotentiometer();
    initDisplay();

    displayLightShow(); // Toon de lichtshow en wacht op knopdruk om te starten

    // Voeg hier de functieaanroep toe om het niveau te kiezen
    int chosenLevel = kiesLevel();  // Laat de speler het niveau kiezen na de lichtshow

    printf("Gebruik button 1 om de flappy bird langs de obstakels te laten vliegen\n");

    while (1) {
        checkButtonAndMoveBird();
    }

    return 0;
}