#define F_CPU 14745600UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "Serial.h"


int main(void)
{
	DDRA = 0xFF;
	PORTA = 0x01;
	initUART();
    while (1) 
    {
		sendCharacter('a');
		_delay_ms(1000);
    }
}

