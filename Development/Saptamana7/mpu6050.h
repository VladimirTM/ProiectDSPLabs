#ifndef __MPU6050_H
#define __MPU6050_H

void writeRegister(uint8_t, uint8_t);
uint8_t readregister(uint8_t);
void initMPU6050(void);
uint8_t whoAmI(void);

#endif