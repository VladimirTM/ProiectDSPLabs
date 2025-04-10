#include <lpc214x.h>
#include <stdint.h>
#include <string.h>

void initI2C(void)
{
	PINSEL0 |= 0x00000050;
	I2C0CONSET = 0x40;
	I2C0SCLH = 0x28;
	I2C0SCLL = 0x28;	
}

void startI2C(void)
{
	I2C0CONSET = 0x20;
	while((I2C0CONSET & 0x08) == 0);
	I2C0CONCLR = 0x28;
}

void stopI2C(void)
{
	I2C0CONSET = 0x50;
}

void writeI2C(char data)
{
	I2C0DAT = data;
	I2C0CONSET = 0x40;
	while((I2C0CONSET & 0x08) == 0);
	I2C0CONCLR = 0x08;
}

unsigned char readACK(void)
{
	I2C0CONSET = 0x44;
	while( (I2C0CONSET & 0x08) == 0 );
	I2C0CONCLR = 0x0C;
	return I2C0DAT;
}

unsigned char readNACK(void)
{
	I2C0CONSET = 0x40;
	while( (I2C0CONSET & 0x08) == 0 );
	I2C0CONCLR = 0x08;
	return I2C0DAT;
}
