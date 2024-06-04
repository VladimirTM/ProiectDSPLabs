#define F_CPU 14745600UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include "Adc.h"
#include "Serial.h"


int main(void)
{
	initUART();
	initADC();
    while (1) 
    {
		uint32_t realValue;
		uint16_t readValue;
		char text[100];
		readValue = readADC();
		realValue = (readValue * 5000) / 1023;
		sprintf(text, "%d mV\n", realValue);
		sendString(text);
		_delay_ms(1000);
    }
}

