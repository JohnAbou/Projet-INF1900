#define F_CPU 8000000
#include "definitions.hpp"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include "definitions.hpp"
#include "Wheel.hpp"
#include "Navigation.hpp"
#include "Led.hpp"
#include "Button.hpp"
#include "memoire_24.h"
#include "Communication.hpp"
#include "Sound.hpp"

void Sound::ajustementPwm ( uint8_t pwm)
{
   OCR2A = pwm;
   TCCR2A = (1 << WGM21)  | (1 << COM2A0);
   TCCR2B = (1 << CS21 ) | (1 << CS22);
}

void Sound::stopSound()
{
  TCCR2B &= ~((1<<CS21 | 1<<CS22));
}
uint16_t Sound::calculerFrequence(uint8_t note)
{
   return 440* pow(2, ((note - 69)/12));
}
uint16_t Sound::pwmMusic(uint8_t note){
   return (8000000/(256 * calculerFrequence(note))-1);
}

void Sound::playNote(uint8_t note, uint16_t duration)
{
   ajustementPwm(pwmMusic(note));
   for(uint16_t i = 0; i < duration; i++ )
   {
      _delay_ms(1);
   }
   
   stopSound();
}
void Sound::playNote(uint8_t note)
{
     ajustementPwm(pwmMusic(note));
}