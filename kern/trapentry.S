/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>



###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
.text;									\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name :			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps;							\
.data;									\
	.long name

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
.text;									\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name :								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps;							\
.data;									\
	.long name

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
	
 .macro  __handler name, idx
	TRAPHANDLER(\name\idx, \idx)
 .endm

.macro  __handler_noec name, idx
	TRAPHANDLER_NOEC(\name\idx, \idx)
.endm	

.data
	// the addresses of all the handlers will be stored from this 
	// location on in the data section.
	.globl vectors
	vectors: 

.irp idx, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 48
	.if \idx == 8 || (\idx >= 10 && \idx <= 15) || \idx == 17
		TRAPHANDLER(vector\idx, \idx)
	.else
		TRAPHANDLER_NOEC(vector\idx, \idx)
	.endif
.endr

/*
 * Lab 3: Your code here for _alltraps
 */

.text

_alltraps:
	pushl	%ds
	pushl	%es
	pushal

	movw	$(GD_KD), %ax
	movw	%ax, %ds
	movw	%ax, %es
	
	pushl	%esp
	call	trap
