/**
 * @file i2c_ll.c
 * @brief Low-level I2C driver implementation for STM32F407
 *
 * Ported from LPC2294 implementation with register-level I2C access.
 */

#include "i2c_ll.h"
#include "stm32f4xx.h"
#include "cmsis_os.h"
#include <string.h>

// External variables shared with MPU driver
extern Message message;
extern I2C_States i2cState;
extern uint8_t measurements[14];
extern uint8_t cntWrite, cntRead, cntIndex, cntOperation;
extern uint8_t lenWrite, lenRead;
extern uint8_t doneInit;

/**
 * @brief Initialize I2C1 peripheral and GPIO (PB6/PB7)
 *
 * Configures I2C1 for 100kHz operation using register-level programming.
 * APB1 clock assumed to be 42MHz.
 */
void I2C_LL_Init(void)
{
    // 1. Enable GPIOB and I2C1 clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // 2. Configure PB6 (SCL) and PB7 (SDA) as alternate function
    GPIOB->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    GPIOB->MODER |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1); // AF mode (0b10)

    // 3. Set to open-drain output type (required for I2C)
    GPIOB->OTYPER |= (GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);

    // 4. Set to high speed
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7);

    // 5. Enable internal pull-ups
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0); // Pull-up (0b01)

    // 6. Configure alternate function AF4 (I2C1) for PB6 and PB7
    GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL6 | GPIO_AFRL_AFSEL7);
    GPIOB->AFR[0] |= (4 << GPIO_AFRL_AFSEL6_Pos) | (4 << GPIO_AFRL_AFSEL7_Pos);

    // 7. Reset I2C1 peripheral
    I2C1->CR1 = I2C_CR1_SWRST;
    I2C1->CR1 = 0;

    // 8. Configure I2C1 timing (APB1 = 42MHz)
    I2C1->CR2 = 42; // FREQ = 42MHz

    // 9. Configure CCR for 100kHz (Standard Mode)
    I2C1->CCR = 210; // CCR value for 100kHz

    // 10. Configure TRISE
    I2C1->TRISE = 43; // Max rise time for 100kHz

    // 11. Enable I2C peripheral
    I2C1->CR1 = I2C_CR1_PE;

    // 12. Generate START condition to prepare for first transaction
    I2C1->CR1 |= I2C_CR1_START;
}

/**
 * @brief I2C write state machine
 *
 * Ported from LPC2294 writeAddress() with STM32 register adaptations.
 *
 * LPC2294 → STM32 Status Code Mapping:
 * 0x08 START → SR1.SB
 * 0x18 ADDR ACK → SR1.ADDR
 * 0x28 DATA ACK → SR1.TXE
 * 0x20/0x30 NACK → SR1.AF
 */
void writeAddress(uint8_t address, uint8_t value)
{
    uint32_t sr1 = I2C1->SR1;
    uint32_t sr2;

    if (sr1 & I2C_SR1_SB)  // 0x08: START condition generated
    {
        cntOperation = 0;
        I2C1->DR = 0xD0;  // Send slave address + Write (MPU6050 address)
        // START bit auto-cleared by writing to DR
    }
    else if (sr1 & I2C_SR1_ADDR)  // 0x18: Address ACK received
    {
        // CRITICAL: Clear ADDR flag by reading SR1 then SR2
        sr2 = I2C1->SR2;
        cntOperation++;
        I2C1->DR = address;  // Send register address
    }
    else if (sr1 & I2C_SR1_TXE)  // 0x28: Data sent, ACK received
    {
        cntOperation++;
        I2C1->DR = value;  // Send data value

        if (cntOperation == 3)
        {
            I2C1->CR1 |= I2C_CR1_STOP;  // Generate STOP

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
    }
    else if (sr1 & I2C_SR1_AF)  // NACK received (0x20 or 0x30)
    {
        I2C1->SR1 &= ~I2C_SR1_AF;  // Clear AF flag
        I2C1->CR1 |= I2C_CR1_STOP;  // Generate STOP
    }
}

/**
 * @brief I2C read state machine
 *
 * Ported from LPC2294 readAddress() with STM32 register adaptations.
 *
 * Handles repeated START for register read sequence:
 * START → ADDR+W → REG_ADDR → REPEATED_START → ADDR+R → DATA... → STOP
 */
void readAddress(uint8_t address)
{
    uint32_t sr1 = I2C1->SR1;
    uint32_t sr2;

    if (sr1 & I2C_SR1_SB)  // START or Repeated START condition
    {
        if (cntOperation == 0)
        {
            I2C1->DR = 0xD0;  // Send slave address + Write (for register address)
        }
        else  // Repeated START
        {
            I2C1->DR = 0xD1;  // Send slave address + Read
        }
    }
    else if (sr1 & I2C_SR1_ADDR)  // Address ACK received
    {
        sr2 = I2C1->SR2;  // Clear ADDR flag

        if (sr2 & I2C_SR2_TRA)  // Transmitter mode (sending register address)
        {
            I2C1->DR = address;  // Send register address
        }
        else  // Receiver mode (reading data)
        {
            I2C1->CR1 |= I2C_CR1_ACK;  // Enable ACK for data reception
        }
    }
    else if (sr1 & I2C_SR1_TXE)  // Register address sent
    {
        I2C1->CR1 |= I2C_CR1_START;  // Generate repeated START
        cntOperation++;
    }
    else if (sr1 & I2C_SR1_RXNE)  // Data received
    {
        measurements[cntIndex++] = I2C1->DR;
        cntRead++;

        if (cntRead == lenRead - 1)  // Second-to-last byte
        {
            I2C1->CR1 &= ~I2C_CR1_ACK;  // Disable ACK for last byte (send NACK)
        }
        else if (cntRead == lenRead)  // Last byte
        {
            I2C1->CR1 |= I2C_CR1_STOP;  // Generate STOP
            message.start = false;
            i2cState = IDLE_I2C;
            cntIndex = 0;
            cntOperation = 0;
        }
    }
    else if (sr1 & I2C_SR1_AF)  // NACK received (error)
    {
        I2C1->SR1 &= ~I2C_SR1_AF;
        I2C1->CR1 |= I2C_CR1_STOP;
    }
}

/**
 * @brief Main I2C task state machine
 *
 * Processes I2C requests from MPU task based on Message structure.
 * Direct port from LPC2294 I2CTask() function.
 */
void I2CTask(void)
{
    switch (i2cState)
    {
    case IDLE_I2C:
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

    case READ:
        message.busy = true;
        readAddress(message.address);
        break;

    case WRITE:
        message.busy = true;
        writeAddress(message.address, message.data);
        break;
    }
}

/**
 * @brief FreeRTOS task wrapper for I2CTask
 *
 * Initializes I2C peripheral then runs I2CTask in loop with 1ms period.
 */
void I2CTaskWrapper(void *argument)
{
    I2C_LL_Init();  // Initialize I2C1 peripheral

    for (;;)
    {
        I2CTask();
        osDelay(1);  // 1ms period (10x slower than LPC2294's 100μs)
    }
}
