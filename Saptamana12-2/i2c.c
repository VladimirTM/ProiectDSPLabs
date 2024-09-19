#include <lpc214x.h>
#include <stdint.h>
#include <string.h>

volatile uint8_t buffer[10];
volatile uint8_t indexB = 0;
volatile uint8_t length = 0;
volatile uint8_t done = 0;
volatile uint8_t dataB[14];
volatile uint8_t indexD = 0;

void initInterrupts(void)
{
	VICIntEnable |= 1 << 9;
	VICIntSelect |= 1 << 9;
}

void initI2C(void)
{
	PINSEL0 |= 0x00000050;
	I2C0CONCLR = 0x6C;
	I2C0CONSET = 0x40;
	I2C0SCLH = 0x28;
	I2C0SCLL = 0x28;	
}

void startI2C(void)
{
	I2C0CONSET = 1 << 5;
}

void stopI2C(void)
{
	I2C0CONSET = 1 << 4;
	I2C0CONCLR = 1 << 3;
}

void writeI2C(uint8_t adresa, uint8_t *data, uint8_t len)
{
	buffer[0] = adresa;
	for(int i = 0; i < len; i++)
	{
		buffer[i + 1] = data[i];
	}
	length = length + 1;
	indexB = 0;
	done = 0;
	startI2C();
}

__irq void myirq(void)
{
	switch(I2C0STAT)
	{
		case 0x08:
		case 0x10:
		{
			I2C0DAT = buffer[indexB++];
			I2C0CONCLR = 1 << 5;
			break;
		}
		case 0x18:
		case 0x28:
		{
			if(indexB < length)
			{
				I2C0DAT = buffer[indexB++];
			}
			else
			{
				stopI2C();
				done = 1;
			}
			break;
		}
		case 0x20:
		case 0x30:
		case 0x38:
		{
			stopI2C();
			done = 1;
			break;
		}
		case 0x50:
		{
			dataB[indexD++] = I2C0DAT;
			break;
		}
		case 0x58:
		{
			dataB[indexD] = I2C0DAT;
			stopI2C();
			break;
		}
		default:
		{
			I2C0CONCLR = 1 << 3;
			break;
		}
	}
	I2C0CONCLR = 0x08;
	VICVectAddr = 0x00;
}
