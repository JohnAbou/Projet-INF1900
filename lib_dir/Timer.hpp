/*
////////////////////////////////////////////////////////////////////////////////
/// \file    Timer.hpp
/// \authors Jason Chen
///          Rafik Hachemi Boumila
         Jérôme Fréchette
         John Abou Nakoul
////////////////////////////////////////////////////////////////////////////////
Description: Sert de minuterie pour le robot. Retourne un nombre de cycles d'horloge correspondant à une durée en ms fournie et peut servir à faire avancer le robot pendant un certain temps, allumer une DEL pendant un certain temps, etc., et ce, sans avoir recours à des délais.


Identifications matérielles (Broches I/O): Pas de matériel externe utilisé. Les registres utilisés sont: TCNT1, OCR1A, TCCR1A, TCCR1B et TIMSK1
*/

#pragma once
#define F_CPU 8000000
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

class Timer
{
public:
    Timer(){}
    void startTimer_ms(uint16_t duration_ms);
    void stopTimer();
    volatile bool isExpired = false;

private:
    uint16_t convertToCycles(const uint16_t ms);
};

/* Exemple de code allumant une led en rouge pendant 2 secondes et ensuite en vert pendant 5 secondes
    
    Timer timer;

    timer.startTimer_ms(2000);
    while (!timer.isExpired)
    {
      led.setRed();
    }
    timer.startTimer_ms(5000);
    while (!timer.isExpired)
    {
      led.setGreen();
    }
    led.setOff();

*/