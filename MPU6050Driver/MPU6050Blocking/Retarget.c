#include <stdio.h>
#include <rt_misc.h>
#include "serial.h"
#pragma import(__use_no_semihosting_swi)

struct __FILE { int handle;};
FILE __stdout;

int fputc(int ch, FILE *f) {
  return (sendCharacter(ch));
}

int ferror(FILE *f) {
  return EOF;
}

void _ttywrch(int ch) {
  sendCharacter(ch);
}

void _sys_exit(int return_code) {
	return;
}
