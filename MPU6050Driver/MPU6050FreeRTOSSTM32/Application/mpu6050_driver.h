/**
 * @file mpu6050_driver.h
 * @brief MPU6050 sensor driver for STM32F407 with FreeRTOS
 *
 * Provides initialization, data acquisition, and measurement processing
 * for MPU6050 6-axis IMU (accelerometer + gyroscope + temperature).
 */

#ifndef MPU6050_DRIVER_H
#define MPU6050_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Integer measurement structure (scaled by 1000)
 *
 * Values are multiplied by 1000 to avoid floating point in some contexts.
 * Example: AX = 1500 represents 1.500g
 */
typedef struct
{
    int32_t AX, AY, AZ;  // Accelerometer X, Y, Z (×1000)
    int32_t T;           // Temperature (×1000)
    int32_t GX, GY, GZ;  // Gyroscope X, Y, Z (×1000)
} MeasurementInt;

/**
 * @brief Float measurement structure
 *
 * Calibrated sensor values in physical units:
 * - Accelerometer: g (gravitational acceleration)
 * - Gyroscope: degrees/second
 * - Temperature: degrees Celsius
 */
typedef struct
{
    float AX, AY, AZ;  // Accelerometer X, Y, Z (g)
    float T;           // Temperature (°C)
    float GX, GY, GZ;  // Gyroscope X, Y, Z (°/s)
} MeasurementFloat;

/**
 * @brief Complete measurement structure with validity flag
 *
 * Contains both integer and float representations of sensor data.
 */
typedef struct
{
    MeasurementInt measurementINT;
    MeasurementFloat measurementFLOAT;
    uint8_t valid;  // 1 = valid data, 0 = invalid or sensor error
} Measurement;

/**
 * @brief MPU6050 task state machine states
 */
typedef enum
{
    IDLE_MPU,
    INIT,
    READ_DATA
} MPU_States;

/**
 * @brief Get current MPU6050 measurement
 *
 * Thread-safe function to retrieve the latest valid sensor measurement.
 * Should be called by user tasks to access MPU6050 data.
 *
 * @param data Pointer to Measurement structure to fill
 * @return 1 if measurement is valid, 0 if invalid or not ready
 */
int GetMeasurement(Measurement* data);

/**
 * @brief Main MPU6050 task function
 *
 * State machine that manages MPU6050 initialization and continuous
 * data acquisition. Should be called periodically (every 3ms).
 */
void MPUTask(void);

/**
 * @brief FreeRTOS task wrapper for MPUTask
 *
 * @param argument Unused
 */
void MPUTaskWrapper(void *argument);

/**
 * @brief Process raw sensor measurements
 *
 * Converts 14 bytes of raw data into calibrated measurements.
 * Performs validation to detect out-of-range values.
 *
 * @param measurement Pointer to Measurement structure to populate
 */
void processMeasurements(Measurement* measurement);

#ifdef __cplusplus
}
#endif

#endif // MPU6050_DRIVER_H
