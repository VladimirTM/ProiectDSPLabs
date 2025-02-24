#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
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
	initMPU6050();
	delay();
	while(1)
	{
		uint8_t value;
		value = whoAmI();
		print(value);
		delay();
	}
	return 0;
}