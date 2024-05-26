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
int burst = 0;
uint32_t teller = 0;
int win = 0; // Als je gewonnen bent


ISR(PCINT1_vect){
  // interrupt voor onze button
  if (buttonPushed(1) && fuelReserve > 0){
    burst++;
    printf("\n::: BURST ++ :::\n");
    if (burst <= 50) burst = 50;
    } else {
      burst = 0;
    }
}

void showParameters() {
    // Leds aansturen voor brandstof
    showLeds();
    // Led display toont hoogte in meter
    writeNumber(distance);

}

// Gegevens worden gedeeld in de seriële terminal
void showParametersOnSerial() {
    printf("Afstand: %d m\n", distance);
    printf("Snelheid: %d m/s\n", currentSpeed);
    printf("Brandstof: %d liter\n\n", fuelReserve);
}


void updateBurst() {
    // Als je op de button drukt, verhoogt de burst
    // Burst != 50 liter per seconde
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
        if (currentSpeed < 0) {
            currentSpeed = 0;
        }
    }
}

void showLeds() {
    
    if (fuelReserve == 1500) {
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
        safe = 0;
        win = 0;
    } 
    
}   


ISR(TIMER0_COMPA_vect) {
    teller++;

    if (teller % 250 == 0) {
        updateParameters();
        showParametersOnSerial();
        showParameters();
    }

    if ((teller % 25) == 0) {
        updateBurst();
    }
}

void startUp() {
    
    enableAllLeds();
    lightDownAllLeds();
    initUSART();
    initDisplay();
    enableButton(1);

    // Timer initialiseren
    // Mode of operation is CTC
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

    // Interrupt enabelen
    sei();
}

int main() {
    
    startUp();

    while (safe) {       
        showParameters();
        checkWin();
    }

    printf("Spel is afgelopen\n");
    if (win) {
        printf("Je hebt gewonnen!\n");
    }
    else
        printf("Je hebt verloren!\n");
    return 0; 

}