/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department.
 *
 * %sccs.include.redist.c%
 */

#if defined(LIBC_SCCS) && !defined(lint)
	.asciz "@(#)abs.s	8.1 (Berkeley) 06/04/93"
#endif /* LIBC_SCCS and not lint */

/* abs - int absolute value */

#include "DEFS.h"

ENTRY(abs)
	movl	sp@(4),d0
	jge	L1
	negl	d0
L1:
	rts
