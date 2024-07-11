#include <lpc214x.h>
#include <stdint.h>
#include <string.h>

void initI2C(void)
{
	PINSEL0 |= 0x50;
	I2C0CONSET = 0x40;
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

void writeI2C(uint8_t data)
{
	I2C0DAT = data;
	I2C0CONSET = 0x40;
	while((I2C0CONSET & 0x08) == 0);
	I2C0CONCLR = 0x08;
}

uint8_t readACK(void)
{
	I2C0CONSET = 0x44;
	while( (I2C0CONSET & 0x08) == 0 );
	I2C0CONCLR = 0x0C;
	return I2C0DAT;
}

uint8_t readNACK(void)
{
	I2C0CONSET = 0x40;
	while( (I2C0CONSET & 0x08) == 0 );
	I2C0CONCLR = 0x08;
	return I2C0DAT;
}
