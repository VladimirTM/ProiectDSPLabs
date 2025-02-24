#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int ok = 0;

void init_clock(void)
{
	unsigned int N = 0x7080;
	OCR1AH = (N >> 8) & 0xFF;
	OCR1AL = N & 0xFF;
	TCCR1A=0x00;
	TCCR1B=(1 << 3) | (1 << 2);
}

void toggle(void)
{
	if(ok)
	{
		PORTA |= (1<<0);
		ok = 0;
	}
	else
	{
		PORTA &= ~(1 << 0);
		ok = 1;
	}
}

void A1(void)
{
	if ((TIFR & (1 << 4))!= 0)
	{
		toggle();
		TIFR |= 1 << 4;
	}
}

void A2(void)
{
	sei();
	init_LED();
	init_clock();
	TIMSK |= 1 << OCIE1A;
	while (1)
	{
	}
	cli();
}

//ISR(TIMER1_COMPA_vect)
//{
//	toggle();
//}