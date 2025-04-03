#ifndef __SYSTEMCONFIG_H
#define __SYSTEMCONFIG_H

#include <lpc22xx.h>
#include <FreeRTOSConfig.h>

#define RTC_FREQ configCPU_CLOCK_HZ
#define FREERTOS_TICK_HZ configTICK_RATE_HZ
#define FREERTOS_TICK_US (1000000UL / FREERTOS_TICK_HZ)

#define FREERTOS_US_TO_TICK(time_us) (time_us / FREERTOS_TICK_US)
#define FREERTOS_MS_TO_TICK(time_ms) (time_ms * 1000 / FREERTOS_TICK_US)

#endif


