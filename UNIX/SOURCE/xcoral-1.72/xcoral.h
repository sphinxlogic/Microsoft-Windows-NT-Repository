/*
** Copyright 1989, 1992 by Lionel Fournigault
**
** Permission to use, copy, and distribute for non-commercial purposes,
** is hereby granted without fee, providing that the above copyright
** notice appear in all copies and that both the copyright notice and this
** permission notice appear in supporting documentation.
** The software may be modified for your own purposes, but modified versions
** may not be distributed.
** This software is provided "as is" without any expressed or implied warranty.
**
**
*/

#ifndef  _XCORAL_H_
#define  _XCORAL_H_

#include "text.h"
#include "m_key.h"
#include "menus.h"

/*
 *	La structure associe a chaque fenetre d'edition.
 */
typedef struct {
	Window		w_frame;	/* La fenetre mere */
	int		width, height;	/* Geometrie */
	Text		*text;		/* La structure Text */
	Buf		*buf;		/* Le buffer */
	ST		*stat;		/* Etat de l'automate */
	MWin		*mwin;		/* La fenetre pour les messages */
	SWin		*scroll;	/* La fenetre de scroll */	
	XYMenu		*menu;		/* Les menus XY */
	XSizeHints	*shints;	/* Les options */
} EdWin;

extern Display	*dpy;		/* Le display */
extern EdWin	*edwin;		/* La fenetre courante */
extern EdWin	*TWin [];		/* La table de fenetres */
extern XContext	EdContext;	/* Le contexte courant */

extern char 	*menu_names [];
extern char 	**item_names [];	
extern void		(**func_names []) ();	/* Callback */

#define	MAXWIN	16	/* Nb max de fenetres d'editions. 32 ou 64 si on veut */
#define CURRENT_VERSION	"xcoral version 1.72 "

#endif /* _XCORAL_H_ */
