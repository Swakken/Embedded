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
int currentSpeed = 100;
int distance = 9999; 
int fuelReserve = 1500; // Brandstof in liter
int safe = 1; // Als alles goed gaat dan blijft je in de while loop
float burst = 0;
uint32_t teller = 0;
int win = 0; // Als je gewonnen bent


void handleBurst() {
    if (buttonPushed(BUTTON_PIN2)) {
        if (fuelReserve > 50) {
            burst = 50; 
        } else {
            burst = fuelReserve;
        }
    } else {
        burst = 0;
    }
}

void updateBurst()
{
  // als je op de knop drukt, verhoogt de burst
  // mag niet meer dan 50 liter per seconde
  if (buttonPushed(2) && fuelReserve > 0) {
    burst++;
    if (burst >= 50) burst = 50;
  } else {
    burst = 0;
  }
}

void updateParameters() {
    currentSpeed += gravity - burst / 5;
    distance -= currentSpeed;
    fuelReserve -= burst;
}

void updateDisplay() {

    if (distance <= 0) {
        safe = 0; 
        distance = 0; 
        
    }
    if (burst == 0) {
        currentSpeed -= gravity / 5; 
    }
}

void showParametersOnSerial() {
    printf("Afstand: %d m\n", distance);
    printf("Snelheid: %d m/s\n", currentSpeed);
    printf("Brandstof: %d liter\n\n", fuelReserve);
}

void showParameters() {
    writeNumber(distance);
    showLeds();
}

ISR(TIMER0_COMPA_vect) {
    teller++;

    // We voeren iets uit na 1 seconden
    if (teller % 250 == 0) {
        handleBurst();
        updateParameters();
        showParametersOnSerial();
        showParameters();  // Update de weergave
    }

    if ((teller % 25) == 0) {
        updateBurst();
    }
}

void showLeds() {
    
    if (fuelReserve == 1500)
    {
        lightUpAllLeds();
    }

    if (fuelReserve <= 0) {
        lightDownAllLeds();
        return;
    }

    if (fuelReserve <= 1125) {
        lightDownLed(3);
    }

    if (fuelReserve <= 750) {
        lightDownLed(2);
    }

    if (fuelReserve <= 375) {
        lightDownLed(1);
    }

    if (fuelReserve <= 0) {
        lightDownAllLeds();
    }
}

void checkWin() {
    
    // Als we winnen
    if (distance <= 3 && currentSpeed <= 5) {
        safe = 0;
        win = 1;
    } 

    // Als we verliezen
    if (distance <= 0 || fuelReserve <= 0) {
        safe = 1;
        win = 0;
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
        checkWin();
 
    }
    printf("Spel is afgelopen\n");
   
}
