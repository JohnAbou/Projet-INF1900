#pragma once
#define F_CPU 8000000
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "Led.hpp"

class InstructionUploader
{
    public:
    InstructionUploader(){}
    void uploadInstructions();

    private:
    uint16_t address_ = 0x00;

};