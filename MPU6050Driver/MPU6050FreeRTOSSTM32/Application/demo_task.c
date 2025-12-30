//
// Created by valy on 14.04.2025.
//

#include "demo_task.h"

#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "usbd_cdc_if.h"

void MyDemoTask(void)
{
	static uint32_t n = 0;
	static char text[1000];
	if (n % 2)
	{
		HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin, GPIO_PIN_SET);
	}
	n++;

	sprintf (text,"Hello %d\n", n);
	CDC_Transmit_FS(text,strlen(text));
}
