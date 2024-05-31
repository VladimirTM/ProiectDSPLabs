#define F_CPU 14745600UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

void initUART(void)
{
	uint16_t ubrr = 0x5F;
	DDRD = (1 << 1);
	UCSRC = 0;
	UBRRH = ubrr >> 8;
	UBRRL = ubrr;
	UCSRB = (1 << RXEN) | (1 << TXEN);
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

void sendCharacter(char ch)
{
	while(!(UCSRA & (1 << UDRE)));
	UDR = ch;
}