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

#define LED1 0
#define LED2 1
#define LED3 2
#define LED4 3

#define BUZZER_PIN PB1
#define DELAY_TIME 200
#define NUM_LEDS 4

int birdPos;
int globalIndex;

volatile int gameOver = 0;
volatile int gameOverNext = 0;

// Toegevoegde globale array voor pijpen
int pipeArray[9999];

}


    // Check elke 100ms (10 interrupts)
    if (globalIndex % 10 == 0) {
        drawPipes(pipeArray, 1);
    }

    // Pauzeer spel indien nodig
    pause();
}

    printf("Draai aan de potentiometer om het niveau te kiezen.\n");


void displayLightShow(){
    while (!(buttonPushed(PC1) || buttonPushed(PC2) || buttonPushed(PC3))) {
        // Effect 1: Willekeurige lijnen
        for (int i = 0; i < 10; i++) {
            drawLine(rand() % 4, rand() % 7);
            _delay_ms(100);
        }
    }
}

int kiesLevel(){
    int value;
    while (!(buttonPushed(PC1) || buttonPushed(PC2) || buttonPushed(PC3))) {
        ADCSRA |= (1<<ADSC);
        loop_until_bit_is_clear(ADCSRA,ADSC);
        value = ADC / 100;
        writeNumber(value);
    }
  
    printf("Gekozen Level:%d\n", value);
    return value;
}

void flappyBird(int level){
    switch (level) {
        case 0:
            drawLine(0, 3); // 3 komt overeen met 0xBF in LINE_MAP voor het middelste segment
            break;
        case 1:
            drawLine(0, 6); // 6 komt overeen met 0xBF in LINE_MAP voor het middelste segment
            break;
        case 2:
            drawLine(0, 0); // 0 komt overeen met 0xFE in LINE_MAP voor het bovenste segment
            break;
        default:
            break;
    }
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

volatile int paused = 0;

void pause() {
    if (buttonPushed(PC2)) {
        paused = !paused;
        while (buttonPushed(PC2)); // Wacht tot de knop wordt losgelaten
        _delay_ms(100); // Debounce delay
        if (paused) {
            printf("Gepauzeerd\n");
            lightDownAllLeds();
            clearDisplay(); // Voeg deze regel toe om de display uit te zetten
        } else {
            printf("Hervat\n");
        }
    }
}

void clearDisplay() {
    for (int i = 0; i < 4; i++) {
        drawLine(i, 0); // Assuming 0 clears the display line by turning off all segments
    }
}




int main(void) {
    initUSART();
    enableAllLeds();
    lightDownAllLeds();
    enableAllButtons();
    startPotentiometer();
    initDisplay();

    sei(); // Globaal interrupts inschakelen


    printf("Druk op een willekeurige button om het spel te starten.\n");

    while (1) {
        if (!buttonPressed) {
            displayLightShow();
        }

        if (!buttonPressed && (buttonPushed(PC1) || buttonPushed(PC2) || buttonPushed(PC3))) {
            lightDownAllLeds();
            printf("Het spel is gestart\n");
            buttonPressed = 1;
            _delay_ms(100);
        } else if (buttonPressed == 1 && (buttonPushed(PC1) || buttonPushed(PC2) || buttonPushed(PC3))) {
            int level = kiesLevel();
            printf("Starten met niveau: %d\n", level);
            break;
        }
    }
    
    printf("Het spel begint nu!\n");
    printf("Gebruik button 1 om de flappy bird langs de opstakels te laten vliegen\n");

    while (1) {
        pause(); // Controleer de pauzetoestand

        if (!paused) {
            obstakels();
            flappyBird();

        } else {
            clearDisplay();
        }
    }
    


    return 0;
}