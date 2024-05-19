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


int main(void) {
    initUSART();
    enableAllButtons();
    initDisplay();
    enableAllLeds();
    lightDownAllLeds();
    enableAllButtons();

    while (1) {
        displayLightShow();

        if (buttonPushed(PC1) || buttonPushed(PC2) || buttonPushed(PC3)) {
            printf("Het spel is gestart\n");
            break;
        }
    }

    return 0;
}