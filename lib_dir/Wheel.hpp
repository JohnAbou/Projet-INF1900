/*
////////////////////////////////////////////////////////////////////////////////
/// \file    Wheel.hpp
/// \authors Jason Chen
///          Rafik Hachemi Boumila
            Jérôme Fréchette
            John Abou Nakoul
////////////////////////////////////////////////////////////////////////////////
Description: Mouvements des roues. Fonctions differentes qui servent a avancer les roues de sortes que le robot tourne sur place vers la gauche ou vers la droite seulement. Il peut egalement avancer ou reculer.  

Identifications matérielles (Broches I/O): Utilisation de moteur-engrenage et de roues motrices. 
*/

#pragma once
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "definitions.hpp"
#include "Timer.hpp"

class Wheel
{
public:
    Wheel(){}
    Wheel(register_t side, uint8_t pin, uint8_t pinDir);
    void moveForward(uint8_t speed);
    void moveBackward(uint8_t speed);
    void idle();
    
    
private:
    void ajustementPwm(uint8_t pourcentage);
    void setDirBackward();
    void setDirForward();
    uint8_t pinDir_;
    register_t side_;
    register_t mode_;
    register_t port_;
};

/* Exemple de code faisant avancer une roue pendant 1 seconde, ensuite, elle
 * arrête de bouger pendant 1 seconde et finalement, elle recule pendant 1 seconde.
  
  Wheel wheel;
  wheel.moveForward(150);
  _delay_ms(1000);
  wheel.idle();
  _delay_ms(1000);
  wheel.moveBackward(150);
  _delay_ms(1000);
    
*/