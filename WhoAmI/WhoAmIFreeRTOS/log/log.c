//
// Created by valy on 10/31/24.
//

#include "log.h"
#include <FreeRTOS.h>
#include <serial.h>
#include <stdarg.h>
#include <stdio.h>

void LOG_init(void)
{
	xSerialPortInitMinimal(LOG_BAUDRATE, LOG_QUEUE_SIZE);
}

void LOG_SERIAL(const char *format, ...)
{
#ifdef LOG_ENABLED
	va_list ap;
	va_start(ap, format);
	static char buf[LOG_BUFFER_SIZE];
	static size_t len = 0;
	len = vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	if (len > sizeof(buf) - 1)
		len = sizeof(buf) - 1;
	vSerialPutString(NULL, buf, len);
#endif
}