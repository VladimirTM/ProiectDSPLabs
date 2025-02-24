#ifndef __MPU6050_H
#define __MPU6050_H

void writeRegister(uint8_t , uint8_t);
void readRegister(uint8_t , uint8_t *, uint8_t);
void initMPU(void);
void readData(double *);
uint8_t readWAI(void);

#endif