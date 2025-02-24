#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "serial.h"
#include "i2c.h"
#include "mpu6050.h"

volatile uint8_t wai, donewai;

void startI2CI(void)
{
	I2C0CONCLR = 0x000000FF;
	I2C0CONSET = 0x00000040;
	I2C0CONSET = 0x00000020;
}

__irq void irq_handler(void)
{
	//interrupt pentru who am i
	switch(I2C0STAT)
	{
		case 0x08: // start
		{
			I2C0DAT = 0xD0;
			I2C0CONCLR = 0x20;
			I2C0CONSET = 0x04;
			break;
		}
		case 0x10: // repeated start
		{
			I2C0DAT = 0xD1;
			I2C0CONCLR = 0x20;
			I2C0CONSET = 0x04;
			break;
		}
		case 0x18: // enter data
		{
			I2C0DAT = 0x75;
			break;
		}
		case 0x20: // NACK received
		{
			I2C0DAT = 0x75;
			//I2C0CONSET = 0x14;
			break;
		}
		case 0x28: // data sent with ACK
		{
			I2C0CONSET = 0x20;
			//I2C0CONSET = 0x10;
			break;
		}
		case 0x30: // data sent with NACK
		{
			I2C0CONSET = 0x20;
			//I2C0CONSET = 0x14;
			break;
		}
//		case 0x38: // arbitration lost
//		{
//			I2C0CONSET = 0x24;
//			break;
//		}
		case 0x40: // address received with ACK
		{
			I2C0CONSET = 0x04;
			break;
		}
		case 0x48: //  address received with NACK
		{
			I2C0CONSET = 0x20;
			break;
		}
		case 0x50: // data received with ACK
		{
			wai = I2C0DAT;
			I2C0CONSET = 0x10;
			donewai = 1;
			break; 
		}
		case 0x58: // data received with NACK
		{
			wai = I2C0DAT;
			I2C0CONSET = 0x10;
			donewai = 1;
			break;
		}
		default:
		{
			while(1);
		}
	}
	I2C0CONCLR = 0x08;
	VICVectAddr = 0x00000000;
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
	startI2CI();
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
	
	// init I2C with interrupts
	initI2C();
	initInterrupts();
	WAI();
	while(1)
	{
		//read wai (one and display)
		if(donewai == 1)
		{
			print(wai);
			doneWAI = 0;
		}
	}
	return 0;
}