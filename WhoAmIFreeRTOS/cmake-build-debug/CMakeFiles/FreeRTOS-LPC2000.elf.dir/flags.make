# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

# compile ASM with /usr/bin/arm-none-eabi-gcc
# compile C with /usr/bin/arm-none-eabi-gcc
ASM_DEFINES = -DINCLUDE_uxTaskGetStackHighWaterMark=1 -DLOG_ENABLED -DROM_RUN -DconfigCHECK_FOR_STACK_OVERFLOW=1 -DconfigUSE_MALLOC_FAILED_HOOK=1

ASM_INCLUDES = -I/home/dspproject/1/lpc2000-demo-projects/FreeRTOS-LPC2000/FreeRTOS -I/home/dspproject/1/lpc2000-demo-projects/FreeRTOS-LPC2000/LinkerScript -I/home/dspproject/1/lpc2000-demo-projects/FreeRTOS-LPC2000/cpu -I/home/dspproject/1/lpc2000-demo-projects/FreeRTOS-LPC2000/src -I/home/dspproject/1/lpc2000-demo-projects/FreeRTOS-LPC2000/log -I/home/dspproject/1/lpc2000-demo-projects/FreeRTOS-LPC2000/includes

ASM_FLAGS = -g -mfloat-abi=soft -mcpu=arm7tdmi-s -mthumb-interwork -ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -g -g3 -ggdb -Og -O0

C_DEFINES = -DINCLUDE_uxTaskGetStackHighWaterMark=1 -DLOG_ENABLED -DROM_RUN -DconfigCHECK_FOR_STACK_OVERFLOW=1 -DconfigUSE_MALLOC_FAILED_HOOK=1

C_INCLUDES = -I/home/dspproject/1/lpc2000-demo-projects/FreeRTOS-LPC2000/FreeRTOS -I/home/dspproject/1/lpc2000-demo-projects/FreeRTOS-LPC2000/LinkerScript -I/home/dspproject/1/lpc2000-demo-projects/FreeRTOS-LPC2000/cpu -I/home/dspproject/1/lpc2000-demo-projects/FreeRTOS-LPC2000/src -I/home/dspproject/1/lpc2000-demo-projects/FreeRTOS-LPC2000/log -I/home/dspproject/1/lpc2000-demo-projects/FreeRTOS-LPC2000/includes

C_FLAGS = -g -std=gnu11 -fdiagnostics-color=always -mfloat-abi=soft -mcpu=arm7tdmi-s -mthumb-interwork -ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -g -g3 -ggdb -Og -O0

