// factorial
#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	long num, fac = 1;
	if(argc < 2) {
		cprintf("Usage : factorial <number>\n");
	}

	num = strtol(argv[1], &argv[2], 10);
	while(num > 0) {
		fac *= num--;
	}
	cprintf("factorial = %d\n", fac);
	exit();
}
