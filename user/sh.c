// shell
#include <inc/lib.h>
//#include <inc/stdio.h>
//#include <inc/error.h>

#define BUFLEN 1024
static char buf[BUFLEN];

char *
readline(const char *prompt)
{
	int i, c, echoing;

	if (prompt != NULL)
		cprintf("%s", prompt);

	i = 0;
	echoing = 1;
	while (1) {
		c = getchar();
		if (c < 0) {
			cprintf("read error: %e\n", c);
			return NULL;
		} else if ((c == '\b' || c == '\x7f') && i > 0) {
			if (echoing)
				cputchar('\b');
			i--;
		} else if (c >= ' ' && i < BUFLEN-1) {
			if (echoing)
				cputchar(c);
			buf[i++] = c;
		} else if (c == '\n' || c == '\r') {
			if (echoing)
				cputchar('\n');
			buf[i] = 0;
			return buf;
		}
	}
}

#define CMDBUF_SIZE	80	// enough for one VGA text line

struct Command {
	const char *name;
	const char *desc;
};

static struct Command commands[] = {
	{ "help", "Display the list of commands"},
	{ "echo", "Echo the message"},
	{ "factorial", "Calculate factorial"},
	{ "fibonacci", "Calculate nth fibonacci number with seed value 0 and 1"},
	{ "date", "Display the current date"},
	{ "exit", "Exit the shell."}
};

#define NCOMMANDS (sizeof(commands)/sizeof(commands[0]))

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf)
{
	int argc;
	const char *argv[MAXARGS];
	int i;
	int bg = 0;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;

		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = NULL;		// last argument needs to be null
	
	if(!strcmp(argv[0], "exit")) {
		if(argc == 1)
			exit();
		else
			cprintf("exit accepts no arguments.\n");
	}

	if(!strcmp(argv[argc-1], "&"))
		bg = 1;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < NCOMMANDS; i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
		{			
			cprintf("forking a new process\n");
			int pid = fork();

			if(pid < 0) {
				cprintf("error in fork.\n");
			} else if(pid == 0) {
				sys_exec(argv[0], argv);
				exit();
			} else {
				cprintf("this is parent and my child's pid is : %d\n",pid);
				if(!bg){
					int status;
					while(!sys_wait(pid,&status))
					{
						if(status==1)
						{
							break;
						}	
					}
					
					cprintf("this is parent and my child: %d exited\n",pid);
				}
				
				return 0;
			}
		}
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}


void
umain(int argc, char **argv)
{

	char *buf;

	
	cprintf("Welcome to the Shell!\n");
	cprintf("Type 'help' for a list of commands.\n");
	// cprintf("You can type hello!\n");

	while (1) {
		buf = readline("$ ");
		if (buf != NULL)
			if (runcmd(buf) < 0)
				break;
	}

	
	exit();
}