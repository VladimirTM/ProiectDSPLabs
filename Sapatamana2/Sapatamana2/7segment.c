#define F_CPU 14745600UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "timer.h"

static const uint8_t digitmap[10] = {
	0xC0,
	0xF9,
	0xA4,
	0xB0,
	0x99,
	0x92,
	0x82,
	0xF8,
	0x80,
	0x90
};

int contor = 0, flag = 1;

void init_digit(void)
{
	DDRA  |= 0xFF;
}

void display_digit(uint8_t digit)
{
	PORTA = digitmap[digit];
}

void clear_digit(void)
{
	PORTA = 0;
}

void changeNumber(void)
{
	display_digit(contor);
	contor++;
	if(contor==10)
	{
		contor=0;
	}
}

void A3(void)
{
	sei();
	init_clock();
	init_digit();
	TIMSK |= 1 << OCIE1A;
	while (1)
	{
	}
	clear_digit();
	cli();
}

void A4(void)
{
	sei();
	init_clock();
	init_digit();
	TIMSK |= 1 << OCIE1A;
	while (1)
	{
		if(flag)
		{
			changeNumber();
			flag = 0;
		}
	}
	clear_digit();
	cli();
}

ISR(TIMER1_COMPA_vect)
{
	//changeNumber();
	flag = 1;
}