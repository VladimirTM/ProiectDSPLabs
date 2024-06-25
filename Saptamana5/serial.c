#include <lpc214x.h>
#include <stdint.h>

void initUART(void)
{
	PINSEL0 = 0x5; 
	U0LCR = 0x83; // initializare tipul de variabila
	U0DLL = 78; // calcul divizor
	U0DLM = 0x00; // calcul divizor
	U0LCR = 0x03;
}

void sendCharacter(char ch)
{
	while((U0LSR & 0x20) == 0);
	U0THR = ch;
}

uint8_t getCharacter(void)
{
	while((U0LSR & 0x01) == 0);
	return U0RBR;
}