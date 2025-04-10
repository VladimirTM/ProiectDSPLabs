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

uint8_t doneInit = 0, cntWrite = 0, cntRead = 0, cntIndex = 0, cntOperation = 0, lenWrite = 13, lenRead = 13;
uint8_t regDat[] = {0x6B, 0x00, 0x19, 0x07, 0x1A, 0x00, 0x1B, 0x00, 0x1C, 0x00, 0x23, 0x00, 0x24, 0x00, 0x37, 0x00, 0x38, 0x01, 0x67, 0x00, 0x68, 0x00, 0x6A, 0x00, 0x6C, 0x00, 0x74, 0x00};
uint8_t measurements[14];
int32_t results[7];

void pollingWrite(uint8_t adresa, uint8_t valoare)
{
	uint8_t stat = I2STAT;
	switch(stat)
	{
		case 0x08: // start
		{
			cntOperation = 0;
			I2DAT = 0xD0;
			I2CONCLR = 0x20;
			I2CONSET = 0x04;
			break;
		}
		case 0x18: // enter data ACK received
		{
			cntOperation++;
			I2DAT = adresa;
			break;
		}
		case 0x20: // enter data NACK received
		{
			I2CONSET = 0x14;
			break;
		}
		case 0x28: // data sent with ACK
		{
			cntOperation++;
			I2DAT = valoare;
			if(cntOperation == 3)
			{
				I2CONSET = 0x20;
				if(cntWrite < lenWrite)
				{
					cntWrite++;
				}
				else
				{
					doneInit = 1;
				}
			}
			break;
		}
		case 0x30: // data sent with NACK
		{
			I2CONSET = 0x14;
			break;
		}
		default:
		{
				if (I2STAT != 0xF8)
					LOG_SERIAL("0x%x\n", I2STAT); // for debugging error messages
			break;
		}
	}
	if (stat != 0xF8)
	{
		I2CONCLR = 0x08;
	}
}

void pollingRead(uint8_t adresa)
{
	uint8_t stat = I2STAT;
	switch(stat)
	{
		case 0x08: // start
		{
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
		case 0x18: // enter data ACK received
		{
			I2DAT = adresa;
			break;
		}
		case 0x20: // enter data NACK received
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
			I2CONSET = 0x04; // genereaza data + ACK
			break;
		}
		case 0x48: //  address received with NACK
		{
			I2CONSET = 0x20;
			break;
		}
		case 0x50: // data received with ACK
		{
			measurements[cntIndex++] = I2DAT;
			cntRead++;
			if(cntRead == lenRead)
			{
				I2CONCLR = 0x3C; // genereaza data + NACK
			}
			break;
		}
		case 0x58: // data received with NACK
		{
			measurements[cntIndex++] = I2DAT;
			I2CONSET = 0x14;
			cntRead++;
			cntIndex = 0;
			break;
		}
		default:
		{
				if (I2STAT != 0xF8)
					LOG_SERIAL("0x%x\n", I2STAT); // for debugging error messages
			break;
		}
	}
	if (stat != 0xF8)
	{
		I2CONCLR = 0x08;
	}
}

void processMeasurements()
{
	int16_t ax, ay, az, t, gx, gy, gz;
	double AX, AY, AZ, T, GX, GY, GZ;
	ax = ((int16_t)(measurements[0]) << 8) | ((int16_t)(measurements[1]));
	AX = ((double)ax / 16384.0) * 1000;
	ay = ((int16_t)(measurements[2]) << 8) | ((int16_t)(measurements[3]));
	AY = ((double)ay / 16384.0) * 1000;
	az = ((int16_t)(measurements[4]) << 8) | ((int16_t)(measurements[5]));
	AZ = ((double)az / 16384.0) * 1000;
	t = ((int16_t)(measurements[6]) << 8) | ((int16_t)(measurements[7]));
	T = (((double)t / 340.0) + 36.53) * 1000;
	gx = ((int16_t)(measurements[8]) << 8) | ((int16_t)(measurements[9]));
	GX = ((double)gx / 131.0) * 1000;
	gy = ((int16_t)(measurements[10]) << 8) | ((int16_t)(measurements[11]));
	GY = ((double)gy / 131.0) * 1000;
	gz = ((int16_t)(measurements[12]) << 8) | ((int16_t)(measurements[13]));
	GZ = ((double)gz / 131.0) * 1000;
	results[0] = AX;
	results[1] = AY;
	results[2] = AZ;
	results[3] = T;
	results[4] = GX;
	results[5] = GY;
	results[6] = GZ;
}

void printResults()
{
	LOG_SERIAL("Acclerometru : %d.%u %d.%u %d.%u\n", results[0] / 1000, results[0] % 1000, results[1] / 1000, results[1] % 1000, results[2] / 1000, results[2] % 1000);
	LOG_SERIAL("Temperatura : %d.%u\n", results[3] / 1000, results[3] % 1000);
	LOG_SERIAL("Giroscop : %d.%u %d.%u %d.%u\n\n", results[4] / 1000, results[4] % 1000, results[5] / 1000, results[5] % 1000,results[6] / 1000, results[6] % 1000);
}

void MPUTask()
{
	if(doneInit == 0) // initializare
	{
		pollingWrite(regDat[2 * cntWrite], regDat[2 * cntWrite + 1]);
	}
	else // citire
	{
		if(cntRead <= lenRead)
		{
			IO0SET = 1u << 24;
			pollingRead(0x3B);
		}
		else {
			processMeasurements();
			printResults();
			cntRead = 0;
			IO0CLR = 1u << 24;
			I2CONSET = 0x00000020; // generate start
		}
	}
}

void MPUTaskWrapper(void *pvParameters)
{
	for (;;)
	{
		MPUTask();
		vTaskDelay(FREERTOS_US_TO_TICK(1000));
	}
}

int main(void)
{
	volatile int a = 4;
	set_pll();

	MAMCR = 2;
	MAMTIM = 4;

	// Init Debug methods(LEDs and one output to measure the duration of an I2C transaction)
	PINSEL1 &= ~((1 << 29) | ( 1 << 28 ));
	PINSEL0 |= 5;
	IO0DIR |= 1 << 30; //P0.30 output
	PINSEL1 &= ~((1 << 16) | ( 1 << 17 ));
	IO0DIR |= (1 << 24); //P0.24 output
	// Init I2C
	PINSEL0 |= (1 << 4) | (1 << 6); // I2C
	I2CONSET = 0x40; // enable I2C
	I2SCLH = 0x64;
	I2SCLL = 0x64;
	// Start Sensor
	I2CONCLR = 0x000000FF; // reset I2C settings
	I2CONSET = 0x00000040; // enable I2C
	I2CONSET = 0x00000020; // generate start
	LOG_init();

	xTaskCreate( TaskTest1, "Test1", 100, NULL, 2, ( TaskHandle_t * ) NULL );
	xTaskCreate(MPUTaskWrapper, "MPU", 500, NULL, 2,  ( TaskHandle_t * ) NULL);
	vTaskStartScheduler();
	//

	while(1);

	return 0;
}

