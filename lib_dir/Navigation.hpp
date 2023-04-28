/*
////////////////////////////////////////////////////////////////////////////////
/// \file    Navigation.hpp
/// \authors Jason Chen
///          Rafik Hachemi Boumila
            Jérôme Fréchette
            John Abou Nakoul
////////////////////////////////////////////////////////////////////////////////
Description: Mouvement du robot. Sert a faire bouger le robot de différentes manières: avancer, reculer,  tourner à gauche ou tourner à droite selon des configurations de roues definies dans les fichiers Wheel.hpp et Wheel.cpp. 

Identifications matérielles (Broches I/O): Utilisation de moteur-engrenage et de roues motrices. 
*/

#pragma once
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "definitions.hpp"
#include "Timer.hpp"
#include "Wheel.hpp"

const uint8_t BASE_TURN_SPEED = 200;

class Navigation
{
public:
    Navigation()
    {
       rightWheel = Wheel(&OCR0A, PB3, PB5);
       leftWheel = Wheel(&OCR0B, PB4, PB6);
       port_ = &PORTB;
    }
    Wheel leftWheel;
    Wheel rightWheel;
    void rotateRight(uint8_t speed);
    void rotateLeft(uint8_t speed);
    void moveForward(uint8_t speed);
    void moveBackward(uint8_t speed);
    void idle();
    
private:
    register_t port_;
};
/* Exemple de code: Le robot avance pendant 2 secondes, recule pendant 1 seconde, tourne vers 
 * la droite pendant une demi seconde et tourne vers la gauche pendant une demi seconde

Navigation nav(&PORTD, &DDRD, PD4, PD5);

nav.moveForward(100);
_delay_ms(2000);
nav.moveBackward(100);
_delay_ms(1000);
nav.rotateRight();
_delay_ms(500);
nav.rotateLeft();
_delay_ms(500);

*/