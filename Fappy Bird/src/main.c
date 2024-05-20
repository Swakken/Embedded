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

#define BUZZER_PIN PB1
#define DELAY_TIME 200
#define NUM_LEDS 4


int displayLightShow() {
    for (uint8_t i = 0; i < 4; i++) {
        lightUpLed(i);
        _delay_ms(200);
        lightDownLed(i);
        _delay_ms(200);

        if (buttonPushed(PC1) || buttonPushed(PC2) || buttonPushed(PC3)) {
            lightDownAllLeds();
            return 1;
        }
    }
    lightUpAllLeds();
    _delay_ms(500);
    lightDownAllLeds();

    return 0;
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

    

void obstakels() {
    while (1) {
        // Display 1: Bovenste segment
        drawLine(1, 0); // 0 komt overeen met 0xFE voor het bovenste segment
        _delay_ms(500);
        writeNumberToSegment(1, 0xFF); // Zet alle segmenten uit op display 1
        _delay_ms(500);

        // Display 1: Middelste segment
        drawLine(1, 6); // 6 komt overeen met 0xBF voor het middelste segment
        _delay_ms(500);
        writeNumberToSegment(1, 0xFF); // Zet alle segmenten uit op display 1
        _delay_ms(500);

        // Display 1: Onderste segment
        drawLine(1, 3); // Aannemend dat 3 het onderste segment is
        _delay_ms(500);
        writeNumberToSegment(1, 0xFF); // Zet alle segmenten uit op display 1
        _delay_ms(500);

        // Display 2: Bovenste segment
        drawLine(2, 0); // 0 komt overeen met 0xFE voor het bovenste segment
        _delay_ms(500);
        writeNumberToSegment(2, 0xFF); // Zet alle segmenten uit op display 2
        _delay_ms(500);

        // Display 2: Middelste segment
        drawLine(2, 6); // 6 komt overeen met 0xBF voor het middelste segment
        _delay_ms(500);
        writeNumberToSegment(2, 0xFF); // Zet alle segmenten uit op display 2
        _delay_ms(500);

        // Display 2: Onderste segment
        drawLine(2, 3); // Aannemend dat 3 het onderste segment is
        _delay_ms(500);
        writeNumberToSegment(2, 0xFF); // Zet alle segmenten uit op display 2
        _delay_ms(500);

        // Display 3: Bovenste segment
        drawLine(3, 0); // 0 komt overeen met 0xFE voor het bovenste segment
        _delay_ms(500);
        writeNumberToSegment(3, 0xFF); // Zet alle segmenten uit op display 3
        _delay_ms(500);

        // Display 3: Middelste segment
        drawLine(3, 6); // 6 komt overeen met 0xBF voor het middelste segment
        _delay_ms(500);
        writeNumberToSegment(3, 0xFF); // Zet alle segmenten uit op display 3
        _delay_ms(500);

        // Display 3: Onderste segment
        drawLine(3, 3); // Aannemend dat 3 het onderste segment is
        _delay_ms(500);
        writeNumberToSegment(3, 0xFF); // Zet alle segmenten uit op display 3
        _delay_ms(500);
    }
}




int main(void) {
    initUSART();
    enableAllLeds();
    lightDownAllLeds();
    enableAllButtons();
    startPotentiometer();
    initDisplay();

    int buttonPressed = 0;


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

    while (1)
    {
        obstakels();
    }
    


    return 0;
}