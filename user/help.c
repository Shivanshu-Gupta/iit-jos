// factorial
#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	long idx, fib = 1;
	long num1 = 0, num2 = 1, temp;
	if(argc < 2) {
		cprintf("Usage : factorial <number>\n");
		exit();
	}

	idx = strtol(argv[1], &argv[2], 10);
	if(idx <= 0) {
		cprintf("Invalid index\n");
	}
	if(idx == 0) {
		fib = num1;
	} else {
		while(idx > 0) {
			temp = num2;
			num2 = num1 + num2;
			num1 = temp;
			idx--;
		}
		fib = num1;
	}
	cprintf("%dth fibonacci number is %d", idx, fib);
	exit();
}
