#ifndef BUTTON_H
#define BUTTON_H

#include <avr/io.h>

void enableButton(int button);
void enableAllButtons();

int buttonPushed(int button);
int buttonReleased(int button);

#endif 
