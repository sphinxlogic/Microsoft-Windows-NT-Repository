/* $Id: flist.h,v 1.7 1995/06/06 13:13:06 tom Exp $
 *
 * Title:	flist.h - misc
 * Author:	Thomas E. Dickey
 * Created:	04 May 1984
 * Last update:
 *		28 May 1995, prototypes for functions in fl.c
 *		04 May 1985, removed qCMD (restructuring), sCMD
 *		28 Aug 1984, added 'qCMD()'
 *		24 Aug 1984, broke out 'crt.h'
 *		14 Aug 1984, moved DELETED to 'dirent.h'
 *		20 Jun 1984, added 'is_sCMD()'
 *		21 May 1984
 */

#ifndef FLIST_H
#define FLIST_H

#include	"bool.h"
#include	"crt.h"
#include	"warning.h"

#define	CTL(c)	(037 & c)
#define	is_PAD(c)	(c >= 256 && c < 512)

extern	void	clrbeep (void);
extern	void	clrwarn (void);
extern	int	didbeep (void);
extern	int	didwarn (void);
extern	void	fledit_pack (void);
extern	void	flist_chdir (char *path);
extern	void	flist_date (int curfile, int opt);
extern	void	flist_date2 (int curfile);
extern	void	flist_help (int curfile, char *key);
extern	int	flist_hold (void);
extern	void	flist_info (char *format, ...);
extern	char*	flist_lis (char *format, ...);
extern	void	flist_log (char *format, ...);
extern	void	flist_move (int *curfile_, int ref, int code);
extern	int	flist_nest (void);
extern	void	flist_quit (int status);
extern	int	flist_sysmsg (unsigned status);
extern	void	flist_tell (char *format, ...);
extern	void	set_beep (void);
extern	void	warn2 (char *format, ...);

#ifdef $DESCRIPTOR
extern	int	flist_tell_dsc (struct dsc$descriptor_s *ds_);
#endif

#ifdef	DCLARG_H
extern	void	flist (DCLARG *dcl_);
extern	int	flist_opts (int argc, char **argv, DCLARG *arg_, int subset);
#endif

#endif /* FLIST_H */
