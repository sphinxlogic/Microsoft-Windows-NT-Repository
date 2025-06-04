/* $Id: rmsinit.h,v 1.2 1995/06/06 13:06:58 tom Exp $
 *
 * interface of rmsinit.c
 */

#ifndef RMSINIT_H
#define RMSINIT_H

#include <rms.h>

extern	void	rmsinit_fab( struct FAB *fab_, struct NAM *nam_, char *dna_, char *fna_);
extern	void	rmsinit_nam ( struct NAM *nam_, char *rsa_, char *esa_);

#endif /* RMSINIT_H */
