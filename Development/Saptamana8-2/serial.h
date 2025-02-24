#ifndef __SERIAL_H
#define __SERIAL_H

void initUART(void);
int sendCharacter(unsigned char);
void print(uint16_t);
unsigned char getCharacter(void);
void sendString(char *);

#endif