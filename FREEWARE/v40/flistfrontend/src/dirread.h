/* $Id: dirread.h,v 1.1 1995/06/04 21:40:00 tom Exp $
 *
 * interface of dirread.c
 */

extern	void	dirread_init (void);
extern	void	dirread_put (struct FAB *fab_);
extern	char*	dirread_get (char *filespec, int inx);
extern	void	dirread_free (void);
extern	char*	dirread_path (int *inx_);

#ifdef	DEBUG
extern	void	dirread_show (void);
#endif
