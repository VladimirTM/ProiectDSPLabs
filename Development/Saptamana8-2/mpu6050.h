#ifndef __MPU6050_H
#define __MPU6050_H

void writeRegister(char, char);
unsigned char readregister(char);
void initMPU6050(void);
unsigned char whoAmI(void);

#endif