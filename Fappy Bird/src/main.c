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
#define NUM_LEDS 4

int level = 1;

void displayLightShow(){
  printf("Druk op een willekeurige button om het spel te starten.\n");
  
  while (!(buttonPushed(PC1) || buttonPushed(PC2) || buttonPushed(PC3))) {
    drawLine(rand() % 4, rand() % 7);
    _delay_ms(100);
  }
  _delay_ms(500);
}



// Ik moet echt 2 rondjes draaien aan de potentiometer om 1 getal hoger of lager te krijgen? 

int kiesLevel() {
    int potValue;

    printf("Draai aan de potentiometer om het niveau te kiezen.\n");

    while (1) {
        potValue = readPotentiometer();

        level = (potValue / 102) + 1;
        if (level > 10) {
            level = 10;
        }

        writeNumber(level);
        _delay_ms(100);

        if (buttonPushed(PC0) || buttonPushed(PC1) || buttonPushed(PC2)) {
            printf("Niveau gekozen: %d\n", level);
            break;
        }
    }
    return level;
}

void flappyBird(int level){
  switch (level) {
    case 0:
      drawLine(0, 3); // 3 komt overeen met 0xBF in LINE_MAP voor het middelste segment
      break;
    case 1:
      drawLine(0,6); // 6 komt overeen met 0xBF in LINE_MAP voor het middelste segment
      break;
    case 2:
      drawLine(0, 0); // 0 komt overeen met 0xFE in LINE_MAP voor het bovenste segment
      break;
    default:
      break;
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
        drawLine(i, -1); // Assuming -1 clears the display line
    }
}





int main(void) {
    initUSART();
    enableAllLeds();
    lightDownAllLeds();
    enableAllButtons();
    startPotentiometer();
    initDisplay();

    displayLightShow();

    
    printf("Het spel begint nu!\n");
    printf("Gebruik button 1 om de flappy bird langs de opstakels te laten vliegen\n");

    while (1) {
        pause(); // Controleer de pauzetoestand

        if (!paused) {
            obstakels();
            // flappyBird();

        } else {
            clearDisplay();
        }
    }
    


    return 0;
}