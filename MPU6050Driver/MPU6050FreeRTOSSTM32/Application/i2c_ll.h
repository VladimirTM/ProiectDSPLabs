/**
 * @file i2c_ll.h
 * @brief Low-level I2C driver for STM32F407 using register-level programming
 *
 * This driver provides polling-based I2C communication using state machines,
 * ported from LPC2294 implementation to STM32F407.
 */

#ifndef I2C_LL_H
#define I2C_LL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief I2C state machine states
 */
typedef enum
{
    IDLE_I2C,
    READ,
    WRITE
} I2C_States;

/**
 * @brief Message structure for communication between tasks
 */
typedef struct
{
    uint8_t address;  // Register address
    uint8_t data;     // Data to write
    bool start;       // Request flag - set by MPU task
    bool write;       // Direction: true = write, false = read
    bool busy;        // Status flag - set by I2C task
} Message;

/**
 * @brief Initialize I2C1 peripheral and GPIO pins
 *
 * Configures PB6 (SCL) and PB7 (SDA) for I2C1 operation at 100kHz
 * using register-level programming (no HAL).
 */
void I2C_LL_Init(void);

/**
 * @brief I2C write state machine
 *
 * Handles I2C write transaction using polling and status flags.
 * Based on LPC2294 writeAddress() function.
 *
 * @param address Register address to write to
 * @param value Data value to write
 */
void writeAddress(uint8_t address, uint8_t value);

/**
 * @brief I2C read state machine
 *
 * Handles I2C read transaction using polling and status flags.
 * Based on LPC2294 readAddress() function.
 *
 * @param address Register address to read from
 */
void readAddress(uint8_t address);

/**
 * @brief Main I2C task function
 *
 * State machine that processes I2C requests from MPU task.
 * Should be called periodically (every 1ms).
 */
void I2CTask(void);

/**
 * @brief FreeRTOS task wrapper for I2CTask
 *
 * @param argument Unused
 */
void I2CTaskWrapper(void *argument);

#ifdef __cplusplus
}
#endif

#endif // I2C_LL_H
