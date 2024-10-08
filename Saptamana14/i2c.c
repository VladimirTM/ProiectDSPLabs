#include <lpc214x.h>
#include <stdint.h>
#include <string.h>

void delayI2C(void)
{
	return;
	volatile unsigned int i = 0;
	for (i = 0; i < 12000; i++);
}

volatile int doneFunction = 0;
#define SIZE 100

int states[SIZE], contor = 0;

void initI2C(void)
{
	PINSEL0 |= 0x00000050;
	I2C0CONSET = 0x40;
	I2C0SCLH = 0x28;
	I2C0SCLL = 0x28;
}
void WstartI2C(void)
{
	I2C0CONSET = 0x20;
	while((I2C0CONSET & 0x08) == 0);
	I2C0CONCLR = 0x28;
	doneFunction = doneFunction + 1;
}

void stopI2C(void)
{
	I2C0CONSET = 0x50;
	doneFunction = doneFunction + 1;
}

void WwriteI2C(char data)
{
	I2C0DAT = data;
	I2C0CONSET = 0x40;
	while((I2C0CONSET & 0x08) == 0);
	I2C0CONCLR = 0x08;
	doneFunction = doneFunction + 1;
}

void WreadACK(unsigned char *data)
{
	I2C0CONSET = 0x44;
	while( (I2C0CONSET & 0x08) == 0 );
	I2C0CONCLR = 0x0C;
	doneFunction = doneFunction + 1;
	*data = I2C0DAT;
}

void WreadNACK(unsigned char *data)
{
	I2C0CONSET = 0x40;
	while( (I2C0CONSET & 0x08) == 0 );
	I2C0CONCLR = 0x08;
	doneFunction = doneFunction + 1;
	*data = I2C0DAT;
}

typedef enum
{
	START,
	SETTING_REGISTER,
	CONSET_READY,
	STOP
}FUNCTIONFSM;

volatile FUNCTIONFSM startFSM = START;

void startI2C(void)
{
	delayI2C();
	states[contor] = startFSM;
	contor = (contor + 1) % SIZE;
	switch(startFSM)
	{
		case START:
		{
			I2C0CONSET = 0x20;
			startFSM = SETTING_REGISTER;
			break;
		}
		case SETTING_REGISTER:
		{
			if((I2C0CONSET & 0x08) != 0)
			{
				startFSM = CONSET_READY;
			}
			break;
		}
		case CONSET_READY:
		{
			I2C0CONCLR = 0x28;
			startFSM = STOP;
			break;
		}
		case STOP:
		{
			doneFunction = doneFunction + 1;
			startFSM = START;
			break;
		}
	}
}

FUNCTIONFSM writeFSM = START;

void writeI2C(char data)
{
	delayI2C();
	states[contor] = writeFSM;
	contor = (contor + 1) % SIZE;
	switch(writeFSM)
	{
		case START:
		{
			I2C0DAT = data;
			I2C0CONSET = 0x40;
			writeFSM = SETTING_REGISTER;
			break;
		}
		case SETTING_REGISTER:
		{
			if((I2C0CONSET & 0x08) != 0)
			{
				writeFSM = CONSET_READY;
			}
			break;
		}
		case CONSET_READY:
		{
			I2C0CONCLR = 0x08;
			writeFSM = STOP;
			break;
		}
		case STOP:
		{
			doneFunction = doneFunction + 1;
			writeFSM = START;
			break;
		}
	}
}

FUNCTIONFSM readACKFSM = START;

void readACK(unsigned char *data)
{
	delayI2C();
	states[contor] = readACKFSM;
	contor = (contor + 1) % SIZE;
	switch(readACKFSM)
	{
		case START:
		{
			I2C0CONSET = 0x44;
			readACKFSM = SETTING_REGISTER;
			break;
		}
		case SETTING_REGISTER:
		{
			if((I2C0CONSET & 0x08) != 0)
			{
				readACKFSM = CONSET_READY;
			}
			break;
		}
		case CONSET_READY:
		{
			I2C0CONCLR = 0x0C;
			*data = I2C0DAT;
			readACKFSM = STOP;
			break;
		}
		case STOP:
		{
			doneFunction = doneFunction + 1;
			readACKFSM = START;
			break;
		}
	}
}

FUNCTIONFSM readNACKFSM = START;

void readNACK(unsigned char *data)
{
	delayI2C();
	states[contor] = readNACKFSM;
	contor = (contor + 1) % SIZE;
	switch(readNACKFSM)
	{
		case START:
		{
			I2C0CONSET = 0x40;
			readNACKFSM = SETTING_REGISTER;
			break;
		}
		case SETTING_REGISTER:
		{
			if((I2C0CONSET & 0x08) != 0)
			{
				readNACKFSM = CONSET_READY;
			}
			break;
		}
		case CONSET_READY:
		{
			I2C0CONCLR = 0x08;
			*data = I2C0DAT;
			readNACKFSM = STOP;
			break;
		}
		case STOP:
		{
			doneFunction = doneFunction + 1;
			readNACKFSM = START;
			break;
		}
	}
}