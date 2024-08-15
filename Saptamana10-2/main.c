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
	for (i = 0; i < 120000; i++);
}

int main(void)
{
	doneFunction = 0;
	initUART();
	initI2C();
	delay();
	while(1)
	{
		mainFSM();
		//printf("Acclerometru : %f %f %f\n", AX, AY, AZ);
		//printf("Temperatura : %f\n", T);
		//printf("Giroscop : %f %f %f\n\n", GX, GY, GZ);
	}
	return 0;
}