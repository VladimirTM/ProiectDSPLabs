#include <lpc214x.h>
#include <stdint.h>

void initTimer(void)
{
	T0MCR |= (3 << 0); // activez interrupt pe MR0 si resetez MR0
	T0MR0 = 12000000; // valoarea de match
	T0TCR |= (1 << 0); // pornesc timer-ul
}