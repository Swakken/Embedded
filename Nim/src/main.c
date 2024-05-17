#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <usart.h>
#include <pot.h>
#include "display.h"
#include <button.h>

#define BUTTON_PIN PC1
#define BUTTON_VERHOOG PC2
#define BUTTON_VERLAAG PC0

int aantalGekozen = 1;
int startAantal = 21;

/*
In de spelerBeurt functie is er een fout met de buttons, als ik de middelste button druk dan krijg ik volgende waarde in de terminal:

Waarde verhoogd met 1
Waarde verlaagd met 1

één van de buttons staat dus fout ingesteld, maar ik heb alles getest en lijkt te werken? 

--------------

 In de computerBeurt functie worden de volgende 3 waarden niet getoond op de display
    writeCharToSegment(1, 'p');
    writeNumberToSegment(2, startAantal / 10);
    writeNumberToSegment(3, startAantal % 10);

-------------

Hoe ga ik als de speler aan de beurt is geweest naar de computerBeurt functie en omgekeerd? 


*/


void initButton() {
    DDRC &= ~((1 << BUTTON_PIN) | (1 << BUTTON_VERHOOG) | (1 << BUTTON_VERLAAG));
    PORTC |= (1 << BUTTON_PIN) | (1 << BUTTON_VERHOOG) | (1 << BUTTON_VERLAAG);
}



void spelerBeurt(int startAantal, int maxAantal) {
    printf("Speler is aan de beurt!\n");

    while (1) {
        writeNumberToSegment(0, aantalGekozen);
        writeCharToSegment(1, 'p');
        writeNumberToSegment(2, startAantal / 10);
        writeNumberToSegment(3, startAantal % 10);

        // && aantalGekozen < maxAantal (Lijkt niet te werken? Oplossing?)
        if (buttonPushed(3)) { 
            writeNumberToSegment(0, ++aantalGekozen);
            printf("Waarde verhoogd met 1.\n");
            ++aantalGekozen;
            _delay_ms(300);
        }
        // && aantalGekozen > 1 (Lijkt niet te werken? Oplossing?)
        if (buttonPushed(2)) {
            writeNumberToSegment(0, --aantalGekozen);
            printf("Waarde verlaagd met 1.\n");
            --aantalGekozen;
            _delay_ms(300);
        }

        if (buttonPushed(1)) {
            startAantal -= aantalGekozen;
            printf("Waarde bevestigd en afgetrokken van het startaantal.\n");
            writeNumberToSegment(1, startAantal / 10);
            writeNumberToSegment(2, startAantal % 10);
            _delay_ms(300);
            break;
        }
    }
}




void computerBeurt(int *startAantal, int maxAantal) {
    
    writeCharToSegment(1, 'c'); 
    writeNumberToSegment(2, *startAantal / 10); 
    writeNumberToSegment(3, *startAantal % 10);
    writeNumberToSegment(0, aantalGekozen);

    while (1) {
        if ((*startAantal - 1) % (maxAantal + 1) == 0) {
            aantalGekozen = 1;
        } else {
            aantalGekozen = rand() % maxAantal + 1;
        }

        while (!buttonPushed(BUTTON_PIN)) {
            
        }
        
        printf("De beurt is aan de computer.\n");
        printf("De computer kiest %d lucifers.\n", aantalGekozen);
        printf("Bevestig de waarde van de computer door op de eerste button te drukken\n");

        startAantal -= aantalGekozen;

        writeNumberToSegment(2, *startAantal / 10);
        writeNumberToSegment(3, *startAantal % 10);
        _delay_ms(200);

        if (*startAantal <= 0) {
            break;
        }
    }
    
    // Geef de controle door aan de speler
    spelerBeurt(startAantal, maxAantal);
}

int main() {
    initUSART();
    startPotentiometer();
    initDisplay(); 
    initButton();

    uint16_t value = 0;
    unsigned int seed = 0;

    enableAllButtons();

    printf("Draai aan de potentiometer om de seed te kiezen en druk op de knop om te starten.\n");

    while (!buttonPushed(BUTTON_PIN)) {
        value = readPotentiometer();
        writeNumberAndWait(value % 10000, 200);
    }

    seed = value % 10000;
    srand(seed);
    printf("Game started with seed: %u\n", seed);

    
    int maxAantal = (rand() % 3) + 1;
    // 0 voor C en 1 voor P
    int speler = rand() % 2;

    while (1) {
        if (speler == 1) {
            spelerBeurt(startAantal, maxAantal);
            speler = 1;
        } else {
            computerBeurt(startAantal, maxAantal);
            speler = 0;
        }

        if (startAantal <= 0) {
            printf("Spel afgelopen\n");
            break; 
        }
    }

    return 0;
}
