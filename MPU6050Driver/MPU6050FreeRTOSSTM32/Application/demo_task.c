//
// Created by valy on 14.04.2025.
//

#include "demo_task.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "usbd_cdc_if.h"
#include "mpu6050_driver.h"

void MyDemoTask(void)
{
	static uint32_t n = 0;
	static char text[1000];
	Measurement myMeasurement;

	if (n % 2)
	{
		HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin, GPIO_PIN_SET);
	}
	n++;

	// Comment out Hello message
	// sprintf (text,"Hello %d\n", n);
	// CDC_Transmit_FS(text,strlen(text));

	// Display MPU6050 measurements
	if (GetMeasurement(&myMeasurement) == 1)
	{
		sprintf(text, "Accel: %d.%03d %d.%03d %d.%03d | Temp: %d.%03d | Gyro: %d.%03d %d.%03d %d.%03d\n",
				myMeasurement.measurementINT.AX / 1000, abs(myMeasurement.measurementINT.AX % 1000),
				myMeasurement.measurementINT.AY / 1000, abs(myMeasurement.measurementINT.AY % 1000),
				myMeasurement.measurementINT.AZ / 1000, abs(myMeasurement.measurementINT.AZ % 1000),
				myMeasurement.measurementINT.T / 1000, abs(myMeasurement.measurementINT.T % 1000),
				myMeasurement.measurementINT.GX / 1000, abs(myMeasurement.measurementINT.GX % 1000),
				myMeasurement.measurementINT.GY / 1000, abs(myMeasurement.measurementINT.GY % 1000),
				myMeasurement.measurementINT.GZ / 1000, abs(myMeasurement.measurementINT.GZ % 1000));
		CDC_Transmit_FS((uint8_t*)text, strlen(text));
	}
	else
	{
		sprintf(text, "MPU6050: No valid data\n");
		CDC_Transmit_FS((uint8_t*)text, strlen(text));
	}
}
