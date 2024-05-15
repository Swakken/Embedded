#include "led.h"
#include <avr/io.h>


// Schakelt de LED met het opgegeven nummer in.
void enableLed(int lednumber) {//C werkt niet met classes: functies staan gewoon rechtstreeks in de .c file.
    if (lednumber<0||lednumber>NUMBER_OF_LEDS-1) return;
    DDRB |= (1 << (PB2 + lednumber)); //Check de tutorial "Schrijven naar een PIN". Uit de documentatie van de Multifunctional Shield weten we dat de leds beginnen op PB2
}

// Functie om een combinatie van LEDS te activeren op basis van de gegeven byte
void enableLeds(uint8_t leds) {
    DDRB |= leds;
}

// Functie om alle LEDS te activeren
void enableAllLeds() {
    DDRB |= 0b00111100;
}


void lightUpLed(int lednumber){//opgelet: ge-enabelde leds staan ook ineens aan (want 0 = aan)
    if (lednumber<0||lednumber>NUMBER_OF_LEDS-1) return;
    PORTB &= ~(1 << (PB2 + lednumber));//Check de tutorial "Bit operaties" om te weten wat hier juist gebeurd.
}

// Laat een combinatie van LED's oplichten op basis van de gegeven byte.
void lightUpLeds (uint8_t leds) {
    PORTB &= ~leds;
}

// Laat alle LED's oplichten.
void lightUpAllLeds() {
    PORTB &= ~(0b00111100);
}

// Schakelt de LED met het opgegeven nummer uit.
void lightDownLed(int lednumber){
    if (lednumber<0||lednumber>3) return;
    PORTB |= (1 << (PB2 + lednumber));//Zorg ook dat je deze code goed begrijpt!
}

// Schakelt een combinatie van LED's uit op basis van de gegeven byte.
void lightDownLeds(uint8_t leds) {
    PORTB |= leds;
}

// Schakelt alle LEDS uit
void lightDownAllLeds() {
    PORTB |= 0b00111100;
}

