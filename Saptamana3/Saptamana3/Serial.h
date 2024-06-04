#ifndef __SERIAL_H
#define __SERIAL_H

uint8_t flag;

void initUART(void);
void sendCharacter(char);
void sendString(char *);
uint8_t UART_ReceiveByte(void);

#endif