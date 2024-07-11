#ifndef __I2C_H
#define __I2C_H

void initI2C(void);
void startI2C(void);
void stopI2C(void);
void writeI2C(uint8_t);
uint8_t readACK(void);
uint8_t readNACK(void);

#endif