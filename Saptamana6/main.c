#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
#include "timer.h"
#include "serial.h"
#include "adc.h"

void delay(void)
{
	volatile unsigned int i = 0;
	for (i = 0; i < 1200000; i++);
}

int main(void)
{
	//unsigned char ch;
	//initBoardLED();
	//initTimer();
	initUART();
	initADC();
	while(1)
	{
		//ch = getCharacter();
		//sendCharacter(ch);
		uint32_t value;
		uint32_t readValue;
		char text[100];
		readValue = readADC();
		value = readValue * 3300 / 1023;
		sprintf(text, "ADCvalue %d\n", value);
		sendString(text);
		delay();
	}
	return 0;
}