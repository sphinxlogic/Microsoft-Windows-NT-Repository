/* $Id: getprot.h,v 1.4 1995/06/04 21:47:28 tom Exp $
 * 
 * public interface of 'getprot.c', 'setprot.c'
 */

#ifndef GETPROT_H
#define GETPROT_H

typedef	struct	{
	unsigned short	p_mask;		/* protection mask	*/
	unsigned short	p_grp,		/* group code		*/
			p_mbm;		/* member code		*/
	} GETPROT;

extern	int	getprot (GETPROT *ret_, char *name_);
extern	int	setprot (char *filespec, char *prot_code);
extern	int	cmpprot (GETPROT *pr_, char *mode);

#endif	/* GETPROT_H */
