#define __DELAY_BACKWARDS_COMPATIBLE__

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <usart.h>
#include <avr/interrupt.h>

#include "display.h"
#include "button.h"
#include "led.h"
#include "buz.h"

#define LED1 0
#define LED2 1
#define LED3 2
#define LED4 3

#define BUTTON_PIN PCINT1
#define BUTTON_PIN2 PCINT2
#define BUTTON_PIN3 PCINT3

float gravity = 1.622;
float currentSpeed = 100;
int distance = 9999; 
float maxFuel = 1500; // Brandstof in liter
int safe = 1; // Als alles goed gaat dan blijft je in de while loop
float burst = 0;
int teller = 0;


void showParameters() {
    
    // showLeds();
    // Leds aansturen voor brandstof
    // Led display met hoogte in meter

    writeNumber(distance);
}

void showParametersOnSerial() {
    printf("Test");
    printf("%f", maxFuel);
    printf("%f", distance);
}

void updateParameters() {
    currentSpeed += gravity - burst / 5;
    distance -= currentSpeed;
    maxFuel -= burst;

}

ISR(TIMER0_COMPA_vect) {
    teller++;

    // We voeren iets uit na 1 seconden
    if ((teller % 250) == 0) {
        updateParameters();
    }
    
    // Printf om de seconde van alle informatie
}

void showLeds() {
    if (maxFuel <= 0) {
        lightDownAllLeds();
        return 0;
    }

    if (maxFuel <= 1500 && maxFuel > 1125) {
        lightUpAllLeds();
    }

    if (maxFuel <= 1125 && maxFuel > 750) {
        lightDownLed(0);
    }

    if (maxFuel <= 750 && maxFuel > 375) {
        lightDownLed(1);
    }

    if (maxFuel <= 375 && maxFuel > 0) {
        lightDownLed(2);
    }    

}

void startUp() {
    
    enableAllLeds();
    lightDownAllLeds();
    initUSART();
    initDisplay();
    enableButton(1);

    // Timer initialiseren
    TCCR0A &= ~_BV(WGM00);
    TCCR0A |= _BV(WGM01);
    TCCR0B &= ~_BV(WGM02);
    // Prescaler instellen = 256
    TCCR0B |= _BV(CS02);
    TCCR0B &= ~_BV(CS00);
    TCCR0B &= ~_BV(CS01);

    // Compare top value
    OCR0A = 250;

    // OCRA intterupt
    TIMSK0 |= _BV(OCIE0A);

    sei();


}

int main() {
    
    startUp();

    while (safe) {
        
        showParameters();
    }


}
