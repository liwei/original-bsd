/*-
 * Copyright (c) 1992 The Regents of the University of California.
 * All rights reserved.
 *
 * %sccs.include.redist.c%
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)cfree.c	5.2 (Berkeley) 01/12/93";
#endif /* LIBC_SCCS and not lint */

void
cfree(p)
	void *p;
{
	free(p);
}
