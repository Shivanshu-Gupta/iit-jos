// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/kdebug.h>
#include <kern/trap.h>

#define CMDBUF_SIZE	80	// enough for one VGA text line


struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
	{ "backtrace", "Display Stack backtrace", mon_backtrace },
	{ "stepi", "Step through instructions", mon_stepi },
	{ "continue", "Continue execution", mon_continue }
};
#define NCOMMANDS (sizeof(commands)/sizeof(commands[0]))

/***** Implementations of basic kernel monitor commands *****/

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
	int i;

	for (i = 0; i < NCOMMANDS; i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
	extern char _start[], entry[], etext[], edata[], end[];

	cprintf("Special kernel symbols:\n");
	cprintf("  _start                  %08x (phys)\n", _start);
	cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
	cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("Kernel executable memory footprint: %dKB\n",
		ROUNDUP(end - entry, 1024) / 1024);
	return 0;
}

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
	// Your code here.
	cprintf("Stack backtrace:\n");
	uint32_t fn_line;
	uint32_t ebp, eip;	
	struct Eipdebuginfo info;

	next:
	eip = (uintptr_t)&&next;
	debuginfo_eip((uintptr_t)eip, &info);
	cprintf("current eip=%08x\n", eip);
	cprintf("\t %s:%d: ", info.eip_file, info.eip_line);
	fn_line = eip - info.eip_fn_addr;
	cprintf("%.*s+%d\n", info.eip_fn_namelen, info.eip_fn_name, fn_line);
	//cprintf("\t fnargs: %d\n", info.eip_fn_narg);
	
	ebp = read_ebp();
	while(ebp>0){
		eip = ((uint32_t*)ebp)[1];
		debuginfo_eip((uintptr_t)eip, &info);	
		cprintf("ebp %08x  ",ebp);
		cprintf("eip %08x  ",eip);
		cprintf("args %08x ",((uint32_t*)ebp)[2]);	
		cprintf("%08x ",((uint32_t*)ebp)[3]);
		cprintf("%08x ",((uint32_t*)ebp)[4]);
		cprintf("%08x ",((uint32_t*)ebp)[5]);
		cprintf("%08x\n",((uint32_t*)ebp)[6]);
		cprintf("\t %s:%d: ", info.eip_file, info.eip_line);
		fn_line = eip - info.eip_fn_addr;
		cprintf("%.*s+%d\n", info.eip_fn_namelen, info.eip_fn_name, fn_line);
		//cprintf("\t fnargs: %d\n", info.eip_fn_narg);
		ebp = ((uint32_t*)ebp)[0];
	}
	return 0;
}

int
mon_stepi(int argc, char **argv, struct Trapframe *tf)
{
	// start single stepping only if a Breakpoint Exception 
	// or a Debug Exception occured.
	if(tf == NULL || !(tf->tf_trapno == T_BRKPT || tf->tf_trapno == T_DEBUG)) {
		cprintf("can't stepi after a non breakpoint/debug interrupt.\n");
		return 0;
	}

	tf->tf_eflags |= FL_TF;
	cprintf("eip : 0x%08x\n",tf->tf_eip);
	return -1;
}

int
mon_continue(int argc, char **argv, struct Trapframe *tf)
{
	tf->tf_eflags &= ~FL_TF;
	return -1;
}

/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

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
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < NCOMMANDS; i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

void
monitor(struct Trapframe *tf)
{
	char *buf;

	cprintf("Welcome to the JOS kernel monitor!\n");
	cprintf("Type 'help' for a list of commands.\n");

	if (tf != NULL)
		print_trapframe(tf);

	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}
