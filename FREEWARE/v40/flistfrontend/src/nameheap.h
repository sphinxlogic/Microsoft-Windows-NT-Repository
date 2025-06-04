/* $Id: nameheap.h,v 1.3 1995/06/04 23:05:16 tom Exp $
 *
 * public interface of nameheap.c
 */

#ifndef	NAMEHEAP_H
#define	NAMEHEAP_H

/* FIXME: void => TEXTLINK */

extern	char *	nameheap (char *s_, int len, void **heap);
extern	void	nameheap_set (int new_refs);
extern	int	nameheap_ref (void);
extern	void	nameheap_clr (int old_refs, void **heap);
extern	void	nameheap_add (int refs, char *text);

#ifdef	DEBUG1
extern	void	nameheap_dump (char *tag, void **heap);
#endif

#endif	/* NAMEHEAP_H */
