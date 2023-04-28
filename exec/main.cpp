/*
 * Fichier: Probleme2.cpp
 * Auteurs: John Abou Nakoul et Jérôme Fréchette
 * Date: 23 fevrier 2023
 *
 */

#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can.h"
#include "can.cpp"
#include "Led.hpp"
#include "Point.h"
#include "Communication.hpp"
#include "memoire_24.h"
#include "Navigation.hpp"
#include "Timer.hpp"
#include "Sound.hpp"
#include "Button.hpp"
#include <stdio.h>
#include <math.h>
const uint16_t LOW_INTENSITY = 30;
const uint16_t HIGH_INTENSITY = 100;

Timer timer;
Led led(&PORTA, &DDRA, PA1, PA0);
Button buttonInterrupt(&DDRD, PD2);
Button buttonWhite(&DDRD, PD3);
Memoire24CXXX mem;
volatile uint16_t mode = 0;

uint16_t nPoteaux;
uint16_t* listeX;
uint16_t* listeY;
uint16_t* enveloppeX;
uint16_t* enveloppeY;
uint16_t* nouvelleEnveloppeX;
uint16_t* nouvelleEnveloppeY;
uint16_t* listeATrier;
uint16_t nouvelleTaille = 0;
Communication comm;
enum class State
{
    ORTHOGONAL,
    DIAGONAL,
    FOUND,
    LOST,
    WAIT
};

ISR(TIMER1_COMPA_vect)
{
    timer.isExpired = true;
    if(mode == 1)
    {
        led.setOff();
        _delay_ms(50);
        timer.startTimer_ms(50);
        led.setGreen();
    }
}
ISR(INT1_vect)
{
    buttonWhite.hasBeenPressed = true;
    buttonWhite.isPressed = !buttonWhite.isPressed;
}
ISR(INT0_vect)
{
    buttonInterrupt.hasBeenPressed = true;
    buttonInterrupt.isPressed = !buttonInterrupt.isPressed;
}
void initInterrupt(void)
{
    cli();
    EICRA &= ~(1 << ISC01);
    EICRA |= (1 << ISC00);
    EIMSK = (1 << INT0) | (1 << INT1);
    sei();
}

/*uint32_t crc(uint bitString[32], uint8_t polynome)
{
    uint32_t shiftRegister = 0xffffffff;
    for(uint8_t i = 0; i < 32; i++)
    {
        if(((shiftRegister & 1) ^ bitString[i]) & 1)
        {
            shiftRegister = (shiftRegister << 1) ^ polynome;
        }
        else
        {
            shiftRegister = (shiftRegister << 1);
        }
    }
    return shiftRegister;
}*/


void detect()
{
    Led led(&PORTA, &DDRA, PA1, PA0);
    State state = State::ORTHOGONAL;
    Navigation nav;
    Communication comm;
    uint16_t reading = 0;
    uint16_t nPosts = 0;
    uint16_t address = 1;
    can converter = can();
    DDRA &= ~(1 << PA3);
    comm.initUART();
    initInterrupt();
    Sound sound;
    Point pos(0, 0, 0, 1);
    timer.startTimer_ms(1000);
    while (!timer.isExpired)
    {
        sound.playNote(60);
    }
    sound.stopSound();
    _delay_ms(2000);
    timer.stopTimer();
    buttonInterrupt.hasBeenPressed = false;
    buttonWhite.hasBeenPressed = false;
    while (true)
    {
        led.setAmber();
        if (buttonInterrupt.hasBeenPressed)
        {

            led.setGreen();
            _delay_ms(2000);
            led.setOff();
            break;
        }
        if (buttonWhite.hasBeenPressed)
        {
            led.setRed();
            _delay_ms(2000);
            led.setOff();
            nav.rotateLeft(150);
            _delay_ms(1100);
            nav.idle();
            break;
        }
    }
    for (uint16_t i = 0; i < 8; i++)
    {
        switch (state)
        {
        case State::ORTHOGONAL:
            reading = 0;
            for (uint16_t j = 0; j < 5; j++)
            {
                reading += converter.lecture(PA3) >> 2;
                _delay_ms(10);
            }
            reading = reading / 5;
            if (reading > 20)
            {
                pos.updatePosition(1);
                state = State::FOUND;
                break;
            }
            pos.updateDirections();
            for (int k = 0; k < 15; k++)
            {
                timer.startTimer_ms(100);
                while (!timer.isExpired)
                {
                    led.setGreen();
                    nav.rotateRight(200);
                }
                led.setOff();
                nav.idle();
                reading = 0;
                for (uint16_t j = 0; j < 25; j++)
                {
                    reading += converter.lecture(PA3) >> 2;
                    _delay_ms(10);
                }
                reading = reading / 25;
                if (reading > 16)
                {
                    timer.startTimer_ms(100);
                    while (!timer.isExpired)
                    {
                        nav.rotateRight(200);
                    }
                    nav.idle();
                    reading = converter.lecture(PA3) >> 2;
                    break;
                }
            }
            if (reading > 30) // 1 DIAGONAL
            {
                led.setGreen();
                pos.updatePosition(1);
                state = State::FOUND;
            }
            else if (reading >= 18 && reading <= 30) // 2 DIAGONAL
            {
                led.setRed();
                pos.updatePosition(2);
                state = State::FOUND;
            }
            else
            {
                state = State::DIAGONAL;
            }
            break;
        case State::DIAGONAL:
            reading = 0;
            for (uint16_t j = 0; j < 5; j++)
            {
                reading += converter.lecture(PA3) >> 2;
                _delay_ms(10);
            }
            reading = reading / 5;
            if (reading > 16)
            {
                pos.updatePosition(1);
                state = State::FOUND;
                break;
            }
            pos.updateDirections();
            for (int k = 0; k < 15; k++)
            {
                timer.startTimer_ms(100);
                while (!timer.isExpired)
                {
                    led.setGreen();
                    nav.rotateRight(200);
                }
                led.setOff();
                nav.idle();
                reading = 0;
                for (uint16_t j = 0; j < 25; j++)
                {
                    reading += converter.lecture(PA3) >> 2;
                    _delay_ms(10);
                }
                reading = reading / 25;
                if (reading > 30)
                {
                    timer.startTimer_ms(100);
                    while (!timer.isExpired)
                    {
                        nav.rotateRight(200);
                    }
                    nav.idle();
                    reading = converter.lecture(PA3) >> 2;
                    break;
                }
            }
            if (reading > 35) // 1 tout droit
            {
                led.setGreen();
                pos.updatePosition(1);
                state = State::FOUND;
            }
            else if (reading >= 20 && reading <= 35) // 1 tout droit
            {
                led.setRed();
                pos.updatePosition(2);
                state = State::FOUND;
            }
            else
            {
                state = State::ORTHOGONAL;
            }
            break;

        case State::FOUND:
            while (reading < 75)
            {
                nav.moveForward(150);
                reading = converter.lecture(PA3) >> 2;
                if (reading < 20)
                {
                    state = State::LOST;
                    break;
                }
            }
            if (state == State::LOST)
            {
                nav.idle();
                break;
            }
            nav.idle();
            mem.ecriture(address++, pos.x);
            _delay_ms(5);
            mem.ecriture(address++, pos.y);
            sound.playNote(24, 300);
            _delay_ms(300);
            sound.playNote(24, 300);
            _delay_ms(300);
            sound.playNote(24, 300);
            _delay_ms(300);
            i = 0;
            state = State::WAIT;
            break;
        case State::LOST:
            reading = 0;
            for (int k = 0; k < 5; k++)
            {
                timer.startTimer_ms(100);
                while (!timer.isExpired)
                {
                    led.setGreen();
                    nav.rotateLeft(200);
                }
                led.setOff();
                nav.idle();
                reading = 0;
                for (uint16_t j = 0; j < 5; j++)
                {
                    reading += converter.lecture(PA3) >> 2;
                    _delay_ms(10);
                }
                reading = reading / 5;
                if (reading > 18)
                {
                    break;
                }
            }
            if (reading > 18)
            {
                state = State::FOUND;
                break;
            }
            reading = 0;
            for (int k = 0; k < 10; k++)
            {
                reading = 0;
                timer.startTimer_ms(100);
                while (!timer.isExpired)
                {
                    led.setRed();
                    nav.rotateRight(200);
                }
                led.setOff();
                nav.idle();
                reading = 0;
                for (uint16_t j = 0; j < 5; j++)
                {
                    reading += converter.lecture(PA3) >> 2;
                    _delay_ms(10);
                }
                reading = reading / 5;
                if (reading > 18)
                {
                    break;
                }
            }
            if (reading > 18)
            {
                state = State::FOUND;
                break;
            }
            reading = 0;
            for (int k = 0; k < 5; k++)
            {
                timer.startTimer_ms(100);
                while (!timer.isExpired)
                {
                    led.setGreen();
                    nav.rotateLeft(200);
                }
                led.setOff();
                nav.idle();
                reading = 0;
                for (uint16_t j = 0; j < 5; j++)
                {
                    reading += converter.lecture(PA3) >> 2;
                    _delay_ms(10);
                }
                reading = reading / 5;
                if (reading > 18)
                {
                    break;
                }
            }
            if (reading > 18)
            {
                state = State::FOUND;
                break;
            }
            break;
        case State::WAIT:
            buttonInterrupt.hasBeenPressed = false;
            while (!buttonInterrupt.hasBeenPressed) {
                led.setAmber_ms(250);
                led.setOff();
                _delay_ms(250);
            }
            pos.directionX = 0;
            pos.directionY = 1;
            nPosts++;
            state = State::ORTHOGONAL;
            break;
        }
        reading = converter.lecture(PA3) >> 2;
    }

    mem.ecriture(0, nPosts);
    sound.playNote(80, 2000);
    while(true)
    {
        led.setRed();
        _delay_ms(250);
        led.setOff();
        _delay_ms(250);
    }
}

void lectureMemoire()
{
    uint16_t address = 0;
    mem.lecture(address++, &nPoteaux);
    listeX = new uint16_t[nPoteaux];
    listeY = new uint16_t[nPoteaux];
    enveloppeX= new uint16_t[nPoteaux];
    enveloppeY= new uint16_t[nPoteaux];
    nouvelleEnveloppeX= new uint16_t[nPoteaux];
    nouvelleEnveloppeY= new uint16_t[nPoteaux];
    listeATrier= new uint16_t[nPoteaux * 2];
    uint16_t x = 0;
    uint16_t y = 0;
    for (uint16_t i = 0; i < nPoteaux; i++)
    {
        mem.lecture(address++, &x);
        mem.lecture(address++, &y);
        x = (x * 110) + 192;
        listeX[i] = x;
        y = (y * 110) + 123;
        listeY[i] = y;
    }
}

uint16_t orientation(uint16_t p, uint16_t q, uint16_t r)
{
    uint16_t val = (listeY[q] - listeY[p]) * (listeX[r] - listeX[q]) - (listeX[q] - listeX[p]) * (listeY[r] - listeY[q]);
    if (val == 0)
    {
        return 0;
    }
    return (val > 0) ? 1:2;
}

void enveloppeConvexe()
{
    uint16_t enveloppe[nPoteaux];
    for (uint16_t i = 0; i < nPoteaux; i++)
    {
         enveloppe[i] = 0;
    }

    uint16_t l = 0;
    for (uint16_t i = 1; i < nPoteaux; i++)
        if (listeX[i] < listeX[l])
            l = i;

    uint16_t p = l, q;
    do
    {
        enveloppe[p] = 1; 

        q = (p + 1) % nPoteaux;
        for (uint16_t r = 0; r < nPoteaux; r++)
            if (orientation(p, q, r) == 2)
                q = r;

        p = q; 
    } while (p != l); 

    uint16_t taille = 0;
    for (uint16_t i = 0; i < sizeof(enveloppe)/sizeof(enveloppe[0]); i++)
    {
        if (enveloppe[i] == 1){
            enveloppeX[taille] = listeX[i];
            enveloppeY[taille] = listeY[i];
            taille++;
        }
    }
    
    for (uint16_t i = 0; i < taille; i++)
    {
        bool contained = false;
        for (uint16_t j = 0; j < nouvelleTaille; j++)
        {
            if (enveloppeX[i] == nouvelleEnveloppeX[j] && enveloppeY[i] == nouvelleEnveloppeY[j])
            {
                contained = true;
                break;
            }
        }

        if (!contained) {
            nouvelleEnveloppeX[nouvelleTaille] = enveloppeX[i];
            nouvelleEnveloppeY[nouvelleTaille] = enveloppeY[i];
            nouvelleTaille++;
        }
    }
   
    uint16_t compteur = 0;

    for (int i = 0; i < nouvelleTaille; i++)
    {
        listeATrier[compteur] = nouvelleEnveloppeX[i];
        listeATrier[compteur + 1] = nouvelleEnveloppeY[i];
        compteur += 2;
    }
}

void triageListe()
{
    enveloppeConvexe();
    double x = 0, y = 0;
    for (uint16_t i = 0; i < nouvelleTaille * 2; i += 2)
    {
        x += listeATrier[i];
        y += listeATrier[i + 1];
    }
    x /= nouvelleTaille;
    y /= nouvelleTaille;

    for (uint16_t i = 0; i < nouvelleTaille * 2; i += 2)
    {
        double angle1 = atan2(listeATrier[i + 1] - y, listeATrier[i] - x);
        for (uint16_t j = i + 2; j < nouvelleTaille * 2; j += 2)
        {
            double angle2 = atan2(listeATrier[j + 1] - y, listeATrier[j] - x);
            if (angle1 > angle2)
            {
         
                uint16_t temp = listeATrier[i];
                listeATrier[i] = listeATrier[j];
                listeATrier[j] = temp;
                temp = listeATrier[i + 1];
                listeATrier[i + 1] = listeATrier[j + 1];
                listeATrier[j + 1] = temp;
                angle1 = angle2;
            }
        }
    }
    uint16_t compteur = 0;
    for (uint16_t i = 0; i < nouvelleTaille; i++)
    {
        nouvelleEnveloppeX[i] = listeATrier[compteur];
        nouvelleEnveloppeY[i] = listeATrier[compteur + 1];
        compteur += 2;
    }

}

uint16_t airPrisme(uint16_t nouvelleTaille, uint16_t listeFinaleX[], uint16_t listeFinaleY[])
{
    uint16_t aire = 0.0;
    
    for (uint16_t i = 0; i < nouvelleTaille; i++) 
    {
        uint16_t index = (i + 1) % nouvelleTaille; 
        aire += listeFinaleX[i] * listeFinaleY[index] - listeFinaleY[i] * listeFinaleX[index];
    }

    if(aire < 0)
    {
        aire *= -1;
    }

    return (aire/2.0) / 100;
}


void transmettreMessage(char* message, int size) 
{
    for (int i = 0; i < size; i++)
    { 
        comm.transmissionUART(*(message+i));
    }
}

void poteauxADessiner()
{
    char buffer[50];
    char buffer2[90];
    for (int i = 0; i < nPoteaux; i++)
    {
        sprintf(buffer, "<circle cx=\"%d\" cy=\"%d\" r=\"10\" ", listeX[i], listeY[i]);
        transmettreMessage(buffer, sizeof(buffer)-1);
        sprintf(buffer2, "stroke=\"black\" stroke-width=\"4\" fill=\"grey\" />\n");
        transmettreMessage(buffer2, sizeof(buffer2)-1);
    }
}

void aireAColorier()
{
    triageListe();
    char buffer[60];
    char buffer2[90];
    char buffer3[60];
    for(int i = 0; i < nouvelleTaille; i++)
    {
        sprintf(buffer3, "\"%d\",\"%d\" ", nouvelleEnveloppeX[i], nouvelleEnveloppeY[i]);
    }

    for (int i = 0; i < nouvelleTaille; i++)
    {
        sprintf(buffer, "<polygon points=\"%s\"", buffer3);
        transmettreMessage(buffer, sizeof(buffer)-1);
        sprintf(buffer2, "style=\"fill:green;stroke:black;stroke-width:3\" />\n");
        transmettreMessage(buffer2, sizeof(buffer2)-1);
    }
}
    
void aireAEcrire()
{
    triageListe();
    int aire = airPrisme(nouvelleTaille, nouvelleEnveloppeX, nouvelleEnveloppeY);
    char buffer[100];
    char buffer2[100];
    for (int i = 0; i < nouvelleTaille; i++)
    {
        sprintf(buffer, "<text x=\"96\" y=\"564\" font-family=\"arial\" font-size=\"20\"");
        transmettreMessage(buffer, sizeof(buffer)-1);
        sprintf(buffer2, "fill=\"blue\">AIRE: \"%d\" pouces carrees</text>\n", aire);
        transmettreMessage(buffer2, sizeof(buffer2)-1);
    }
}


void envoyerSVG()
{
            char ligne1[] = "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"100%\" height=\"100%\" viewBox=\"0 0 1152 576\" class=\"__web-inspector-hide-shortcut__\">\n";
            transmettreMessage(ligne1, sizeof(ligne1)-1);
            char ligne2[] = "<text x=\"96\" y=\"36\" font-family=\"arial\" font-size=\"20\" fill=\"blue\">INF1900 -- Équipe -- 3536 -- Bin2Dec</text><rect x=\"96\" y=\"48\" width=\"960\" height=\"480\" stroke=\"black\" stroke-width=\"1\" fill=\"white\"/>\n";
            transmettreMessage(ligne2, sizeof(ligne2)-1);
           
            aireAColorier();
    
            char message1[] = "<rect x=\"192\" y=\"123\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message1, sizeof(message1)-1);
            char message2[] = "<rect x=\"192\" y=\"233\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message2, sizeof(message2)-1);
            char message3[] = "<rect x=\"192\" y=\"343\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message3, sizeof(message3)-1);
            char message4[] = "<rect x=\"192\" y=\"453\" width=\"5\" height=\"5\" stroke=\"red\" fill=\"red\" stroke-width=\"5\"/>\n";
            transmettreMessage(message4, sizeof(message4)-1);

            char message5[] =  "<rect x=\"302\" y=\"123\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message5, sizeof(message5)-1);
            char message6[] = "<rect x=\"302\" y=\"233\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message6, sizeof(message6)-1);
            char message7[] = "<rect x=\"302\" y=\"343\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message7, sizeof(message7)-1);
            char message8[] = "<rect x=\"302\" y=\"453\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message8, sizeof(message8)-1);

            char message9[] = "<rect x=\"412\" y=\"123\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message9, sizeof(message9)-1);
            char message10[] = "<rect x=\"412\" y=\"233\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message10, sizeof(message10)-1);
            char message11[] = "<rect x=\"412\" y=\"343\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message11, sizeof(message11)-1);
            char message12[] = "<rect x=\"412\" y=\"453\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message12, sizeof(message12)-1);

            char message13[] = "<rect x=\"522\" y=\"123\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message13, sizeof(message13)-1);
            char message14[] = "<rect x=\"522\" y=\"233\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message14, sizeof(message14)-1);
            char message15[] = "<rect x=\"522\" y=\"343\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message15, sizeof(message15)-1);
            char message16[] = "<rect x=\"522\" y=\"453\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message16, sizeof(message16)-1);

            char message17[] = "<rect x=\"632\" y=\"123\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message17, sizeof(message17)-1);
            char message18[] = "<rect x=\"632\" y=\"233\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message18, sizeof(message18)-1);
            char message19[] = "<rect x=\"632\" y=\"343\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message19, sizeof(message19)-1);
            char message20[] = "<rect x=\"632\" y=\"453\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message20, sizeof(message20)-1);

            char message21[] = "<rect x=\"742\" y=\"123\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message21, sizeof(message21)-1);
            char message22[] = "<rect x=\"742\" y=\"233\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message22, sizeof(message22)-1);
            char message23[] = "<rect x=\"742\" y=\"343\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message23, sizeof(message23)-1);
            char message24[] = "<rect x=\"742\" y=\"453\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message24, sizeof(message24)-1);

            char message25[] = "<rect x=\"852\" y=\"123\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message25, sizeof(message25)-1);
            char message26[] = "<rect x=\"852\" y=\"233\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message26, sizeof(message26)-1);
            char message27[] = "<rect x=\"852\" y=\"343\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message27, sizeof(message27)-1);
            char message28[] = "<rect x=\"852\" y=\"453\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message28, sizeof(message28)-1);

            char message29[] = "<rect x=\"962\" y=\"123\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message29, sizeof(message29)-1);
            char message30[] = "<rect x=\"962\" y=\"233\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message30, sizeof(message30)-1);
            char message31[] = "<rect x=\"962\" y=\"343\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message31, sizeof(message31)-1);
            char message32[] = "<rect x=\"962\" y=\"453\" width=\"5\" height=\"5\" stroke=\"black\" fill=\"black\" stroke-width=\"5\"/>\n";
            transmettreMessage(message32, sizeof(message32)-1);

            poteauxADessiner();
            aireAEcrire();
            char lastLine[] = "</svg>";
            transmettreMessage(lastLine, sizeof(lastLine)-1);
}
void transmit()
{
    Led led(&PORTA, &DDRA, PA1, PA0);
    buttonInterrupt.hasBeenPressed = false;
    buttonWhite.hasBeenPressed = false;
    led.setGreen();
    _delay_ms(2000);
    led.setOff();

    timer.startTimer_ms(100);
    led.setGreen();
}

int main()
{
    initInterrupt();
    
    while(true){
        if(buttonWhite.hasBeenPressed)
        {
            mode = 1;
            transmit();
        }
        if(buttonInterrupt.hasBeenPressed)
        {
            mode = 0;
            detect();
        }
    }
    envoyerSVG();
}