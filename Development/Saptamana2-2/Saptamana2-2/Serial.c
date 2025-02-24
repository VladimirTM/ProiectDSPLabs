#define F_CPU 14745600UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>

uint8_t flag;

void initUART(void)
{
	uint16_t ubrr = 0x7;
	DDRD = (1 << 1);
	UCSRC = 0;
	UBRRH = ubrr >> 8;
	UBRRL = ubrr;
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

void sendCharacter(char ch)
{
	while(!(UCSRA & (1 << UDRE)));
	UDR = ch;
}

void sendString(char *string)
{
	uint16_t len = strlen(string), i;
	for(i = 0; i < len; i++)
	{
		sendCharacter(string[i]);
	}
}

uint8_t UART_ReceiveByte(void)
{
	uint8_t ch;
	while(!(UCSRA & (1 << RXC)));
	ch = UDR;
	return ch;
}

ISR(USART_RXC_vect)
{
	flag = 1;
}