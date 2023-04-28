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

int main(){
    Communication comm;
    Memoire24CXXX mem;
    comm.initUART();
    uint16_t byte;
    uint16_t j;
  
    for (j = 0; j < 16; j++)
    {
      mem.lecture(j, &byte);
      comm.transmissionUART(byte);
    }
}