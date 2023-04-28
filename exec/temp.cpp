// INF1900_projet tests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Timer.hpp"
#include "Point.h"
#include "Navigation.hpp"
#include "Button.hpp"
#include "can.h"

Timer timer;
Button button;
const uint8_t N_READINGS = 30;

enum class State
{
   ORTHOGONAL,
   DIAGONAL,
   FOUND,
   WAIT   
};

ISR(TIMER1_COMPA_vect)
{
  timer.isExpired = true;
}

ISR(INT0_vect)
{
  button.hasBeenPressed = true;
  button.isPressed = !button.isPressed;
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
    Navigation nav(&PORTD, &DDRD, PD4, PD5);
    Point pos(0,0,0,1);
    can converter = can();
    State state = State::ORTHOGONAL;
    Sound sound;
    uint8_t address = 0;
    uint8_t positions[64] = {};

    uint8_t reading = 0;
    uint8_t nRotations = 8;
    uint8_t nPoteaux = 0;

    initInterrupt();
    for (int i = 0; i < nRotations; i++)
    {
        switch(state)
        {
            case ORTHOGONAL:
            for(uint8_t j = 0; j < N_READINGS; j++)
            {
                reading += converter.lecture(PA3) >> 2;
            }
            reading = reading/N_READINGS;

            if(reading >= 65 && reading <= 80)
            {
                while(reading < 130)
                {
                    nav.moveForward(50);
                    reading = converter.lecture(PA3) >> 2;
                }
                nav.idle();
                positions[address++] = pos.x + 1 * pos.directionX;
                positions[address++] = pos.y + 1 * pos.directionY;
                pos.updatePosition(1);
                state = State::FOUND;
            }
            else if(reading >= 15 && reading <= 30)
            {
                while(reading < 130)
                {
                    nav.moveForward(50);
                    reading = converter.lecture(PA3) >> 2;
                }
                nav.idle();
                positions[address++] = pos.x + 2 * pos.directionX;
                positions[address++] = pos.y + 2 * pos.directionY;
                pos.updatePosition(2);
                state = State::FOUND;
            }
            else
            {
                timer.startTimer(temps pour le faire tourner de 45 degree);
                while (!timer.isExpired && reading <= 40)
                {
                    nav.rotateRight(vitesse pour le faire tourner de 45 degree);
                    reading = converter.lecture(PA3) >> 2;
                }
                nav.idle();
                pos.updateDirections();
                state = State::DIAGONAL;
            }
            break;
            case DIAGONAL:
            for(uint8_t j = 0; j < N_READINGS; j++)
            {
                reading += converter.lecture(PA3) >> 2;
            }
            reading = reading/N_READINGS;

            if(reading <= 50 && reading >= 40)
            {
                while(reading < 130)
                {
                    nav.moveForward(50);
                    reading = converter.lecture(PA3) >> 2;
                }
                nav.idle();
                positions[address++] = pos.x + 1 * pos.directionX;
                positions[address++] = pos.y + 1 * pos.directionY;
                pos.updatePosition(1);
                state = State::FOUND;
            }
            else
            {
                nav.moveForward(vitesse pour le faire avancer de 1 point en diagonal);
                _delay_ms(temps pour le faire avancer de 1 point en diagonal);
                nav.idle();
                if(reading <= 50 && reading >= 40)
                {
                    while(reading < 130)
                    {
                        nav.moveForward(50);
                        reading = converter.lecture(PA3) >> 2;
                    }
                    nav.idle();
                    positions[address++] = pos.x + 2 * pos.directionX;
                    positions[address++] = pos.y + 2 * pos.directionY;
                    pos.updatePosition(2);
                    state = State::FOUND;
                }
                else
                {
                    nav.moveBackward(vitesse pour le faire reculer de 1 point en diagonal);
                    _delay_ms(temps pour reculer de 1 en diagonal);
                    nav.idle();
                    timer.startTimer(temps pour le faire tourner de 45 degree);
                    while (!timer.isExpired && reading <= 15)
                    {
                        nav.rotateRight(vitesse pour le faire tourner de 45 degree);
                        reading = converter.lecture(PA3) >> 2;
                    }
                    pos.updateDirections();
                    state = State::ORTHOGONAL;
                }
            }
            break;
            case FOUND:
            nPoteaux++;
            sound.playNote(24, 300);
            _delay_ms(300);
            sound.playNote(24, 300);
            _delay_ms(300);
            sound.playNote(24, 300);
            _delay_ms(300);
            // faire clignoter ambre a 2Hz
            state = State::WAIT;
            break;
            case WAIT:
            while(!button.isPressed)
            {

            }
            state = State::ORTHOGONAL;
            i = 0;
            break;
        }
    }
    sound.playNote(60, 2000);
    // clignoter sa DEL en rouge à 2 Hz
    //finir
}