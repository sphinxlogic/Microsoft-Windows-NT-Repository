/* $Id: dirdata.h,v 1.2 1995/05/28 21:38:48 tom Exp $
 *
 * interface of dirdata.c
 */

#ifndef DIRDATA_H
#define DIRDATA_H

#include "dirent.h"

extern	void	dirdata_add (FILENT *z, FLINK **lastp);
extern	void	dirdata_chg (FILENT *z, FLINK **lastp, int found);
extern	int	dirdata_find (FILENT *z, FLINK **lastp);
extern	int	dirdata_high (FILENT *z, FLINK **lastp);
extern	void	dirdata_one (FILENT *z, FLINK **lastp);
extern	void	dirdata_ren (FILENT *znew, FILENT *zold);

#endif /* DIRDATA_H */
