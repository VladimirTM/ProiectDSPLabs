#define F_CPU 14745600UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>

void initADC(void)
{
	DDRA = 0;
	ADCSRA |= (1 << ADEN) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t readADC(void)
{
	uint8_t lsb, msb;
	uint16_t adc_value;
	ADMUX = 0;
	ADMUX |= (1 << REFS0) | (1 << ADLAR);
	ADCSRA |= (1 << ADSC);
	while(ADCSRA & (1 << ADSC));
	msb = ADCH;
	lsb = ADCL;
	adc_value = (lsb >> 6) | (uint16_t)(msb << 2);
	return adc_value;
}