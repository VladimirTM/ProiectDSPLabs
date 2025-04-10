#ifndef __MPU6050_H
#define __MPU6050_H

void startSensor(void);
void pollingWrite(uint8_t, uint8_t);
void pollingRead(uint8_t);
void processMeasurements(uint8_t *, double *);

extern volatile int cntOperation, cntWrite, cntRead, cntIndex;
extern volatile int doneInit;
extern uint8_t regDat[28];
extern uint8_t lenWrite, lenRead;
extern uint8_t measurements[14];
extern double results[7];

#endif