#ifndef LED_H
#define LED_H

#include <avr/io.h>
#include <stdint.h>

#define NUMBER_OF_LEDS 4

void enableLed(int lednumber);
void enableLeds(uint8_t leds);
void enableAllLeds();

void lightUpLed(int lednumber);
void lightUpLeds(uint8_t leds);
void lightUpAllLeds();

void lightDownLed(int lednumber);
void lightDownLeds (uint8_t leds);
void lightDownAllLeds();

#endif