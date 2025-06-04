/* $Id: dirfind.h,v 1.2 1995/06/06 00:47:44 tom Exp $
 *
 * interface of dirfind.c
 */

#ifndef	DIRFIND_H
#define	DIRFIND_H

#include "dclarg.h"	/* for DCLARG */
#include "dirent.h"	/* for FILENT */

extern	int	dirfind (int curfile, int forward, DCLARG *find_spec, void (*each)(int, int *), int must_find, int unfind);
extern	void	dirfind_chop (DCLARG *find_spec, FILENT *fz_, PATHNT *pz_, char *Fpath, char *Fname, char *Ftype);
extern	int	dirfind_nxt (int j, int forward);
extern	int	dirfind_notexp (DCLARG *find_spec);
extern	int	dirfind_tst (FILENT *z, FILENT *zref, int lookup);

#endif /* DIRFIND_H */
