/*
 * Copyright (c) 1982, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * %sccs.include.redist.c%
 */

#ifndef lint
static char sccsid[] = "@(#)roll.c	8.1 (Berkeley) 05/31/93";
#endif /* not lint */

# include	"mille.h"

/*
 *	This routine rolls ndie nside-sided dice.
 *
 * @(#)roll.c	1.1 (Berkeley) 4/1/82
 *
 */

roll(ndie, nsides)
reg int	ndie, nsides; {

	reg int			tot;
	extern unsigned int	random();

	tot = 0;
	while (ndie--)
		tot += random() % nsides + 1;
	return tot;
}
