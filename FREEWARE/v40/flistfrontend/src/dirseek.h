/* $Id: dirseek.h,v 1.3 1995/10/21 18:41:28 tom Exp $
 *
 * interface of dirseek.c
 */

#ifndef DIRSEEK_H
#define DIRSEEK_H

#include "dclarg.h"

#define	tDIRSEEK \
	void	(*for_each)(char *buf, int len, unsigned status)

extern	int	dirseek ( char *cmd_, DCLARG *d_, int mfld, int need);
extern	int	dirseek_spec (DCLARG *spec, int implicit, tDIRSEEK);
extern	int	dirseek_spec2 (char *spec, tDIRSEEK);

#endif /* DIRSEEK_H */
