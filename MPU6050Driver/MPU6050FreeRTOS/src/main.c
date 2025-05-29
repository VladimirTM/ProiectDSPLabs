#include <lpc22xx.h>
#include <FreeRTOS.h>
#include <serial.h>
#include <task.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "log.h"

#define NRERROR 10

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
    LOG_SERIAL("Stack overflow for task: %s\r\n", pcTaskName);
}

void vApplicationMallocFailedHook(void)
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

void TaskTest1(void* pvParameters)
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

void MyTaskWrapper(void* pvParameters)
{
    for (;;)
    {
        MyTask();
        vTaskDelay(FREERTOS_MS_TO_TICK(1000));
    }
}

uint8_t doneInit = 0, cntWrite = 0, cntRead = 0, cntIndex = 0, cntOperation = 0, lenWrite = 13, lenRead = 13;
uint8_t regDat[] = {
    0x6B, 0x00, 0x19, 0x07, 0x1A, 0x00, 0x1B, 0x00, 0x1C, 0x00, 0x23, 0x00, 0x24, 0x00, 0x37, 0x00, 0x38, 0x01, 0x67,
    0x00, 0x68, 0x00, 0x6A, 0x00, 0x6C, 0x00, 0x74, 0x00
};
uint8_t measurements[14];
uint8_t failedBefore = 0;
uint8_t numberOfFails = 0;

typedef struct
{
    int32_t AX, AY, AZ;
    int32_t T;
    int32_t GX, GY, GZ;
} MeasurementInt;

typedef struct
{
    float AX, AY, AZ;
    float T;
    float GX, GY, GZ;
} MeasurementFloat;

typedef struct
{
    MeasurementInt measurementINT;
    MeasurementFloat measurementFLOAT;
    uint8_t valid;
} Measurement;

typedef struct
{
    uint8_t address;
    uint8_t data;
    bool start;
    bool write;
    bool busy;
} Message;

Measurement privateMeasurementVar;
Message message;

typedef enum
{
    IDLE_I2C,
    READ,
    WRITE
} I2C_States;

I2C_States i2cState = IDLE_I2C;

void writeAddress(uint8_t adresa, uint8_t valoare)
{
    uint8_t stat = I2STAT;
    switch (stat)
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
            if (cntOperation == 3)
            {
                I2CONSET = 0x20;
                if (cntWrite < lenWrite)
                {
                    cntWrite++;
                }
                else
                {
                    message.start = false;
                    i2cState = IDLE_I2C;
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
    }
    if (stat != 0xF8)
    {
        I2CONCLR = 0x08;
    }
}

void readAddress(uint8_t adresa)
{
    uint8_t stat = I2STAT;
    switch (stat)
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
            if (cntRead == lenRead)
            {
                I2CONCLR = 0x3C; // genereaza data + NACK
            }
            break;
        }
    case 0x58: // data received with NACK
        {
            message.start = false;
            i2cState = IDLE_I2C;
            measurements[cntIndex++] = I2DAT;
            I2CONSET = 0x14;
            cntRead++;
            cntIndex = 0;
            break;
        }
    }
    if (stat != 0xF8)
    {
        I2CONCLR = 0x08;
    }
}

void I2CTask()
{
    //LOG_SERIAL("%d", i2cState);
    switch (i2cState)
    {
    case IDLE_I2C:
        {
            message.busy = false;
            if (message.start == true)
            {
                if (message.write == false)
                {
                    i2cState = READ;
                }
                else
                {
                    i2cState = WRITE;
                }
            }
            break;
        }
    case READ:
        {
            message.busy = true;
            readAddress(message.address);
            break;
        }
    case WRITE:
        {
            message.busy = true;
            writeAddress(message.address, message.data);
            break;
        }
    }
}

void I2CTaskWrapper(void* pvParameters)
{
    for (;;)
    {
        IO1SET = 1u << 21;
        I2CTask();
        IO1CLR = 1u << 21;
        vTaskDelay(FREERTOS_US_TO_TICK(100));
        //vTaskDelay(FREERTOS_MS_TO_TICK(1));
    }
}

void processMeasurements(Measurement* measurement)
{
    int16_t ax, ay, az, t, gx, gy, gz;
    double AX, AY, AZ, T, GX, GY, GZ;
    ax = ((int16_t)(measurements[0]) << 8) | ((int16_t)(measurements[1]));
    AX = (double)ax / 16384.0;
    ay = ((int16_t)(measurements[2]) << 8) | ((int16_t)(measurements[3]));
    AY = (double)ay / 16384.0;
    az = ((int16_t)(measurements[4]) << 8) | ((int16_t)(measurements[5]));
    AZ = (double)az / 16384.0;
    t = ((int16_t)(measurements[6]) << 8) | ((int16_t)(measurements[7]));
    T = ((double)t / 340.0) + 36.53;
    gx = ((int16_t)(measurements[8]) << 8) | ((int16_t)(measurements[9]));
    GX = (double)gx / 131.0;
    gy = ((int16_t)(measurements[10]) << 8) | ((int16_t)(measurements[11]));
    GY = (double)gy / 131.0;
    gz = ((int16_t)(measurements[12]) << 8) | ((int16_t)(measurements[13]));
    GZ = (double)gz / 131.0;
    if ((AX < -2.0 || AX > 2.0) || (AY < -2.0 || AY > 2.0) || (AZ < -2.0 || AZ > 2.0) || (GX < -250.0 || GX > 250.0) ||
        (GY < -250.0 || GY > 250.0) || (GZ < -250.0 || GZ > 250.0))
    {
        if (failedBefore == 1)
        {
            numberOfFails++;
        }
        else
        {
            failedBefore = 1;
            numberOfFails++;
        }
    }
    else
    {
        failedBefore = 0;
        numberOfFails = 0;
    }

    measurement->measurementINT.AX = AX * 1000;
    measurement->measurementINT.AY = AY * 1000;
    measurement->measurementINT.AZ = AZ * 1000;
    measurement->measurementINT.T = T * 1000;
    measurement->measurementINT.GX = GX * 1000;
    measurement->measurementINT.GY = GY * 1000;
    measurement->measurementINT.GZ = GZ * 1000;

    measurement->measurementFLOAT.AY = AY;
    measurement->measurementFLOAT.AZ = AZ;
    measurement->measurementFLOAT.T = T;
    measurement->measurementFLOAT.GX = GX;
    measurement->measurementFLOAT.GY = GY;
    measurement->measurementFLOAT.GZ = GZ;
}

void printResults(const Measurement* measurement)
{
    LOG_SERIAL("Acclerometru : %d.%u %d.%u %d.%u\n", measurement->measurementINT.AX / 1000,
               measurement->measurementINT.AX % 1000, measurement->measurementINT.AY / 1000,
               measurement->measurementINT.AY % 1000, measurement->measurementINT.AZ / 1000,
               measurement->measurementINT.AZ % 1000);
    LOG_SERIAL("Temperatura : %d.%u\n", measurement->measurementINT.T / 1000, measurement->measurementINT.T % 1000);
    LOG_SERIAL("Giroscop : %d.%u %d.%u %d.%u\n\n", measurement->measurementINT.GX / 1000,
               measurement->measurementINT.GX % 1000, measurement->measurementINT.GY / 1000,
               measurement->measurementINT.GY % 1000, measurement->measurementINT.GZ / 1000,
               measurement->measurementINT.GZ % 1000);
}

typedef enum
{
    IDLE_MPU,
    INIT,
    READ_DATA
} MPU_States;

MPU_States mpuState = IDLE_MPU;

void MPUTask()
{
    //LOG_SERIAL("%d", mpuState);
    privateMeasurementVar.valid = 0;
    switch (mpuState)
    {
    case IDLE_MPU:
        {
            message.start = false;
            message.busy = false;
            mpuState = INIT;
            break;
        }
    case INIT:
        {
            if (doneInit == 0)
            {
                if (message.busy == false)
                {
                    message.write = true;
                    message.address = regDat[2 * cntWrite];
                    message.data = regDat[2 * cntWrite + 1];
                    message.start = true;
                }
            }
            else
            {
                mpuState = READ_DATA;
            }
            break;
        }
    case READ_DATA:
        {
            if (cntRead <= lenRead)
            {
                IO1SET = 1u << 20;
                if (message.busy == false)
                {
                    message.write = false;
                    message.address = 0x3B;
                    message.start = true;
                }
            }
            else
            {
                IO1CLR = 1u << 20;
                I2CONSET = 0x00000020; // generate start
                processMeasurements(&privateMeasurementVar);
                if (numberOfFails <= NRERROR)
                {
                    privateMeasurementVar.valid = 1;
                }
                else
                {
                    privateMeasurementVar.valid = 0;
                    I2CONCLR = 0x00000040;
                }
                cntRead = 0;
            }
            break;
        }
    }
}

int GetMeasurement(Measurement* data)
{
    if (privateMeasurementVar.valid == 1)
    {
        memcpy(data, &privateMeasurementVar, sizeof(Measurement));
        return 1;
    }
    return 0;
}

void userTask()
{
    Measurement myMeasurement;
    if (GetMeasurement(&myMeasurement) == 1)
    {
//        printResults(&myMeasurement);
    }
}

void dummyTask1()
{
    int temp = rand() % 100;
    for (int i = 0; i < temp; i++);
}

void dummyTask1Wrapper(void* pvParameters)
{
    for (;;)
    {
        dummyTask1();
        vTaskDelay(FREERTOS_US_TO_TICK(5000));
    }
}

void dummyTask2()
{
    int temp = rand() % 100;
    for (int i = 0; i < temp; i++);
}

void dummyTask2Wrapper(void* pvParameters)
{
    for (;;)
    {
        dummyTask2();
        vTaskDelay(FREERTOS_US_TO_TICK(5000));
    }
}


void MPUTaskWrapper(void* pvParameters)
{
    for (;;)
    {
        IO1SET = 1u << 22;
        MPUTask();
        IO1CLR = 1u << 22;
        vTaskDelay(FREERTOS_US_TO_TICK(300));
        //vTaskDelay(FREERTOS_MS_TO_TICK(3));
    }
}

void userTaskWrapper(void* pvParameters)
{
    for (;;)
    {
        //linia pe 1
        userTask();
        //linia pe 0
        vTaskDelay(FREERTOS_US_TO_TICK(950));
        //vTaskDelay(FREERTOS_MS_TO_TICK(5));
    }
}

void delay_seconds(int seconds)
{
    volatile unsigned int i = 0;
    for (i = 0; i < 1474560 * seconds; i++);
}

int main(void)
{
    srand(100);
    volatile int a = 4;
    set_pll();

    MAMCR = 2;
    MAMTIM = 4;

    // Init Debug methods(LEDs and one output to measure the duration of an I2C transaction)
    PINSEL1 &= ~((1 << 29) | (1 << 28));
    PINSEL0 |= 5;
    IO0DIR |= 1 << 30; //P0.30 output
    PINSEL1 &= ~((1 << 16) | (1 << 17));
    IO1DIR |= (1 << 20); //P1.20 output
    IO1DIR |= (1 << 21); //P1.21 output
    IO1DIR |= (1 << 22); //P1.22 output
    // Init I2C
    PINSEL0 |= (1 << 4) | (1 << 6); // I2C
    I2CONSET = 0x40; // enable I2C
    I2SCLH = 0x14;
    I2SCLL = 0x14;
    // Start Sensor
    I2CONCLR = 0x000000FF; // reset I2C settings
    I2CONSET = 0x00000040; // enable I2C
    I2CONSET = 0x00000020; // generate start
    LOG_init();

    delay_seconds(5);

    xTaskCreate(TaskTest1, "Test1", 100, NULL, 2, (TaskHandle_t*)NULL);
    xTaskCreate(I2CTaskWrapper, "I2CTaskWrapper", 200, NULL, 7, (TaskHandle_t*)NULL);
    xTaskCreate(MPUTaskWrapper, "MPU", 200, NULL, 6, (TaskHandle_t*)NULL);
    xTaskCreate(userTaskWrapper, "USRAPI", 200, NULL, 2, (TaskHandle_t*)NULL);
    xTaskCreate(dummyTask1Wrapper, "Task1", 100, NULL, 2, (TaskHandle_t*)NULL);
    xTaskCreate(dummyTask2Wrapper, "Task2", 100, NULL, 2, (TaskHandle_t*)NULL);
    vTaskStartScheduler();
    //

    while (1);

    return 0;
}
