#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
#include "i2c.h"
#include "serial.h"

#define MPU_WRITE_ADDR 0xD0
#define MPU_READ_ADDR 0xD1

void writeRegister(uint8_t adresa, uint8_t data)
{
	uint8_t buffer[2] = {adresa, data};
	writeI2C(MPU_WRITE_ADDR, buffer, 2);
	while(!done);
}

void readRegister(uint8_t adresa, uint8_t * data, uint8_t len)
{
	uint8_t buffer[1] = {adresa};
	writeI2C(MPU_WRITE_ADDR, buffer, 1);
	while(!done);
	writeI2C(MPU_READ_ADDR, data, len);
	while(!done);
}

void initMPU(void)
{
	writeRegister(0x6B, 0x00);
	writeRegister(0x19, 0x07);
	writeRegister(0x1A, 0x00);
	writeRegister(0x1B, 0x00);
	writeRegister(0x1C, 0x00);
	writeRegister(0x23, 0x00);
	writeRegister(0x24, 0x00);
	writeRegister(0x37, 0x00);
	writeRegister(0x38, 0x01);
	writeRegister(0x67, 0x00);
	writeRegister(0x68, 0x00);
	writeRegister(0x6A, 0x00);
	writeRegister(0x6C, 0x00);
	writeRegister(0x74, 0x00);	
}

void readData(double *results)
{
	uint8_t measurements[14];
	int16_t ax, ay, az, t, gx, gy, gz;
	double AX, AY, AZ, T, GX, GY, GZ;
	
	readRegister(0x3B, measurements, 14);

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

uint8_t readWAI(void)
{
	uint8_t buffer[1];
	readRegister(0x75, buffer, 1);
	return dataB[0];
}