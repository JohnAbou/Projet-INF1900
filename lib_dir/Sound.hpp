#pragma once
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "definitions.hpp"

class Sound
{
    public:
    Sound()
    {
        DDRD |= ((1<<PD6)| (1<<PD7));
    }
    void stopSound();
    void playNote(uint8_t note, uint16_t duration);
    void playNote(uint8_t note);

    private:
    uint16_t pwmMusic(uint8_t note);
    uint16_t calculerFrequence(uint8_t note);
    void ajustementPwm ( uint8_t pwm);
};