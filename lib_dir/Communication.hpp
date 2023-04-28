/*
////////////////////////////////////////////////////////////////////////////////
/// \file    Communication.hpp
/// \authors Jason Chen
///          Rafik Hachemi Boumila
         Jérôme Fréchette
         John Abou Nakoul
////////////////////////////////////////////////////////////////////////////////
Description: Implémente deux fonctions pour configurer et transmettre des données avec une communication UART sur un microcontrôleur étant la fonction initUART() qui initialise la communication UART et la fonction transmissionUART(uint8_t donnee) qui transmet une donnée vers le UART en attendant que la transmission soit vide.

Identifications matérielles (Broches I/O): USART et port USB
*/

#pragma once
#define F_CPU 8000000
#include "definitions.hpp"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

class Communication
{
    public:
    Communication(){};
    void transmissionUART(uint8_t donnee);
    uint8_t readUart(void);


    void initUART();
    
};

/* Exemple de code qui affiche "Le robot en INF1900\n" 10 fois
Communication comm;
  char mots[21] = "Le robot en INF1900\n";
  _delay_ms(2000);

  uint8_t i, j;
  for (i = 0; i < 10; i++)
  {
    for (j = 0; j < 20; j++)
    {
      comm.transmissionUART(mots[j]);
    }
  }
*/