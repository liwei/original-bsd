/*-
 * Copyright (c) 1991 The Regents of the University of California.
 * All rights reserved.
 *
 * %sccs.include.proprietary.c%
 */

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1991 The Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)spellout.c	4.3 (Berkeley) 04/18/91";
#endif /* not lint */

#include "spell.h"

main(argc, argv)
int argc;
char **argv;
{
	register i, j;
	long h;
	register long *lp;
	char word[NW];
	int dflag = 0;
	int indict;
	register char *wp;

	if (argc>1 && argv[1][0]=='-' && argv[1][1]=='d') {
		dflag = 1;
		argc--;
		argv++;
	}
	if(argc<=1) {
		fprintf(stderr,"spellout: arg count\n");
		exit(1);
	}
	if(!prime(argc,argv)) {
		fprintf(stderr,
		    "spellout: cannot initialize hash table\n");
		exit(1);
	}
	while (fgets(word, sizeof(word), stdin)) {
		indict = 1;
		for (i=0; i<NP; i++) {
			for (wp = word, h = 0, lp = pow2[i];
				(j = *wp) != '\0'; ++wp, ++lp)
				h += j * *lp;
			h %= p[i];
			if (get(h)==0) {
				indict = 0;
				break;
			}
		}
		if (dflag == indict)
			fputs(word, stdout);
	}
}
