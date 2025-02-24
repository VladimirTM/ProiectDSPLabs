#ifndef __I2C_H
#define __I2C_H

void initI2C(void);
void startI2C(void);
void stopI2C(void);
void writeI2C(char);
unsigned char readACK(unsigned char *);
unsigned char readNACK(unsigned char *);

extern int doneFunction;

#endif