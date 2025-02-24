#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
#include "i2c.h"
#include "serial.h"

#define MPU_WRITE_ADDR 0xD0
#define MPU_READ_ADDR 0xD1

typedef enum
{
	NOT_INIT,
	INIT,
	READ_MEASUREMENTS,
	PROCESS_MEASUREMENTS,
	PRINT_MEASUREMENTS
}MPUSTATES;

MPUSTATES mainState = NOT_INIT;

int doneWrite = 0;
char measurements[14];
double results[7];
int doneWAI = 0;

// classical write in register
void writeRegister(char adresa, char valoare)
{
	switch(doneFunction)
	{
		case 0:
		{
			startI2C();
			break;
		}
		case 1:
		{
			writeI2C(MPU_WRITE_ADDR);
			break;
		}
		case 2:
		{
			writeI2C(adresa);
			break;
		}
		case 3:
		{
			writeI2C(valoare);
			break;
		}
		case 4:
		{
			stopI2C();
			doneWrite = doneWrite + 1;
			doneFunction = 0;
			break;
		}
	}
}

// citire clasica din registru
void Wreadregister(char adresa, unsigned char *valoare)
{
	WstartI2C();
	WwriteI2C(MPU_WRITE_ADDR);
	WwriteI2C(adresa);
	WstartI2C();
	WwriteI2C(MPU_READ_ADDR);
	WreadNACK(valoare);
	stopI2C();
}

void readRegister(char adresa, unsigned char *valoare)
{
	switch(doneFunction)
	{
		case 0:
		{
			startI2C();
			break;
		}
		case 1:
		{
			writeI2C(MPU_WRITE_ADDR);
			break;
		}
		case 2:
		{
			writeI2C(adresa);
			break;
		}
		case 3:
		{
			startI2C();
			break;
		}
		case 4:
		{
			writeI2C(MPU_READ_ADDR);
			break;
		}
		case 5:
		{
			readNACK(valoare);
			break;
		}
		case 6:
		{
			stopI2C();
			doneWAI = 1;
			break;
		}
	}
}

unsigned char whoAmI(void)
{
	unsigned char valoare;
	readRegister(0x75, &valoare);
	return valoare;
}

void initMPU6050(void)
{
	switch(doneWrite)
	{
		case 0:
		{
			writeRegister(0x6B, 0x00);
			break;
		}
		case 1:
		{
			writeRegister(0x19, 0x07);
			break;
		}
		case 2:
		{
			writeRegister(0x1A, 0x00);
			break;
		}
		case 3:
		{
			writeRegister(0x1B, 0x00);
			break;
		}
		case 4:
		{
			writeRegister(0x1C, 0x00);
			break;
		}
		case 5:
		{
			writeRegister(0x23, 0x00);
			break;
		}
		case 6:
		{
			writeRegister(0x24, 0x00);
			break;
		}
		case 7:
		{
			writeRegister(0x37, 0x00);
			break;
		}
		case 8:
		{
			writeRegister(0x38, 0x01);
			break;
		}
		case 9:
		{
			writeRegister(0x67, 0x00);
			break;
		}
		case 10:
		{
			writeRegister(0x68, 0x00);
			break;
		}
		case 11:
		{
			writeRegister(0x6A, 0x00);
			break;
		}
		case 12:
		{
			writeRegister(0x6C, 0x00);
			break;
		}
		case 13:
		{
			writeRegister(0x74, 0x00);
			break;
		}
	}
}

void readMeasurements(char *data)
{
	unsigned char measurement;
	int lastState;
	if(doneFunction < 13)
	{
		lastState = doneFunction;
		readACK(&measurement);
		if(lastState != doneFunction)
		{
			data[lastState] = measurement;
		}
	}
	else
	{
		lastState = doneFunction;
		readNACK(&measurement);
		if(lastState != doneFunction)
		{
			data[lastState] = measurement;
		}
	}
}

void processMeasurements(char *measurements, double *results)
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

void mainFSM()
{
	switch(mainState)
	{
		case NOT_INIT:
		{
			if(doneWrite < 14)
			{
				initMPU6050();
			}
			else
			{
				mainState = INIT;
				doneFunction = 0;
			}
			break;
		}
		case INIT:
		{
			switch(doneFunction)
			{
				case 0:
				{
					startI2C();
					break;
				}
				case 1:
				{
					writeI2C(0xD0);
					break;
				}
				case 2:
				{
					writeI2C(0x3B);
					break;
				}
				case 3:
				{
					startI2C();
					break;
				}
				case 4:
				{
					writeI2C(0xD1);
					break;
				}
				case 5:
				{
					mainState = READ_MEASUREMENTS;
					doneFunction = 0;
					break;
				}
			}
			break;
		}
		case READ_MEASUREMENTS:
		{
			if(doneFunction < 14)
			{
				readMeasurements(measurements);
			}
			else
			{
				stopI2C();
				mainState = PROCESS_MEASUREMENTS;
				doneFunction = 0;
			}
			break;
		}
		case PROCESS_MEASUREMENTS:
		{
			processMeasurements(measurements, results);
			mainState = PRINT_MEASUREMENTS;
			break;
		}
		case PRINT_MEASUREMENTS:
		{
			printResults(results);
			mainState = INIT;
			break;
		}
	}
}