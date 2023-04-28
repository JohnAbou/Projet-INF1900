#include "Timer.hpp"

// Convertit les millisecondes en cycles d'horloge
uint16_t Timer::convertToCycles(const uint16_t ms)
{
  return (ms*7.8);
}

// Méthode démarrant une minuterie en millisecondes, une interruption est levée lorsque la minuterie atteint 0
void Timer::startTimer_ms(const uint16_t duration_ms)
{
  const uint16_t duration_cycles = convertToCycles(duration_ms);
  isExpired = false;
  TCNT1 = 0 ;
  OCR1A = duration_cycles;
  TCCR1A = (1<< COM1A1) | (1<<WGM12);
  TCCR1B = (1<<CS12) | (1<<CS10);
  TCCR1C = 0;
  TIMSK1 = (1<<ACIS1) | (1<<OCIE1A);
}

// Méthode permettant d'arrêter la minuterie par force
void Timer::stopTimer()
{
  isExpired = true;
  TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12));
}
