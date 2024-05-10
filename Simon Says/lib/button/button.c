#include "button.h"

#define BUTTON_PORT PORTC
#define BUTTON_DDR DDRC
#define BUTTON_PIN PINC

void enableButton(int button) {
    BUTTON_DDR &= ~_BV(button);
    BUTTON_PORT |= _BV(button);
} 

void enableAllButtons() {
  DDRC &= ~_BV(PC1) & ~_BV(PC2) & ~_BV(PC3);
  PORTC |= _BV(PC1) | _BV(PC2) | _BV(PC3);
}

int buttonPushed(int button) {
    return (BUTTON_PIN & _BV(button)) == 0;
}

int buttonReleased(int button) {
    return (BUTTON_PIN & _BV(button)) != 0;
}
