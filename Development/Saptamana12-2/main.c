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

int main(void)
{
	initUART();
	initI2C();
	initInterrupts();
	initMPU();
	double results[7];
	//uint8_t wai = readWAI();
	while(1)
	{
		//print(wai);
		
		delay();
	}
	return 0;
}