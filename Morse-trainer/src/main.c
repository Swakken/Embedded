#include <avr/io.h>
#include <led.h>
#include <stdlib.h>
#include <string.h>
#include "usart.h"
#include <util/delay.h>

#define __DELAY_BACKWARD_COMPATIBLE__

#define KORT 150
#define LANG 800

// Alle letters te tonen in morse code
const int data[26][4] = {
    {KORT, LANG},                 // A
    {LANG, KORT, KORT, KORT},     // B
    {LANG, KORT, LANG, KORT},     // C
    {LANG, KORT, KORT},           // D
    {KORT},                       // E
    {KORT, KORT, LANG, KORT},     // F
    {LANG, LANG, KORT},           // G
    {KORT, KORT, KORT, KORT},     // H
    {KORT, KORT},                 // I
    {KORT, LANG, LANG, LANG},     // J
    {LANG, KORT, LANG},           // K
    {KORT, LANG, KORT, KORT},     // L
    {LANG, LANG},                 // M
    {LANG, KORT},                 // N
    {LANG, LANG, LANG},           // O
    {KORT, LANG, LANG, KORT},     // P
    {LANG, LANG, KORT, LANG},     // Q
    {KORT, LANG, KORT},           // R
    {KORT, KORT, KORT},           // S
    {LANG},                       // T
    {KORT, KORT, LANG},           // U
    {KORT, KORT, KORT, LANG},     // V
    {KORT, LANG, LANG},           // W
    {LANG, KORT, KORT, LANG},     // X
    {LANG, KORT, LANG, LANG},     // Y
    {LANG, LANG, KORT, KORT}      // Z
};

const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void displayKort() {
    for (int j = 0; j < 4; j++) {
        lightUpLed(j);
    }
    _delay_ms(KORT);
    for (int j = 0; j < 4; j++) {
        lightDownLed(j);
    }
    _delay_ms(400);
}

void displayLang() {
    for (int j = 0; j < 4; j++) {
        lightUpLed(j);
    }
    _delay_ms(LANG);
    for (int j = 0; j < 4; j++) {
        lightDownLed(j);
    }
    _delay_ms(400);
}

void displayMorse(const int letter[], char ch) {
    for (int i = 0; i < 4 && letter[i] != 0; i++) {
        if (letter[i] == KORT) {
            displayKort();
        } else if (letter[i] == LANG) {
            displayLang();
        }
    }
}

int main() {
    initUSART();
    enableAllLeds();
    lightDownAllLeds();

    _delay_ms(100);
    srand(time(NULL));

    // Toont 10 willekeurige morsecodes
    for (int i = 0; i < 10; i++) {
        int randIndex = rand() % 26;
        displayMorse(data[randIndex], alphabet[randIndex]);

    // Print de overeenkomstige letter in de terminal
    printf("%c\n", alphabet[randIndex]);
    }

    return 0;
}
