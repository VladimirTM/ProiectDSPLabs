#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "serial.h"
#include "i2c.h"
#include "mpu6050.h"

void delay(void)
{
	volatile unsigned int i = 0;
	for (i = 0; i < 1200000; i++);
}

void readMeasurements(char *data)
{
	int i;
	for(i = 0; i < 13; i++)
	{
		data[i] = readACK();
	}
	data[i] = readNACK();
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

int main(void)
{
	char measurements[14];
	double results[7];
	initUART();
	initI2C();
	delay();
	initMPU6050();
	while(1)
	{
		startI2C();
		writeI2C(0xD0);
		writeI2C(0x3B);
		startI2C();
		writeI2C(0xD1);
		readMeasurements(measurements);
		stopI2C();
		processMeasurements(measurements, results);
		printResults(results);
		//printf("Acclerometru : %f %f %f\n", AX, AY, AZ);
		//printf("Temperatura : %f\n", T);
		//printf("Giroscop : %f %f %f\n\n", GX, GY, GZ);
		delay();
	}
	return 0;
}