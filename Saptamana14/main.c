#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "serial.h"
#include "i2c.h"
#include "mpu6050.h"

volatile unsigned char wai;
volatile uint8_t flag = 0;

void writeI2CI(void)
{
	I2C0CONCLR = 0xFF;
	I2C0CONSET = 0x40;
	I2C0CONSET = 0x20;
}

__irq void irq_handler(void)
{
	//interrupt pentru who am i
	switch(I2C0STAT)
	{
		case 0x08:
		{
			I2C0DAT = 0xD0;
			I2C0CONCLR = 0x20;
			I2C0CONSET = 0x04;
			break;
		}
		case 0x10:
		{
			I2C0DAT = 0xD1;
			I2C0CONSET = 0x04;
			break;
		}
		case 0x18:
		{
			I2C0DAT = 0x75;
			break;
		}
		case 0x20:
		{
			I2C0CONSET = 0x14;
			break;
		}
		case 0x28:
		{
			flag = 1;
			I2C0CONSET = 0x14;
			break;
		}
		case 0x30:
		{
			I2C0CONSET = 0x14;
		}
		case 0x40:
		{
			I2C0CONSET = 0x04;
			break;
		}
		case 0x48:
		{
			I2C0CONSET = 0x10;
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
			wai = I2C0DAT;
			I2C0CONSET = 0x10;
			doneWAI = 1;
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
	for (i = 0; i < 12000; i++); //480
}

void WAI(void)
{
	writeI2CI();
}

uint8_t readWAI(void)
{
	unsigned char buffer;
	char adresa = 0x75;
	readRegister(adresa, &buffer);
	return buffer;
}

int main(void)
{
	doneFunction = 0;
	doneWAI = 0;
	initUART();
	
	// initializam I2C cu interrupturi
	initI2C();
	initInterrupts();
	WAI();
	while(1)
	{
		//citim wai
		if(doneWAI == 1)
		{
			print(wai);
			doneWAI = 0;
		}
	}
	return 0;
}