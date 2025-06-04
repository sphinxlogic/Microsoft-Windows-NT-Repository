/* $Id: textlink.h,v 1.4 1995/06/06 00:53:22 tom Exp $
 *
 * Title:	textlink.h
 * Author:	Thomas E. Dickey
 * Created:	06 Dec 1984
 * Last update:	04 Jul 1985, added 'refs' mask
 *		06 Dec 1984
 *
 *	Define a structure of linked-lists with text.
 */

#ifndef	TEXTLINK_H
#define	TEXTLINK_H

#define	TEXTLINK	struct _textlink

TEXTLINK {
	TEXTLINK *	next;		/* => next link in list		*/
	unsigned char	refs;		/* bitmask to track reference	*/
		char	text[1];	/* string, with trailing null	*/
	};

#endif/*TEXTLINK_H*/
