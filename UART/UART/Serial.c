/*
 * Serial.c
 *
 * Created: 30.05.2024 19:53:40
 *  Author: mihai
 */ 
#define F_CPU 14745600UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

void initUART(int BAUD)
{
	int UBRR, FOSC = 8000000;
	UBRR = (FOSC / (16 * BAUD)) - 1;
	UCSRC &= ~(1 << URSEL);
	UBRRH |= (UBRR >> 8);
	UBRRL |= UBRR & 0xFF;
	UCSRB |= (1 << RXEN) | (1 << TXEN);
	UCSRC |= (1 << URSEL);
	UCSRC |= (1 << UCSZ1) | (1 << UCSZ0);
}