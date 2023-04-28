/*
////////////////////////////////////////////////////////////////////////////////
/// \file    Led.hpp
/// \authors Jason Chen
///          Rafik Hachemi Boumila
         Jérôme Fréchette
         John Abou Nakoul
////////////////////////////////////////////////////////////////////////////////
Description: Permet d'éteindre la DEL et de l'allumer en rouge, ambre et vert. De plus, permet d'éteindre la DEL en faisant un fondu de lumière (fade).

Identifications matérielles (Broches I/O): La DEL est reliée à des broches choisis arbitrairement qui prendront le nom de pinGreen et pinRed. Le port sur lequel ces pins se trouve est configuré en sortie. 
*/

#pragma once
#define F_CPU 8000000
#include "definitions.hpp"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

const uint16_t US_PER_MS = 1000;

class Led
{
public:
    Led(register_t port, register_t mode, uint8_t pinGreen, uint8_t pinRed);
    void setColor(uint8_t color);
    void setGreen();
    void setRed();
    void setOff();
    void setAmber();
    void setAmber_ms(uint16_t duration_ms);
    void fade(const uint8_t colorStart, const uint8_t colorEnd);
    const uint8_t ledOff;
    const uint8_t ledGreen;
    const uint8_t ledRed;
    
private:
    register_t port_;
    register_t mode_;
    uint8_t intensity_;
    const uint8_t pinGreen_;
    const uint8_t pinRed_;
};

/* Exemple de code allumant une led rouge pendant 2 secondes, ambre pendant 2 secondes, s'éteint pendant 
 * une demi seconde, allume verte pendant 2 secondes et s'eteint tranquillement
  
    led.setRed();
    _delay_ms(2000);
    led.setAmber_ms(2000);
    led.setOff();
    _delay_ms(500);
    led.setGreen();
    _delay_ms(2000);
    led.fade(led.ledGreen, led.ledOff);
    
*/