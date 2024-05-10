#ifndef BUTTON_H
#define BUTTON_H

#include <avr/io.h>

void enableButton(int button);
void enableButtons();
int buttonPushed(int button);
int buttonReleased(int button);

#endif 
