#include <lpc214x.h>
#include <stdint.h>

int ok = 0, contor = 0;

static const uint32_t digitmap[10] = {
	0x000000C0,
	0x000000F9,
	0x000000A4,
	0x000000B0,
	0x00000099,
	0x00000092,
	0x00000082,
	0x000000F8,
	0x00000080,
	0x00000090
};

void delay(void)
{
	volatile unsigned int i = 0;
	for (i = 0; i < 1474560; i++);
}

void initHex(void)
{
	PINSEL0 = 0x00000000; // Pus ca GPIO
	IO0DIR = 0xFFFFFFFF; // Pus ca output
}

void displayDigit(int digit)
{
	IO0SET = digitmap[digit];
}

void clearDigit(int digit)
{
	IO0CLR = 0xFFFFFFFF;
}

void changeNumber(void)
{
	displayDigit(contor);
	contor++;
	if(contor==10)
	{
		contor=0;
	}
	delay();
	clearDigit(contor);
}

void initPins(void)
{
	PINSEL0 &= ~(1 << 0) & ~(1 << 1); // P0.0 for GPIO
	PINSEL0 &= ~(1 << 4) & ~(1 << 5); // P0.1 for GPIO
	IO0DIR |= (1 << 0); // Set P0.0 for output
	IO0DIR &= ~(1 << 2); // Reset P0.1 for input
}

void toggle(void)
{
	if(ok == 1)
	{
		IO0SET |= (1 << 0);
		ok = 0;
		delay();
	}
	else
	{
		IO0CLR |= (1 << 0);
		ok = 1;
		delay();
	}
}

void blink(void)
{
	if((IO0PIN & (1 << 2)) == 0)
	{
		toggle();
	}
}

int main(void)
{
	//initPins();
	initHex();
	while(1)
	{
		//blink();
		changeNumber();
	}
	return 0;
}