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

//void state_machine_1()
//{
//	static uint8_t state = 0;
//	switch (state)
//	{
//		case 0:
//		{
//			if ( ... )
//			{
//				state = 1;
//			}
//			else
//			{
//				state = 2;
//			}
//			break;
//		}
//		case 1:
//		{
//			break;
//		}
//	}
//}

int main(void)
{
	char measurements[14];
	int16_t ax, ay, az, t, gx, gy, gz;
	double AX, AY, AZ, T, GX, GY, GZ;
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
		
		printf("Acclerometru : %f %f %f\n", AX, AY, AZ);
		printf("Temperatura : %f\n", T);
		printf("Giroscop : %f %f %f\n\n", GX, GY, GZ);
		delay();
	}
	return 0;
}