#include "Interpretor.hpp"
#include "Led.hpp"
#include "Communication.hpp"

int main()
{
    Led led(&PORTA, &DDRA, PA1, PA0);

    led.setColor(1);
    _delay_ms(250);
    led.setOff();
    _delay_ms(250);
    led.setColor(2);
    _delay_ms(2500);
    led.setOff();

    Interpretor interpretor;
    interpretor.readData();

    return 0;
}