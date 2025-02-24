#define F_CPU 14745600UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include "Serial.h"

void A6(void)
{
		char text[100];
		static int counter = 0;
		sprintf(text, "Ana are %d mere ", counter);
		counter++;
		sendString(text);
}

void A8(void)
{
	uint8_t byte_read;
	byte_read = UART_ReceiveByte();
	sendCharacter(byte_read);
}

int main(void)
{
	initUART();
	sei();
    while (1) 
    {
		if(flag)
		{
			A8();
			flag = 0;
		}
    }
	cli();
}

