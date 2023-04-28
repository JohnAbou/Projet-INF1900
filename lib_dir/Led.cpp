#include "Led.hpp"

const uint8_t MS_PER_CYCLE = 20;

Led::Led(register_t port, register_t mode, uint8_t pinGreen, uint8_t pinRed):
    ledOff(~((1 << pinGreen) | (1 << pinRed))),
    ledGreen(1 << pinGreen),
    ledRed(1 << pinRed),
    port_(port),
    mode_(mode),
    pinGreen_(pinGreen),
    pinRed_(pinRed)
{
    *mode |= ((1<<pinGreen) | (1<<pinRed));
}

void Led::setColor(uint8_t color)
{
    if(color == 1){
        setGreen();
    }
    else if(color == 2){
        setRed();
    }
}

void Led::setGreen()
{
    setOff();
    *port_ |= ledGreen;
}

void Led::setRed()
{
    setOff();
    *port_ |= ledRed;
}

void Led::setOff()
{
    *port_ &= ledOff;
}

void Led::setAmber()
{
    setRed();
    _delay_ms(10);
    setGreen();
    _delay_ms(10);
}

void Led::setAmber_ms(uint16_t duration_ms)
{
    uint16_t nCycles = duration_ms/MS_PER_CYCLE;
    for(uint16_t i = 0; i < nCycles; i++)
    {
        setRed();
        _delay_ms(10);
        setGreen();
        _delay_ms(10);
    }
}

void Led::fade(const uint8_t colorStart, const uint8_t colorEnd)
{
    uint8_t b = 100;
    
    for(uint8_t a = 100; a > 0; a--)
    {
        for(uint16_t k = 0; k < 100; k++)
        {
            setOff();
            *port_ |= colorStart;
             for(uint8_t i = 0; i < a; i++)
             {
                 _delay_us(1);
             }
    
            setOff();
            *port_ |= colorEnd;
            for(uint8_t i = 0; i < (b-a); i++) 
            {
                _delay_us(1);
            }
        }
    }
  }

