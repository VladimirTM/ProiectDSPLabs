#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
#include "serial.h"

volatile int cntOperation = 0, cntWrite = 0, cntRead = 0, cntIndex = 0;
volatile int doneInit = 0;
uint8_t regDat[] = {0x6B, 0x00, 0x19, 0x07, 0x1A, 0x00, 0x1B, 0x00, 0x1C, 0x00, 0x23, 0x00, 0x24, 0x00, 0x37, 0x00, 0x38, 0x01, 0x67, 0x00, 0x68, 0x00, 0x6A, 0x00, 0x6C, 0x00, 0x74, 0x00};
uint8_t lenWrite = 13, lenRead = 13;
uint8_t measurements[14];
double results[7];

void resetI2C(void)
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
			cntOperation = 0;
			I2C0DAT = 0xD0;
			I2C0CONCLR = 0x20;
			I2C0CONSET = 0x04;
			break;
		}
		case 0x18: // enter data ACK received
		{
			cntOperation++;
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
			cntOperation++;
			I2C0DAT = valoare;
			if(cntOperation == 3)
			{
				I2C0CONSET = 0x20;
				if(cntWrite < lenWrite)
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
			if(cntRead == lenRead)
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
			cntIndex = 0;
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

void startSensor(void)
{
	resetI2C();
}
