#pragma once
#define F_CPU 8000000
#include "definitions.hpp"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include "memoire_24.h"
#include "Navigation.hpp"
#include "Led.hpp"
#include "Sound.hpp"

const uint8_t
    DBT = 0x01,
    ATT = 0x02,
    DAL = 0x44,
    DET = 0x45,
    SGO = 0x48,
    SAR = 0x09,
    MAR = 0x60,
    MAR2 = 0x61,
    MAV = 0x62,
    MRE = 0x63,
    TRD = 0x64,
    TRG = 0x65,
    DBC = 0xC0,
    FBC = 0xC1,
    FIN = 0xFF;

class Interpretor
{
public:
    Interpretor()
    {
        
    }
    void readData();

private:
    void checkNext();
    void executeCase();
    uint8_t operation_ = 0;
    uint8_t operand_ = 0;
    uint8_t address_ = 0x00;
    uint8_t loopCounter_ = 0;
    uint8_t loopIncrements_ = 0;
    uint8_t loopAddress_ = 0x00;
    Memoire24CXXX memory_;
    bool started_ = false;
    bool loopEnd_ = false;
    Led led_ = Led(&PORTA, &DDRA, PA1, PA0);
    Navigation nav_;
    Sound sound_;
};