#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can.h"
#include "can.cpp"
#include "Led.hpp"
#include "Point.h"
#include "Communication.hpp"
#include "memoire_24.h"
#include "Navigation.hpp"
#include "Timer.hpp"
#include "Sound.hpp"

int main()
{
    Led led(&PORTA, &DDRA, PA1, PA0);
    //State state = State::ORTHOGONAL;
    Navigation nav;

    Communication comm;
    Memoire24CXXX mem;

    uint16_t reading = 0;

    can converter = can();
    DDRA &= ~(1 << PA3);
    comm.initUART();
    while(true)
    {
        reading = converter.lecture(PA3) >> 2;
        _delay_ms(5);
        comm.transmissionUART(reading);
    }
    return 0;
}