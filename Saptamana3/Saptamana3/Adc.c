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
	ADMUX |= (1 << REFS0) | (1 << MUX0);
}

uint16_t readADC(void)
{
	uint8_t lsb, msb;
	uint16_t adc_value;
	
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADSC)));
	lsb = ADCL;
	msb = ADCH;
	adc_value = msb;
	adc_value <<= 8;
	adc_value |= lsb;
	return adc_value;
}