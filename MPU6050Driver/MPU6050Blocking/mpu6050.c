#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
#include "i2c.h"

#define MPU_WRITE_ADDR 0xD0
#define MPU_READ_ADDR 0xD1

void writeRegister(char adresa, char valoare)
{
	startI2C();
	writeI2C(MPU_WRITE_ADDR);
	writeI2C(adresa);
	writeI2C(valoare);
	stopI2C();
}

unsigned char readregister(char adresa)
{
	unsigned char valoare;
	startI2C();
	writeI2C(MPU_WRITE_ADDR);
	writeI2C(adresa);
	startI2C();
	writeI2C(MPU_READ_ADDR);
	valoare = readNACK();
	stopI2C();
	return valoare;
}

void initMPU6050(void)
{
	writeRegister(0x6B, 0x00); // Clock intern
	writeRegister(0x19, 0x07); // Data clock divider
	writeRegister(0x1A, 0x00); // Configurare
	writeRegister(0x1B, 0x00); // Configurare range giroscop
	writeRegister(0x1C, 0x00); // Configurare range accelerometru
	writeRegister(0x23, 0x00); // Oprire FIFO
	writeRegister(0x24, 0x00); // I2C frecventa de 348KHz
	writeRegister(0x37, 0x00);
	writeRegister(0x38, 0x01); // Interupt cand datele au ajuns
	writeRegister(0x67, 0x00);
	writeRegister(0x68, 0x00);
	writeRegister(0x6A, 0x00);
	writeRegister(0x6C, 0x00);
	writeRegister(0x74, 0x00);	
}

unsigned char whoAmI(void)
{
	unsigned char valoare;
	valoare = readregister(0x75);
	return valoare;
}