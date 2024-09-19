#ifndef __I2C_H
#define __I2C_H

void initInterrupts(void);
void initI2C(void);
void startI2C(void);
void stopI2C(void);
void writeI2C(uint8_t ,uint8_t *,uint8_t);

extern uint8_t done;
extern uint8_t *dataB;
extern uint8_t indexD;

#endif