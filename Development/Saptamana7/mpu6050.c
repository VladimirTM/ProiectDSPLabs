#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
#include "i2c.h"

#define MPU6050ADRESS 0x68

// scriere clasica in registru
void writeRegister(uint8_t adresa, uint8_t valoare)
{
	startI2C();
	writeI2C(MPU6050ADRESS << 1);
	writeI2C(adresa);
	writeI2C(valoare);
	stopI2C();
}

// citire clasica din registru
uint8_t readregister(uint8_t adresa)
{
	uint8_t valoare;
	startI2C();
	writeI2C(MPU6050ADRESS << 1);
	writeI2C(adresa);
	startI2C();
	writeI2C(MPU6050ADRESS << 1 | 1);
	valoare = readNACK();
	stopI2C();
	return valoare;
}

void initMPU6050(void)
{
	writeRegister(0x6B, 0);
}

uint8_t whoAmI(void)
{
	uint8_t valoare;
	valoare = readregister(0x75);
	return valoare;
}