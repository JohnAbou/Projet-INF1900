/*
 * Fichier: Probleme2.cpp
 * Auteurs: John Abou Nakoul et Jérôme Fréchette
 * Date: 23 fevrier 2023
 *
 * Description:
 * Allumage d'une DEL à l'aide d'une photorésistance.
 * Si la lumière est basse (en cachant la photorésistance), la DEL prendra la couleur verte.
 * Si la lumière est à un bon niveau (lumière ambiante), la DEL tournera à l'ambré.
 * Si la lumière est trop forte (photorésistance sous une lampe de poche), la DEL devient rouge.
 *
 * Identification matérielle : Résistance de 10K (Digi-Key: S10KQTR-ND), petit condensateur de 0.1 µF (Digi-Key: BC1621-ND) et photorésistance (Digi-Key: PDV-P8101-ND)
 * sur le breadboard, fil male-femelle connecte sur VCC et GND du PORTB au breadboard, fil male-femelle reliant les données à un port analogique de votre
 * choix (de PA0 à PA7), fil femelle-femelle connecte DEL libre à B0-B1.
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
const uint16_t LOW_INTENSITY = 30;
const uint16_t HIGH_INTENSITY = 100;

Timer timer;

enum class State
{
    ORTHOGONAL,
    DIAGONAL,
    FOUND,
    WAIT
};

ISR(TIMER1_COMPA_vect)
{
    timer.stopTimer();
}

void initInterrupt(void)
{
    cli();
    DDRD &= ~(1 << PD2);

    EICRA &= ~(1 << ISC01);
    EICRA |= (1 << ISC00);

    EIMSK = (1 << INT0);
    sei();
}

int main()
{
    Led led(&PORTA, &DDRA, PA1, PA0);
    State state = State::ORTHOGONAL;
    Navigation nav;

    Communication comm;
    Memoire24CXXX mem;

    uint16_t reading = 0;

    can converter = can();
    DDRA &= ~(1 << PA3);
    comm.initUART();
    Sound sound;

    Point pos(0,0,1,0);
    initInterrupt();

    timer.startTimer_ms(1000);
    while (!timer.isExpired)
    {
        sound.playNote(60);
        // reading = converter.lecture(PA3) >> 2;
    }
    sound.stopSound();
    _delay_ms(2000);

    for (uint16_t i = 0; i < 8; i++)
    {
        for(int k = 0; k < 5; k++)
        {
            led.setRed();
            timer.startTimer();
            while (!timer.isExpired && reading < 30)
            {
                nav.rotateRight(150);
                reading = converter.lecture(PA3) >> 2;
            }
            nav.idle();
            reading = 0;
            for(uint16_t j = 0; j < 25; j++)
            {
                reading += converter.lecture(PA3) >> 2;
                _delay_ms(10);
            }
            reading = reading/25;
            if(reading > 30)
            {
                break;
            }
        }
        led.setOff();
        
        switch (state)
        {
        case State::ORTHOGONAL:
            if (reading >= 50)
            {
                led.setGreen();
                pos.updatePosition(1);
                state = State::FOUND;
            }
            else
            {
                pos.updateDirections();
                state = State::DIAGONAL;
            }
            break;
        case State::DIAGONAL:
            if (reading >= 40)
            {
                led.setRed();
                pos.updatePosition(1);
                state = State::FOUND;
            }
            else
            {
                pos.updateDirections();
                state = State::ORTHOGONAL;
            }
            break;

        case State::FOUND:
            while(reading < 130)
            {
                nav.moveForward(50);
                reading = converter.lecture(PA3) >> 2;
            }
            nav.idle();

            comm.transmissionUART(pos.x);
            comm.transmissionUART(pos.y);

            sound.playNote(24, 300);
            _delay_ms(300);
            sound.playNote(24, 300);
            _delay_ms(300);
            sound.playNote(24, 300);
            _delay_ms(300);

            i = 0;
            state = State::WAIT
            break;

        case State::WAIT:
            while(true){}
            break;
        }
        
        reading = converter.lecture(PA3) >> 2;
    }
    led.setGreen();
}