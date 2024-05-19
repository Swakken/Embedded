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


void displayLightShow() {
    for (uint8_t i = 0; i < 4; i++) {
        lightUpLed(i);
        _delay_ms(200);
        lightDownLed(i);
        _delay_ms(200);

        if (buttonPushed(PC1) || buttonPushed(PC2) || buttonPushed(PC3)) {
            return 1;
        }
    }
    lightUpAllLeds();
    _delay_ms(500);
    lightDownAllLeds();

    return 0;
}

int kiesLevel() {
    printf("Draai aan de potentiometer om de snelheid van de obstalkens te bepalen.\n");

    int level = 0;
    while (1) {
        int potValue = readPotentiometer();
        level = (potValue / 102) + 1;
        if (level > 10) {
            level = 10;
        }

        writeNumber(level);

        if (buttonPushed(PC0) || buttonPushed(PC1) || buttonPushed(PC2)) {
            printf("Niveau gekozen: %d\n", level);
            break;
        }

        _delay_ms(100);
    }
    return level;
}



int main(void) {
    initUSART();
    enableAllLeds();
    lightDownAllLeds();
    enableAllButtons();
    startPotentiometer();
    initDisplay();

    printf("Druk op een willekeurige button om het spel te starten.\n");

    while (1) {
        displayLightShow();

        if (buttonPushed(PC1) || buttonPushed(PC2) || buttonPushed(PC3)) {
            
            printf("Het spel is gestart\n");
        }

        if (buttonPushed(PC1) || buttonPushed(PC2) || buttonPushed(PC3)) {
            int level = kiesLevel();
            printf("Starten met niveau: %d\n", level);
            break;
        }
       
    }
    return 0;
}