#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "serial.h"
#include "i2c.h"
#include "mpu6050.h"

volatile uint8_t status = 0;
volatile unsigned char wai;

__irq void irq_handler(void)
{
	switch(I2C0STAT)
	{
		case 0x08:
		{
			I2C0CONCLR = 0x20;
			if(status == 0)
			{
				I2C0DAT = 0xD0;
			}
			else
			{
				I2C0DAT = 0xD1;
			}
			break;
		}
		case 0x18:
		{
			I2C0DAT = 0x75;
			status = status + 1;
			break;
		}
		case 0x20:
		{
			if(status == 0)
			{
				I2C0DAT = 0xD0;
			}
			else
			{
				I2C0DAT = 0xD1;
			}
			break;
		}
		case 0x28:
		{
			I2C0CONSET = 0x10;
			break;
		}
		case 0x40:
		{
			I2C0CONSET = 0x04;
			break;
		}
		case 0x48:
		{
			I2C0CONSET = 0x20;
			break;
		}
		case 0x50:
		{
			wai = I2C0DAT;
			I2C0CONSET = 0x10;
			doneWAI = 1;
			break;
		}
		case 0x58:
		{
			I2C0CONSET = 0x20;
			break;
		}
	}
	I2C0CONCLR = 0x08;
	VICVectAddr = 0x00;
}

void initInterrupts(void)
{
	VICIntEnable |= 1 << 9;
	VICIntSelect |= 1 << 9;
}

void delay(void)
{
	volatile unsigned int i = 0;
	for (i = 0; i < 120; i++);
}

int main(void)
{
	doneFunction = 0;
	doneWAI = 0;
	initUART();
	initI2C();
	initInterrupts();
	while(1)
	{
		//mainFSM();
		if(doneWAI == 1)
		{
			print(wai);
			while(1);
			break;
		}
	}
	return 0;
}