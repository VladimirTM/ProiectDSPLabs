#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "serial.h"
#include "mpu6050.h"

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
	 - reglat sensibilitati date de user + default puse de noi
*/
	
void initI2C(void)
{
	PINSEL0 |= 0x00000050;
	I2C0CONSET = 0x40;
	I2C0SCLH = 0x28;
	I2C0SCLL = 0x28;
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

void initDriver(void)
{
	initUART();
	initI2C();
	initInterrupts();
	startSensor();
	initTimer();
}

__irq void irq_handler(void)
{	
	if(doneInit == 0) // initializare
	{
		pollingWrite(regDat[2 * cntWrite], regDat[2 * cntWrite + 1]);
	}
	else // citire
	{
		if(cntRead <= lenRead)
		{
			pollingRead(0x3B);
		}
		else {
			processMeasurements(measurements, results);
			printResults(results);
			cntRead = 0;
			startSensor();
		}
	}
	T0IR |= 1;
	VICVectAddr = 0x00000000;
}

int main(void)
{
	initDriver();
	while(1)
	{
	}
	return 0;
}