#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "serial.h"

#define FORCESTART 0

volatile uint8_t data;
volatile int cnt = 0, cntWrite = 0, cntRead = 0, flag = 0, cntIndex = 0;
volatile int doneInit = 0;
uint8_t regDat[] = {0x6B, 0x00, 0x19, 0x07, 0x1A, 0x00, 0x1B, 0x00, 0x1C, 0x00, 0x23, 0x00, 0x24, 0x00, 0x37, 0x00, 0x38, 0x01, 0x67, 0x00, 0x68, 0x00, 0x6A, 0x00, 0x6C, 0x00, 0x74, 0x00};
uint8_t lenRegDat = 13, lenDatPoz = 13;
uint8_t measurements[14];
double result[7];
	
void initI2C(void)
{
	PINSEL0 |= 0x00000050;
	I2C0CONSET = 0x40;
	I2C0SCLH = 0x28;
	I2C0SCLL = 0x28;
}	

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
			I2C0CONSET = 0x04; // genereaza data + ACK
			break;
		}
		case 0x48: //  address received with NACK
		{
			I2C0CONSET = 0x20;
			break;
		}
		case 0x50: // data received with ACK
		{
			measurements[cntIndex++] = I2C0DAT;
			cntRead++;
			if(cntRead == lenDatPoz)
			{
				I2C0CONCLR = 0x3C; // genereaza data + NACK
			}
			break; 
		}
		case 0x58: // data received with NACK
		{
			measurements[cntIndex++] = I2C0DAT;
			I2C0CONSET = 0x14;
			cntRead++;
			flag = 0;
			cntIndex = 0;
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

void processMeasurements(uint8_t *measurements, double *results)
{
	int16_t ax, ay, az, t, gx, gy, gz;
	double AX, AY, AZ, T, GX, GY, GZ;
	ax = ((int16_t)(measurements[0]) << 8) | ((int16_t)(measurements[1]));
	AX = (double)ax / 16384.0;
	ay = ((int16_t)(measurements[2]) << 8) | ((int16_t)(measurements[3]));
	AY = (double)ay / 16384.0;
	az = ((int16_t)(measurements[4]) << 8) | ((int16_t)(measurements[5]));
	AZ = (double)az / 16384.0;
	
	t = ((int16_t)(measurements[6]) << 8) | ((int16_t)(measurements[7]));
	T = ((double)t / 340.0) + 36.53;
		
	gx = ((int16_t)(measurements[8]) << 8) | ((int16_t)(measurements[9]));
	GX = (double)gx / 131.0;
	gy = ((int16_t)(measurements[10]) << 8) | ((int16_t)(measurements[11]));
	GY = (double)gy / 131.0;
	gz = ((int16_t)(measurements[12]) << 8) | ((int16_t)(measurements[13]));
	GZ = (double)gz / 131.0;
	
	results[0] = AX;
	results[1] = AY;
	results[2] = AZ;
	results[3] = T;
	results[4] = GX;
	results[5] = GY;
	results[6] = GZ;
}

void Sensor(void)
{
	startI2CI();
}

__irq void irq_handler(void)
{	
	if(doneInit == 0) // initializare
	{
		pollingWrite(regDat[2 * cntWrite], regDat[2 * cntWrite + 1]);
	}
	else // citire
	{
		if(cntRead <= lenDatPoz)
		{
			pollingRead(0x3B);
		}
		else {
			processMeasurements(measurements, result);
			printResults(result);
			cntRead = 0;
			Sensor();
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

/*
- refactorizare
- o functie de init de structuri de date+ i2c ( nu face nici o tranzactie)
- taskuri
   - automat de stari general - NOT_INIT, CONFIGURING, IDLE
		- 2 moduri de functionare - (1) burst - citeste continuu + salvare date in memorie in ceva structura
														o functie de genul get_data(DATA *data)
																(2) on demand - 
																			o fcuntie de cerere() - la primul apel (request) - return OK_LATER - urmat. apeluri (in caz ca nu e gata inca) - am date
   - functie de configurare burst/on demand
   - fucntie de trigger de reinit
 
*/

int main(void)
{
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