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


int level = 1;
volatile int birdPositionIndex = 1; // Start altijd in het midden
volatile int gamePaused = 0;
volatile int lives = 3;
volatile int timer = 0;

// 0 komt overeen met 0xFE voor het bovenste segment
// 6 komt overeen met 0xBF voor het middelste segment
// 3 komt overeen met voor het laagste segment
const int birdPositions[] = {0, 6, 3};
const int obstakelPositions[] = {0, 6, 3};


// Verwijder de lijnen die nog zichtbaar zijn op de display
void clearDisplay() {
    for (int i = 1; i < 4; i++) {
        drawLine(i, -1);
    }
}

// Toont random lijntjes op de display totdat de gebruiker op de button drukt om het spel te starten 
void displayLightShow() {
    printf("Druk op een willekeurige button om het spel te starten.\n");
    
    while (!(buttonPushed(BUTTON_PIN0) || buttonPushed(BUTTON_PIN1) || buttonPushed(BUTTON_PIN2))) {
        drawLine(rand() % 4, rand() % 7);
        _delay_ms(100);
    }
    _delay_ms(500);
}

// Laat de gebruiker een moeilijkheidsgraad kiezen voor dat het spel start
int kiesLevel() {
    int potValue;

    printf("Draai aan de potentiometer om het niveau te kiezen.\n");

    while (!(buttonPushed(BUTTON_PIN0) || buttonPushed(BUTTON_PIN1) || buttonPushed(BUTTON_PIN2))) {
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

// Functie om een willekeurig segment te kiezen uit de obstakelPositions array
int kiesObstakelSegment() {
    int index = rand() % (sizeof(obstakelPositions) / sizeof(obstakelPositions[0]));
    return obstakelPositions[index];
}


// Eindig het spel en reset
void gameOver() {
    printf("Game Over!\n");
    lives = 3;
    birdPositionIndex = 1;
    displayLightShow();
    level = kiesLevel();
}


void updateGame() {
    int segment = kiesObstakelSegment();
    int display = 3;

    while (lives > 0) {
        if (!gamePaused) {
            int birdPosition = birdPositions[birdPositionIndex];

            // Zorg ervoor dat de vogelpositie blijft staan
            drawLine(0, birdPosition);

            // Voeg hier een delay toe zodat het obstakel eerder wordt getoond
            for (int i = 0; i < (10 - level) * 50; i++) {
                _delay_ms(1);
            }

            drawLine(display, segment);

            for (int i = 0; i < (10 - level) * 100; i++) {
                _delay_ms(1);
            }

            // Clear de obstakel lijn maar laat de vogelpositie staan
            drawLine(display, -1);

            if (display == 1) {
                if (birdPositions[birdPositionIndex] == segment) {
                    lives--;
                    printf("Oeps! Je hebt een obstakel geraakt. Levens over: %d\n", lives);
                    if (lives == 0) {
                        gameOver();
                        return;
                    } else {
                        printf("Blijf doorgaan! Levens over: %d\n", lives);
                    }
                }
            }

            display--;

            if (display == 0) {
                display = 3;
                segment = kiesObstakelSegment();
            }
        }
    }
}


ISR(TIMER1_COMPA_vect) {
    if (!gamePaused) {
        timer++;
        if (timer >= (10 - level)) {
            if (birdPositionIndex < 2) { // Verplaats de vogel naar beneden als hij al niet op het laagste segment staat
                birdPositionIndex++;
            }
            timer = 0;
        }
    }
}

ISR(PCINT1_vect) {
    if (buttonPushed(BUTTON_PIN0)) {
        // Vogel omhoog, als hij nog niet boven is
        if (birdPositionIndex > 0) {
            birdPositionIndex--;
        }
    } else if (buttonPushed(BUTTON_PIN1)) {
        gamePaused = !gamePaused;
    }
}




void initTimer1() {
    TCCR1B |= (1 << WGM12);
    TIMSK1 |= (1 << OCIE1A);
    OCR1A = 15624;
    TCCR1B |= (1 << CS12) | (1 << CS10);
}

int main(void) {
    initUSART();
    enableAllLeds();
    lightDownAllLeds();
    enableAllButtons();
    startPotentiometer();
    initDisplay();

    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << BUTTON_PIN0) | (1 << BUTTON_PIN1);
    sei();

    initTimer1();

    while (1) {
        displayLightShow();
        level = kiesLevel();

        printf("Gebruik button 1 om de flappy bird omhoog te laten vliegen, button 2 om te pauzeren.\n");
        
        updateGame();
    }

    return 0;
}