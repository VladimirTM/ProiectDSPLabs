#ifndef __MPU6050_H
#define __MPU6050_H

void writeRegister(char, char);
void readregister(char, unsigned char *);
void Wreadregister(char, unsigned char *);
void initMPU6050(void);
unsigned char whoAmI(void);
void mainFSM(void);

extern int doneWAI;

#endif