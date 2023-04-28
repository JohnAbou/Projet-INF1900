#include "Wheel.hpp"

Wheel::Wheel(register_t side, uint8_t pin, uint8_t pinDir)
{
   DDRB |= ((1<<pin)| (1<<pinDir));
   mode_ = &DDRB;
   pinDir_ = pinDir;
   side_ = side;
   port_ = &PORTB;
}

void Wheel::ajustementPwm ( uint8_t pwm)
{
   *side_ = pwm;
   TCCR0A = (1 << WGM00) | (1 << COM0A1) | (1 << COM0B1);
   TCCR0B = (1 << CS01);
}

void Wheel::moveForward(uint8_t speed)
{
   setDirForward();
   ajustementPwm(speed);
}

void Wheel::moveBackward(uint8_t speed)
{
   setDirBackward();
   ajustementPwm(speed);
}

void Wheel::setDirBackward()
{
   *port_ |= (1 << pinDir_);
}

void Wheel::setDirForward()
{
   *port_ &= ~(1 << pinDir_);
}

void Wheel::idle()
{
   *side_ = 0;
}