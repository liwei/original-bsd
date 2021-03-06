/*
 * Copyright (c) 1988, 1989, 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Adam de Boor.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
static char sccsid[] = "@(#)lstPred.c	5.2 (Berkeley) 03/11/90";
#endif /* not lint */

/*-
 * LstPred.c --
 *	Return the predecessor of a given list node
 */

#include	"lstInt.h"

/*-
 *-----------------------------------------------------------------------
 * Lst_Pred --
 *	Return the predecessor of the given node.
 *
 * Results:
 *	The node's predecessor, if any, or NILLNODE if it has none.
 *
 * Side Effects:
 *	None.
 *
 *-----------------------------------------------------------------------
 */
LstNode
Lst_Pred (ln)
    LstNode	ln;
{
    if (ln == NILLNODE) {
	return (NILLNODE);
    } else {
	return ((LstNode)((ListNode) ln)->prevPtr);
    }
}

