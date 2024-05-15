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




void handleBurst() {
    if (buttonPushed(BUTTON_PIN2)) {
        if (maxFuel > 50) {
            burst = 50; // Gebruik maximaal 50 liter per seconde als de knop wordt ingedrukt
        } else {
            burst = maxFuel; // Gebruik resterende brandstof als minder dan 50 liter over is
        }
    } else {
        burst = 0; // Geen burst als de knop niet wordt ingedrukt
    }
}


void updateLedBlinkRate() {
    float remainingFuelPercent = (maxFuel / 1500.0) * 100;
    int blinkRate = 1000; // basis knipperfrequentie in ms

    if (remainingFuelPercent < 25) {
        blinkRate = 250; // sneller knipperen als brandstof laag is
    } else if (remainingFuelPercent < 50) {
        blinkRate = 500;
    } else if (remainingFuelPercent < 75) {
        blinkRate = 750;
    }

    // Leds laten flikkeren

}

void updateDisplay() {
    currentSpeed += gravity - burst / 5;
    distance -= (int)currentSpeed;
    maxFuel -= burst;
    if (distance <= 0) {
        safe = 0; // Stop het spel als de lander de maanoppervlakte heeft bereikt
        distance = 0; // Voorkom een negatieve afstand
    }
    if (burst == 0) {
        currentSpeed -= gravity / 5; // Zwaartekracht moet nog worden afgetrokken als er geen burst is
    }
}


void showParametersOnSerial() {
    printf("Voor printf - Snelheid: %.2f, Brandstof: %.1f\n", currentSpeed, maxFuel);
    printf("Afstand: %d m\n", distance);
    printf("Snelheid: %.2f m/s\n", currentSpeed);
    printf("Brandstof: %.1f liter\n\n", maxFuel);
}

void showParameters() {
    
    // showLeds();
    // Leds aansturen voor brandstof
    // Led display met hoogte in meter

    writeNumber(distance);
    showLeds();
}

ISR(TIMER0_COMPA_vect) {
    teller++;
    printf("ISR Called: %d\n", teller);

    // We voeren iets uit na 1 seconden
    if (teller % 250 == 0) {  // Elke seconde bij een 1kHz timer frequentie en prescaler van 256
        handleBurst();        // Handleer de burst voordat je de display en snelheid update
        updateDisplay();
        showParametersOnSerial();
        showParameters();  // Update de weergave
    }
    
    // Printf om de seconde van alle informatie
}

void showLeds() {
    
    lightUpLed(LED1);
    lightUpLed(LED2);
    lightUpLed(LED3);
    lightUpLed(LED4);

    if (maxFuel <= 0) {
        lightDownAllLeds();
        return;
    }

    if (maxFuel <= 1500 && maxFuel > 1125) {
        lightUpAllLeds();
    }

    if (maxFuel <= 1125 && maxFuel > 750) {
        lightDownLed(LED1);
    }

    if (maxFuel <= 750 && maxFuel > 375) {
        lightDownLed(LED2);
    }

    if (maxFuel <= 375 && maxFuel > 0) {
        lightDownLed(LED3);
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
