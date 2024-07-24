#include <lpc214x.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "serial.h"
#include "i2c.h"
#include "mpu6050.h"

typedef enum
{
	MPU6050_NOT_INIT,
	MPU6050_INIT,
	MPU6050_IDLE,
}MPU6050_MAIN_STATES;

void blink(void)
{
	static int state = 0;
	if(state == 0)
	{
		IO1SET |= (1 << 16);
		state = 1;
	}
	else
	{
		IO1CLR |= (1 << 16);
		state = 0;
	}
}

__irq void timer_irq()
{
	blink();
	T0IR |= 1;
	VICVectAddr = 0x00;
}

void initInterrupt(void)
{
	VICIntEnable |= 1 << 4;
	VICIntSelect |= 1 << 4;
}

void initTimer(void)
{
	PINSEL2 &= ~(1 << 3);
	IO1DIR |= (1 << 16);
	T0CTCR = 0x00;
	T0TCR = 0x02;
	T0MR0 = 12000;
	T0MCR |= (3 << 0);
	T0TCR = 0x01;
}

int main(void)
{
	initTimer();
	initInterrupt();
	while(1)
	{
	}
	return 0;
}