// echo
#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	int i;
	for(i = 1; i < argc; i++)
		cprintf("%s%s", argv[i], i+1 < argc ? " " : "\n");
	exit();
}
