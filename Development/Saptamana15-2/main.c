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

void polling(void)
{
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
		case 0x18: // enter data ACK received
		{
			I2C0DAT = 0x75;
			break;
		}
		case 0x20: // enter data NACK received
		{
			I2C0CONSET = 0x14;
			break;
		}
		case 0x28: // data sent with ACK
		{
			I2C0CONSET = 0x20;
			break;
		}
		case 0x30: // data sent with NACK
		{
			I2C0CONSET = 0x14;
			break;
		}
//		case 0x38: // arbitration lost
//		{
//			I2C0CONSET = 0x24;
//			break;
//		}
		case 0x40: // address received with ACK
		{
			//I2C0CONSET = 0x04; // genereaza data + ACK
			I2C0CONCLR = 0x3C; // genereaza data + NACK
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
			I2C0CONSET = 0x14;
			donewai = 1;
			break;
		}
		default:
		{
			while(1);
		}
	}
	I2C0CONCLR = 0x08;
}

void blink(void)
{
	static int state = 0;
	if(state == 0)
	{
		IO1SET |= (1 << 16);
		state = 1;
	}
	else
	{
		IO1CLR |= (1 << 16);
		state = 0;
	}
}


__irq void irq_handler(void)
{	
	polling();
	blink();
	T0IR |= 1;
	VICVectAddr = 0x00000000;
}

void initInterrupts(void)
{
	VICIntEnable |= 1 << 4;
	VICIntSelect |= 1 << 4;
//	VICIntEnable |= 1 << 9;
//	VICIntSelect |= 1 << 9;
}

void initTimer(void)
{
	// for blink
	PINSEL2 &= ~(1 << 3);
	IO1DIR |= (1 << 16);
	// for timer
	T0CTCR = 0x00;
	T0TCR = 0x02;
	T0MR0 = 715;
	T0MCR |= (3 << 0);
	T0TCR = 0x01;
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
	doneWAI = 0;
	initUART();
	initI2C();
	initInterrupts();
	WAI();
	initTimer();
	while(1)
	{
		//citim wai
		if(donewai == 1)
		{
			//print(wai);
			doneWAI = 0;
		}
	}
	return 0;
}