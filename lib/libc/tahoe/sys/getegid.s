/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * %sccs.include.redist.c%
 */

#if defined(SYSLIBC_SCCS) && !defined(lint)
	.asciz "@(#)getegid.s	5.4 (Berkeley) 06/01/90"
#endif /* SYSLIBC_SCCS and not lint */

#include "SYS.h"

PSEUDO(getegid,getgid)
	movl	r1,r0
	ret		# egid = getegid();
