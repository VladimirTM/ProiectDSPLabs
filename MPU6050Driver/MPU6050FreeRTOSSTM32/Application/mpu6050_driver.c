/**
 * @file mpu6050_driver.c
 * @brief MPU6050 sensor driver implementation
 *
 * Ported from LPC2294 FreeRTOS implementation to STM32F407.
 */

#include "mpu6050_driver.h"
#include "i2c_ll.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdlib.h>

#define NRERROR 10  // Maximum number of consecutive errors before marking invalid

// Global variables for I2C communication
uint8_t doneInit = 0, cntWrite = 0, cntRead = 0, cntIndex = 0, cntOperation = 0;
uint8_t lenWrite = 13, lenRead = 14;

/**
 * @brief MPU6050 initialization register configuration
 *
 * Array of register address-value pairs to initialize the MPU6050.
 * Format: [reg_addr, value, reg_addr, value, ...]
 */
uint8_t regDat[] = {
    0x6B, 0x00,  // PWR_MGMT_1: Wake up MPU6050
    0x19, 0x07,  // SMPLRT_DIV: Sample rate divider
    0x1A, 0x00,  // CONFIG: DLPF configuration
    0x1B, 0x00,  // GYRO_CONFIG: ±250°/s
    0x1C, 0x00,  // ACCEL_CONFIG: ±2g
    0x23, 0x00,  // FIFO_EN: Disable FIFO
    0x24, 0x00,  // I2C_MST_CTRL: Disable I2C master mode
    0x37, 0x00,  // INT_PIN_CFG: Interrupt configuration
    0x38, 0x01,  // INT_ENABLE: Enable data ready interrupt
    0x67, 0x00,  // SIGNAL_PATH_RESET: No reset
    0x68, 0x00,  // USER_CTRL: Disable FIFO and I2C master
    0x6A, 0x00,  // USER_CTRL: No FIFO reset
    0x6C, 0x00,  // PWR_MGMT_2: Enable all sensors
    0x74, 0x00   // Undocumented register (from LPC2294 code)
};

// Buffer for raw sensor measurements (14 bytes)
uint8_t measurements[14];

// Error tracking
uint8_t failedBefore = 0;
uint8_t numberOfFails = 0;

// Measurement storage
Measurement privateMeasurementVar;

// Communication message structure (shared with I2C layer)
Message message;

// State variables
I2C_States i2cState = IDLE_I2C;
MPU_States mpuState = IDLE_MPU;

/**
 * @brief Process raw measurements into calibrated values
 *
 * Converts 14 bytes of raw data:
 * - Bytes 0-5: Accelerometer X, Y, Z (16-bit signed)
 * - Bytes 6-7: Temperature (16-bit signed)
 * - Bytes 8-13: Gyroscope X, Y, Z (16-bit signed)
 *
 * Performs range validation and updates error counter.
 */
void processMeasurements(Measurement* measurement)
{
    int16_t ax, ay, az, t, gx, gy, gz;
    double AX, AY, AZ, T, GX, GY, GZ;

    // Combine high and low bytes (big-endian format)
    ax = ((int16_t)(measurements[0]) << 8) | ((int16_t)(measurements[1]));
    AX = (double)ax / 16384.0;  // ±2g range, 16384 LSB/g

    ay = ((int16_t)(measurements[2]) << 8) | ((int16_t)(measurements[3]));
    AY = (double)ay / 16384.0;

    az = ((int16_t)(measurements[4]) << 8) | ((int16_t)(measurements[5]));
    AZ = (double)az / 16384.0;

    t = ((int16_t)(measurements[6]) << 8) | ((int16_t)(measurements[7]));
    T = ((double)t / 340.0) + 36.53;  // Temperature formula from datasheet

    gx = ((int16_t)(measurements[8]) << 8) | ((int16_t)(measurements[9]));
    GX = (double)gx / 131.0;  // ±250°/s range, 131 LSB/(°/s)

    gy = ((int16_t)(measurements[10]) << 8) | ((int16_t)(measurements[11]));
    GY = (double)gy / 131.0;

    gz = ((int16_t)(measurements[12]) << 8) | ((int16_t)(measurements[13]));
    GZ = (double)gz / 131.0;

    // Validate measurements against expected ranges
    if ((AX < -2.0 || AX > 2.0) || (AY < -2.0 || AY > 2.0) || (AZ < -2.0 || AZ > 2.0) ||
        (GX < -250.0 || GX > 250.0) || (GY < -250.0 || GY > 250.0) || (GZ < -250.0 || GZ > 250.0))
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

    // Store integer values (×1000 for fixed-point representation)
    measurement->measurementINT.AX = AX * 1000;
    measurement->measurementINT.AY = AY * 1000;
    measurement->measurementINT.AZ = AZ * 1000;
    measurement->measurementINT.T = T * 1000;
    measurement->measurementINT.GX = GX * 1000;
    measurement->measurementINT.GY = GY * 1000;
    measurement->measurementINT.GZ = GZ * 1000;

    // Store float values
    measurement->measurementFLOAT.AX = AX;
    measurement->measurementFLOAT.AY = AY;
    measurement->measurementFLOAT.AZ = AZ;
    measurement->measurementFLOAT.T = T;
    measurement->measurementFLOAT.GX = GX;
    measurement->measurementFLOAT.GY = GY;
    measurement->measurementFLOAT.GZ = GZ;
}

/**
 * @brief Main MPU6050 task state machine
 *
 * Manages MPU6050 lifecycle:
 * - IDLE_MPU: Initial state
 * - INIT: Writes 13 configuration registers
 * - READ_DATA: Continuously reads 14 bytes of sensor data
 */
void MPUTask(void)
{
    privateMeasurementVar.valid = 0;

    switch (mpuState)
    {
    case IDLE_MPU:
        message.start = false;
        message.busy = false;
        mpuState = INIT;
        break;

    case INIT:
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

    case READ_DATA:
        if (cntRead <= lenRead)
        {
            if (message.busy == false)
            {
                message.write = false;
                message.address = 0x3B;  // ACCEL_XOUT_H register (start of 14-byte data)
                message.start = true;
            }
        }
        else
        {
            // Process measurements when all 14 bytes received
            processMeasurements(&privateMeasurementVar);

            if (numberOfFails <= NRERROR)
            {
                privateMeasurementVar.valid = 1;
            }
            else
            {
                privateMeasurementVar.valid = 0;
            }

            cntRead = 0;
        }
        break;
    }
}

/**
 * @brief Get current MPU6050 measurement
 *
 * Thread-safe copy of the latest valid measurement.
 *
 * @param data Pointer to Measurement structure to fill
 * @return 1 if measurement is valid, 0 otherwise
 */
int GetMeasurement(Measurement* data)
{
    if (privateMeasurementVar.valid == 1)
    {
        memcpy(data, &privateMeasurementVar, sizeof(Measurement));
        return 1;
    }
    return 0;
}

/**
 * @brief FreeRTOS task wrapper for MPUTask
 *
 * Runs MPUTask in loop with 3ms period (approximately 333Hz update rate).
 */
void MPUTaskWrapper(void *argument)
{
    for (;;)
    {
        MPUTask();
        osDelay(3);  // 3ms period
    }
}
