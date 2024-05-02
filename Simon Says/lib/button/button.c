#include "button.h"

#define BUTTON_PORT PORTC
#define BUTTON_DDR DDRC
#define BUTTON_PIN PINC

void enableButton(int button) {
    BUTTON_DDR &= ~_BV(button);
    BUTTON_PORT |= _BV(button);
} 

int buttonPushed(int button) {
    return (BUTTON_PIN & _BV(button)) == 0;
}

int buttonReleased(int button) {
    return (BUTTON_PIN & _BV(button)) != 0;
}
