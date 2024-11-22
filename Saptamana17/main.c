#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "serial.h"
#include "i2c.h"
#include "mpu6050.h"

#define FORCESTART 1

volatile uint8_t data;
volatile int cnt = 0, cntWrite = 0, cntRead = 0, flag = 0;
volatile int doneInit = 0;
uint8_t regDat[] = {0x6B, 0x00, 0x19, 0x07, 0x1A, 0x00, 0x1B, 0x00, 0x1C, 0x00, 0x23, 0x00, 0x24, 0x00, 0x37, 0x00, 0x38, 0x01, 0x67, 0x00, 0x68, 0x00, 0x6A, 0x00, 0x6C, 0x00, 0x74, 0x00};
uint8_t datPoz[] = {0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};
uint8_t lenRegDat = 13, lenDatPoz = 13;
//uint8_t measurements[14];
double result[7];

void startI2CI(void)
{
	I2C0CONCLR = 0x000000FF;
	I2C0CONSET = 0x00000040;
	I2C0CONSET = 0x00000020;
}

void pollingWrite(uint8_t adresa, uint8_t valoare)
{
	uint8_t stat;
	stat = I2C0STAT;
	switch(stat)
	{
		case 0x08: // start
		{
			cnt = 0;
			I2C0DAT = 0xD0;
			I2C0CONCLR = 0x20;
			I2C0CONSET = 0x04;
			break;
		}
		case 0x18: // enter data ACK received
		{
			cnt++;
			I2C0DAT = adresa;
			break;
		}
		case 0x20: // enter data NACK received
		{
			I2C0CONSET = 0x14;
			break;
		}
		case 0x28: // data sent with ACK
		{
			cnt++;
			I2C0DAT = valoare;
			if(cnt == 3)
			{
				I2C0CONSET = 0x20;
				if(cntWrite < lenRegDat)
				{
					cntWrite++;
				}
				else
				{
					doneInit = 1;
				}
			}		
			break;
		}
		case 0x30: // data sent with NACK
		{
			I2C0CONSET = 0x14;
			break;
		}
		case 0xF8: // no relevant information
		{
//			if(FORCESTART == 1)
//			{
//				startI2CI();
//			}	
			break;
		}
		default:
		{
			break;
		}
	}
	if (stat != 0xF8)
	{
		I2C0CONCLR = 0x08;
	}
}

void pollingRead(uint8_t adresa)
{
	uint8_t stat;
	stat = I2C0STAT;
	switch(stat)
	{
		case 0x08: // start
		{
			flag = 1;
			I2C0DAT = 0xD0;
			I2C0CONCLR = 0x20;
			I2C0CONSET = 0x04;
			break;
		}
		case 0x10: // repeated start
		{
			flag = 1;
			I2C0DAT = 0xD1;
			I2C0CONCLR = 0x20;
			I2C0CONSET = 0x04;
			break;
		}
		case 0x18: // enter data ACK received
		{
			I2C0DAT = adresa;
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
		case 0x40: // address received with ACK
		{
			if(cntRead == lenDatPoz)
			{
				I2C0CONCLR = 0x3C; // genereaza data + NACK
			}
			else
			{
				I2C0CONSET = 0x04; // genereaza data + ACK
			}
			break;
		}
		case 0x48: //  address received with NACK
		{
			I2C0CONSET = 0x20;
			break;
		}
		case 0x50: // data received with ACK
		{
			data = I2C0DAT;
			I2C0CONSET = 0x10;
			cntRead++;
			flag = 0;
			break; 
		}
		case 0x58: // data received with NACK
		{
			data = I2C0DAT;
			I2C0CONSET = 0x14;
			cntRead++;
			flag = 0;
			break;
		}
		case 0xF8: // no relevant information
		{
			if(flag == 0 && FORCESTART == 1)
			{
				startI2CI();
			}	
			break;
		}
		default:
		{
			break;
		}
	}
	if (stat != 0xF8)
	{
		I2C0CONCLR = 0x08;
	}
}

__irq void irq_handler(void)
{	
	if(doneInit == 0) // initializare
	{
		pollingWrite(regDat[2 * cntWrite], regDat[2 * cntWrite + 1]);
	}
	else // citire
	{
		if(cntRead < lenDatPoz)
		{
			pollingRead(datPoz[cntRead]);
		}
	}
	T0IR |= 1;
	VICVectAddr = 0x00000000;
}

void initInterrupts(void)
{
	VICIntEnable |= 1 << 4;
	VICIntSelect |= 1 << 4;
}

void initTimer(void)
{
	T0CTCR = 0x00;
	T0TCR = 0x02;
	T0MR0 = 100;
	T0MCR |= (3 << 0);
	T0TCR = 0x01;
}

void Sensor(void)
{
	startI2CI();
}

int main(void)
{
	doneWAI = 0;
	initUART();
	initI2C();
	initInterrupts();
	Sensor();
	initTimer();
	while(1)
	{
	}
	return 0;
}