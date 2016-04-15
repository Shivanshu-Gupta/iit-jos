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

	//TESTCASE for checking background implementation
	//UNCOMMENT and let this run in background while you run more commands on shell
	int i = 0, j;
	for(i = 0; i < 10000000; i++){
		for(j = 0; j < 10; j++);
	}
	
	cprintf("resulting factorial = %d\n", fac);
	exit();
}
