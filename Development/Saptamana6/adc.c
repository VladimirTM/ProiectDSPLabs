#include <lpc214x.h>
#include <stdint.h>
#include <string.h>

void initADC(void)
{
	PINSEL1 |= (1 << 24);
	AD0CR = 0x00200402; // AD0.1
}

uint16_t readADC(void)
{
	uint16_t result;
	AD0CR |= (1 << 24); // start ADC
	while((AD0DR1 & (1 << 31)) == 0);
	result = AD0DR1;
	result = result >> 6;
	result = result & 0x3FF;
	return result;
}