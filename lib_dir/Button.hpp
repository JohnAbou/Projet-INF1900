/*
////////////////////////////////////////////////////////////////////////////////
/// \file    Button.hpp
/// \authors Jason Chen
///          Rafik Hachemi Boumila
         Jérôme Fréchette
         John Abou Nakoul
////////////////////////////////////////////////////////////////////////////////
Description: Définition de deux attributs étant isPressed et hasBeenPressed. L'attribut hasBeenPressed vaut true lorsque le bouton poussoir est appuyé (rising edge). L'attribut isPressed vaut true lorsque le bouton poussoir est appuyé et retourne à la valeur false lorsque le bouton est relâché (rising and falling edge).

Identifications matérielles (Broches I/O): Le bouton poussoir est localisé sur la broche D2 du port D. Cette broche sera configurée en entrée.
*/

#pragma once
#define F_CPU 8000000
#include "definitions.hpp"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

class Button
{
public:
    Button(register_t mode, uint8_t pin)
    {
      *mode &= ~(1 << pin);
    }
    volatile bool isPressed = false;
    volatile bool hasBeenPressed = false;
};
 /* Exemple de code mettant la led à vert si le boutton est enfoncé et à rouge lorsqu'il est relaché
 while(true)
 {
    if(button.isPressed)
    {
      led.setGreen();
    }
    else
    {
      led.setRed();
    }
}
*/

/* Exemple de code mettant la led à vert lorsque le bouton est appuyé et la led ne redevient pas rouge lorsque le bouton est relaché 
 * (tient seulement compte du rising edge)
 while(true)
 {
    if(button.hasBeenPressed)
    {
      led.setGreen();
    }
    else
    {
      led.setRed();
    }
}
*/