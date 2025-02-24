#include <lpc214x.h>
#include <stdint.h>
#include "timer.h"
#include "serial.h"

int ok = 0;

void delay(void)
{
	volatile unsigned int i = 0;
	for (i = 0; i < 12000000; i++);
}

void initBoardLED(void)
{
	PINSEL2 &= ~(1 << 3);
	IO1DIR |= (1 << 16);
}

void toggleLED(void)
{
	if(ok == 0)
	{
		IO1SET |= (1 << 16);
		ok = 1;
	}
	else
	{
		IO1CLR |= (1 << 16);
		ok = 0;
	}
}

int main(void)
{
	//initBoardLED();
	//initTimer();
	initUART();
	while(1)
	{
		//if((T0IR & (1 << 0)) == 1)
		//{
			//toggleLED();
			//T0IR |= (1 << 0);
		//}
		uint8_t ch = 'a';
		//ch = getCharacter();
		sendCharacter(ch);
		delay();
	}
	return 0;
}