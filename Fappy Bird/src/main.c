// Flappy Bird Project 

/*
Ik heb dit project niet kunnen afronden door beperkte tijd en te weinig kennis van C. 
Ik heb meerdere dagen aan dit project gewerkt en geprobeerd om het toch af te krijgen, maar het is niet gelukt.
*/


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


int level = 1;
volatile int birdPositionIndex = 1; // Start altijd in het midden

// 0 komt overeen met 0xFE voor het bovenste segment
// 6 komt overeen met 0xBF voor het middelste segment
// 3 komt overeen met voor het laagste segment
const int birdPositions[] = {0, 3, 6};
const int obstakelPositions[] = {0, 3, 6};


void clearDisplay() {
    for (int i = 0; i < 4; i++) {
        drawLine(i, -1);
    }
}

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
    _delay_ms(500);
}

// Flappy Bird

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





// Functie om een willekeurig segment te kiezen uit de obstakelPositions array
int kiesSegment() {
    int index = rand() % (sizeof(obstakelPositions) / sizeof(obstakelPositions[0]));
    return obstakelPositions[index];
}

// Obstakels
void obstakels() {
    int segment = kiesSegment(); // Kies een willekeurig segment voor het obstakel

    int display = 3; // Start op display 3

    while (1) {
        // Plaats het obstakel op hetzelfde segment van het huidige display
        drawLine(display, segment);

        _delay_ms(1000); // Vertraag elke stap met 1000 milliseconden

        // Verwijder het obstakel van het huidige display
        clearDisplay(display);

        // Ga naar het volgende display
        display--;

        // Als we display 0 bereiken, ga dan terug naar display 3 en plaats een nieuw obstakel
        if (display == 0) {
            display = 3; // Terug naar display 3

            // Kies een nieuw willekeurig segment voor het nieuwe obstakel
            segment = kiesSegment();
        }
    }
}



int main(void) {
    initUSART();
    enableAllLeds();
    lightDownAllLeds();
    enableAllButtons();
    startPotentiometer();
    initDisplay();

    // displayLightShow(); // Toon de lichtshow en wacht op knopdruk om te starten

    // Voeg hier de functieaanroep toe om het niveau te kiezen
    int chosenLevel = kiesLevel();  // Laat de speler het niveau kiezen na de lichtshow

    printf("Gebruik button 1 om de flappy bird langs de obstakels te laten vliegen\n");

    while (1) {
        obstakels();
    }

    return 0;
}