// echo
#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	int i;
	envid_t my_id = sys_getenvid();
	cprintf("I am the echoing child and my id is: %d\n",my_id);
	for(i = 1; i < argc; i++)
		cprintf("%s%s", argv[i], i+1 < argc ? " " : "\n");
	exit();
}
