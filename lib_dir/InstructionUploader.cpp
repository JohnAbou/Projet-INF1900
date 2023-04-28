#include "InstructionUploader.hpp"
#include "Communication.hpp"
#include "memoire_24.h"

//faire un dossier txt avec instructions
//transformer en fichier binaire avec progmem

void InstructionUploader::uploadInstructions()
{
    Led led(&PORTA, &DDRA, PA1, PA0);
    Communication instructions;
    instructions.initUART();
    Memoire24CXXX memory;
    uint8_t byte = 0;
    uint16_t size = 0;
    
    byte = instructions.readUart();
    
    memory.ecriture(address_, byte);
    
    _delay_ms(5);
    
    address_++;
    size = (byte << 8);
    byte = instructions.readUart();

    memory.ecriture(address_, byte);
    
    _delay_ms(5);
    address_++;
    size += byte;

    while (address_ < size)
    {
        byte = instructions.readUart();
        memory.ecriture(address_++, byte);
        _delay_ms(5);
    }
    

}