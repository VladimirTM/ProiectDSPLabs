#include <lpc214x.h>
#include <stdint.h>
#include <string.h>

void initUART(void)
{
	PINSEL0 = 0x5; 
	U0LCR = 0x83; // initializare tipul de variabila
	U0DLL = 78; // calcul divizor
	U0DLM = 0x00; // calcul divizor
	U0LCR = 0x03;
}

void sendCharacter(unsigned char ch)
{
	while((U0LSR & 0x20) == 0);
	U0THR = ch;
}

void sendString(char *string)
{
	uint16_t len = strlen(string), i;
	for(i = 0; i < len; i++)
	{
		sendCharacter(string[i]);
	}
}

void printResults(double *results)
{
	char text[100];
	sprintf(text, "Acclerometru : %f %f %f\n", results[0], results[1], results[2]);
	sendString(text);
	sprintf(text, "Temperatura : %f\n", results[3]);
	sendString(text);
	sprintf(text, "Giroscop : %f %f %f\n\n", results[4], results[5], results[6]);
	sendString(text);
}

//void print(uint16_t value)
//{
//	char text[100];
//	sprintf(text, "Value 0x%x\n", value);
//	sendString(text);
//}

unsigned char getCharacter(void)
{
	unsigned char ch;
	while((U0LSR & 0x01) == 0);
	ch = U0RBR;
	return ch;
}