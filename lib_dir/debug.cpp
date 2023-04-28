#include "debug.h"
#include <avr/io.h>


void initialisationUART(void)
{
    // 2400 bauds. Nous vous donnons la valeur des deux
    // premiers registres pour vous éviter des complications.
    UBRR0H = 0;
    UBRR0L = 0xCF;
    // permettre la réception et la transmission par le UART0
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
    // Format des trames: 8 bits, 1 stop bits, sans parité
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
}

// Du USART vers le PC

void transmissionUART(uint8_t donnee)
{
    while (!(UCSR0A & (1 << UDRE0)))
    ;
    UDR0 = donnee;
}

void debug(const char* message) {
    // Send each character in the message via UART
    for (int i = 0; message[i] != '\0'; i++) {
        transmissionUART(message[i]);
    }
}


