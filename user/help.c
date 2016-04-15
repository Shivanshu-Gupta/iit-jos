// help
#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	// if(argc < 1) {
	// 	cprintf("Usage : help [command_name]\n");
	// 	exit();
	// }
	if(argc>=2)
	{
		if(strcmp(argv[1],"help")==0){
			cprintf("Display the list of commands\n");
		}
		else if(strcmp(argv[1],"echo")==0){
			cprintf("Echo the message\n");
		}
		else if(strcmp(argv[1],"factorial")==0){
			cprintf("Calculate the factorial of a given number\n");
		}
		else if(strcmp(argv[1],"fibonacci")==0){
			cprintf("Calculate nth fibonacci number\n");
		}
		else if(strcmp(argv[1],"date")==0){
			cprintf("Display the current date\n");
		}
		else if(strcmp(argv[1],"exit")==0){
			cprintf("Exit the shell\n");
		}
		else {
			int i;
			for(i = 1; i < argc; i++)
				cprintf("%s%s", argv[i], i+1 < argc ? " " : "\n");
			cprintf("The above command not found!\n");
		}

	}
	else
	{
		cprintf("help => Display this list of commands, \n");
		cprintf("echo => Echo the message , \n");
		cprintf("factorial => Calculate factorial , \n");
		cprintf("fibonacci => Calculate nth fibonacci number , \n");
		cprintf("date => Give the current date \n\n");	
		cprintf("exit => Exit the shell\n");
	}
	
	exit();
}
