/*
 * Sapatamana2.c
 *
 * Created: 28.05.2024 10:02:21
 * Author : dspproject
 */ 

#define F_CPU 14745600UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "timer.h"
#include "7segment.h"

// perioada de 100ms
// frecventa de 14.7456MHz, care va fi divizata in 64 de diviziuni pentru a incapea pe 16 biti

void init_LED(void)
{
	DDRA |= 1;
}

int main(void)
{
	A4();
}

