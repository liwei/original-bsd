/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)SYS.h	5.1 (Berkeley) 04/23/90
 */

#include <syscall.h>

#ifdef PROF
#define	ENTRY(x)	.globl _/**/x; \
			.data; 1:; .long 0; .text; _/**/x:	.align 4; \
			movl $1b,%eax; call mcount
#else
#define	ENTRY(x)	.globl _/**/x; .text; .align 4; _/**/x: 
#endif PROF
#define	SYSCALL(x)	2: jmp cerror; ENTRY(x); lea SYS_/**/x,%eax; LCALL(7,0); jb 2b
#define	PSEUDO(x,y)	ENTRY(x); lea SYS_/**/y, %eax; ; LCALL(7,0)
#define	CALL(x,y)	call _/**/y; addl $4*x,%esp
/* gas fucks up offset -- although we don't currently need it, do for BCS */
#define	LCALL(x,y)	.byte 0x9a ; .long y; .word x

	.globl	cerror
