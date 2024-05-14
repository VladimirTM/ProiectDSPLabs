#define F_CPU 14745600UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

uint8_t enable = 0, counter = 0;

void toggle(int pozitieCurenta)
{
	if(enable == 1)
	{
		PORTB &= ~(1 << pozitieCurenta);
		enable = 0;
	}
	else
	{
		PORTB |= (1 << pozitieCurenta);
		enable = 1;
	}
}

int main(void)
{
	// instantiere butoane
	DDRB = 0;
	// instantiere LED
	DDRA = 0xFF;
	PORTA = 0x01;
    while (1) 
    {
		// buton 1
		if((PINB & (1 << 0)) == 0)
		{
			_delay_ms(80);
			if((PINB & (1 << 0)) == 0)
			{
				while((PINB & (1 << 0)) == 0);
				if(counter == 0)
				{
					counter = 7;
				}
				else
				{
					counter--;
				}
				PORTA = (1 << counter);
			}
		}
		// buton 2
		if((PINB & (1 << 1)) == 0)
		{
			_delay_ms(80);
			if((PINB & (1 << 1)) == 0)
			{
				while((PINB & (1 << 0)) == 0);
				if(counter == 7)
				{
					counter = 0;
				}
				else
				{
					counter++;
				}
				PORTA = (1 << counter);
			}
		}
    }
}
