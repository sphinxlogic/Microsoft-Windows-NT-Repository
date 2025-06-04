/* $Id: rmsio.h,v 1.9 1995/10/21 18:52:08 tom Exp $
 *
 * interface of rmsio.c
 */

#ifndef	RMSIO_H
#define	RMSIO_H

#include	<rms.h>

#define	sys(f)	status = f; if (!$VMS_STATUS_SUCCESS(status))

#define	RMS_STUFF\
	struct	FAB	fab;\
	struct	NAM	nam;\
	unsigned status;\
	char	esa[NAM$C_MAXRSS],	rsa[NAM$C_MAXRSS]

#define	RFILE	struct	_rmsio_file
RFILE	{
	struct	RAB	rab;
	struct	FAB	fab;
	struct	NAM	nam;
	struct	XABFHC	xabfhc;
	char	esa[NAM$C_MAXRSS],	/* expanded by SYS$PARSE	*/
		rsa[NAM$C_MAXRSS];	/* result from SYS$SEARCH	*/
	};

extern	RFILE	*ropen (char *name_, char *mode_);
extern	RFILE	*ropen2 (char *name_, char *dft_, char *mode_);
extern	int	erstat (RFILE *z, char *msg, int msglen);
extern	void	rclear (void);
extern	int	rclose (RFILE *z);
extern	void	rerror (void);
extern	int	rgetr (RFILE * z, char * bfr, int maxbfr, unsigned *mark_);
extern	int	rputr (RFILE *z, char *bfr, int maxbfr);
extern	int	rseek (RFILE *z, int offset, int direction);
extern	int	rsize (RFILE *z);
extern	unsigned rtell (RFILE *z);

#endif /* RMSIO_H */
