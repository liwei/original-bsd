/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * %sccs.include.redist.c%
 */

#if defined(LIBC_SCCS) && !defined(lint)
	.asciz "@(#)ntohs.s	8.1 (Berkeley) 06/04/93"
#endif /* LIBC_SCCS and not lint */

/* hostorder = ntohs(netorder) */

#include "DEFS.h"

ENTRY(ntohs, 0)
	rotl	$8,4(ap),r0
	movb	5(ap),r0
	movzwl	r0,r0
	ret
