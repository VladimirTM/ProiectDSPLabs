#ifndef __I2C_H
#define __I2C_H

void initI2C(void);
void startI2C(void);
void stopI2C(void);
void writeI2C(char);
void readACK(unsigned char *);
void readNACK(unsigned char *);
void WstartI2C(void);
void WwriteI2C(char);
void WreadACK(unsigned char *);
void WreadNACK(unsigned char *);

extern int doneFunction;

#endif