/*-
 * Copyright (c) 1982, 1988 The Regents of the University of California.
 * All rights reserved.
 *
 * %sccs.include.proprietary.c%
 *
 *	@(#)close.c	7.1 (Berkeley) 05/05/91
 */

#include <sys/param.h>
#include "saio.h"

close(fdesc)
	int fdesc;
{
#ifndef SMALL
	struct iob *file;

	fdesc -= 3;
	if (fdesc < 0 || fdesc >= SOPEN_MAX ||
	    ((file = &iob[fdesc])->i_flgs&F_ALLOC) == 0) {
		errno = EBADF;
		return (-1);
	}
	if ((file->i_flgs&F_FILE) == 0)
		devclose(file);
	file->i_flgs = 0;
#endif
	return (0);
}
