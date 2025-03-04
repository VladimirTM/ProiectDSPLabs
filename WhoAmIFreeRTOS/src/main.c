#include <lpc22xx.h>
#include <FreeRTOS.h>
#include <serial.h>
#include <task.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "log.h"

void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName )
{
	LOG_SERIAL("Stack overflow for task: %s\r\n", pcTaskName);
}

void vApplicationMallocFailedHook( void )
{
	unsigned int i = 0;
	while (1)
	{
		IO0SET = 1u << 30;
		for (i = 0; i < 100000; i++);
		IO0CLR = 1u << 30;
		for (i = 0; i < 100000; i++);
	}
}

void TaskTest1(void *pvParameters)
{
	static unsigned int n = 0;
	for (;;)
	{
		n++;
		if (n % 2)
		{
			IO0SET = 1u << 30;
		}
		else
		{
			IO0CLR = 1u << 30;
		}
		vTaskDelay(500);
	}
}

void set_pll(void)
{
	PLLCON = 0x01;
	PLLCFG = 0x23;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	while ((PLLSTAT & (1 << 10)) == 0);
	PLLCON = 0x03;
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	VPBDIV = 0;
}

void MyTask()
{
	LOG_SERIAL("MyTask\r\n");
}

void MyTaskWrapper(void *pvParameters)
{
	for (;;)
	{
		MyTask();
		vTaskDelay(FREERTOS_MS_TO_TICK(1000));
	}
}

volatile uint8_t wai = 0, donewai = 0;

void WhoAmITask()
{
	switch(I2STAT)
	{
	case 0x08: // start
		{
			IO0SET = 1u << 24;
			I2DAT = 0xD0;
			I2CONCLR = 0x20;
			I2CONSET = 0x04;
			break;
		}
	case 0x10: // repeated start
		{
			I2DAT = 0xD1;
			I2CONCLR = 0x20;
			I2CONSET = 0x04;
			break;
		}
	case 0x18: // enter data
		{
			I2DAT = 0x75;
			break;
		}
	case 0x20: // NACK received
		{
			I2CONSET = 0x14;
			break;
		}
	case 0x28: // data sent with ACK
		{
			I2CONSET = 0x20;
			break;
		}
	case 0x30: // data sent with NACK
		{
			I2CONSET = 0x14;
			break;
		}
	case 0x40: // address received with ACK
		{
			I2CONCLR = 0x3C; // generez data + NACK
			break;
		}
	case 0x48: //  address received with NACK
		{
			I2CONSET = 0x20;
			break;
		}
	case 0x50: // data received with ACK
		{
			wai = I2DAT;
			I2CONSET = 0x14;
			donewai = 1;
			break;
		}
	case 0x58: // data received with NACK
		{
			wai = I2DAT;
			I2CONSET = 0x14;
			donewai = 1;
			IO0CLR = 1u << 24;
			break;
		}
	default:
		{
			if (I2STAT != 0xF8)
				LOG_SERIAL("0x%x\n", I2STAT); // for debugging error messages
		}
	}
	if (I2STAT != 0xF8)
	{
		I2CONCLR = 0x08;
	}
}

void WhoAmITaskWrapper(void *pvParameters)
{
	for (;;)
	{
		WhoAmITask();
		if (donewai == 1)
		{
			LOG_SERIAL("0x%x\t", wai);
			I2CONSET = 0x00000020; // send start to restart the transaction
			donewai = 0;
		}
		vTaskDelay(FREERTOS_US_TO_TICK(1000));
	}
}

int main(void)
{
	volatile int a = 4;
	set_pll();

	MAMCR = 2;
	MAMTIM = 4;

	PINSEL1 &= ~((1 << 29) | ( 1 << 28 ));
	PINSEL0 |= 5;
	IO0DIR |= 1 << 30; //P0.30 output
	PINSEL1 &= ~((1 << 16) | ( 1 << 17 ));
	IO0DIR |= (1 << 24); //P0.24 output
	PINSEL0 |= (1 << 4) | (1 << 6); // I2C
	I2CONSET = 0x40; // enable I2C
	I2SCLH = 0x64;
	I2SCLL = 0x64;
	I2CONCLR = 0x000000FF; // reset I2C settings
	I2CONSET = 0x00000040; // enable I2C
	I2CONSET = 0x00000020; // generate start
	LOG_init();

	xTaskCreate( TaskTest1, "Test1", 100, NULL, 2, ( TaskHandle_t * ) NULL );
	xTaskCreate(WhoAmITaskWrapper, "WAI", 500, NULL, 2,  ( TaskHandle_t * ) NULL);
	vTaskStartScheduler();
	//

	while(1);

	return 0;
}

