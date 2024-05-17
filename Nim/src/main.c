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
    int aantalGekozen = 1;

    while (1) {
        writeNumberToSegment(0, aantalGekozen);
        writeCharToSegment(1, 'p');
        writeNumberToSegment(2, startAantal / 10);
        writeNumberToSegment(3, startAantal % 10);

        // Verhogen van aantalGekozen
        if (buttonPushed(3)) {
            if (aantalGekozen < 3) { // Werkt niet met maxAantal?
                aantalGekozen++;
                writeNumberToSegment(0, aantalGekozen);
                printf("Waarde verhoogd met 1.\n");
                _delay_ms(300);
            }
        }

        // Verlagen van aantalGekozen
        if (buttonPushed(2)) {
            if (aantalGekozen > 1) {
                aantalGekozen--;
                writeNumberToSegment(0, aantalGekozen);
                printf("Waarde verlaagd met 1.\n");
                _delay_ms(300);
            }
        }

        // Bevestigen van de keuze
        if (buttonPushed(1)) {
            startAantal -= aantalGekozen;
            printf("Waarde bevestigd en afgetrokken van het startaantal.\n");
            writeNumberToSegment(2, startAantal / 10);
            writeNumberToSegment(3, startAantal % 10);
            _delay_ms(300);
            break;
        }
    }
    computerBeurt(startAantal, maxAantal);
}




void computerBeurt(int startAantal, int maxAantal) {
    printf("De beurt is aan de computer.\n");
    int aantalGekozen = rand() % maxAantal + 1;

    while (1) {
        writeCharToSegment(1, 'c'); 
        writeNumberToSegment(2, startAantal / 10); 
        writeNumberToSegment(3, startAantal % 10);

        if ((startAantal - 1) % (maxAantal + 1) == 0) {
            aantalGekozen = 1;
        } else {
            aantalGekozen = rand() % maxAantal + 1;
        }

        writeNumberToSegment(0, aantalGekozen);

        printf("De computer kiest %d lucifers.\n", aantalGekozen);
        printf("Bevestig de waarde van de computer door op de eerste button te drukken\n");

        // Wacht totdat de button is ingedrukt en losgelaten
        while (!buttonPushed(BUTTON_PIN)) {
            // Continue to update display during the wait
            writeCharToSegment(1, 'c');
            writeNumberToSegment(2, startAantal / 10);
            writeNumberToSegment(3, startAantal % 10);
            writeNumberToSegment(0, aantalGekozen);
        }

        while (buttonPushed(BUTTON_PIN)) {
            // Optionally keep updating or do nothing
        }

        printf("De computer waarde is bevestigd.\n");

        startAantal -= aantalGekozen;

        if (startAantal <= 0) {
            printf("Spel afgelopen\n");
            break;
        }

        // Update the display for the next iteration or exit
        writeCharToSegment(1, 'c');
        writeNumberToSegment(2, startAantal / 10);
        writeNumberToSegment(3, startAantal % 10);
        _delay_ms(200);
    }

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
    // 0 voor C (computer) en 1 voor P (player)
    int speler = rand() % 2;

    while (1) {
        if (speler == 1) {
            speler = 1;  // Wissel naar computer
            spelerBeurt(startAantal, maxAantal);
            
        } else {
            speler = 0;  // Wissel naar speler
            computerBeurt(startAantal, maxAantal);
           
        }

        if (startAantal <= 0) {
            printf("Spel afgelopen\n");
            break; 
        }
    }




    return 0;
}