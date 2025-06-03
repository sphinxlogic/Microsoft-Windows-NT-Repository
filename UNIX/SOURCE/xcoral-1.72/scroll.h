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

#ifndef _SCROLL_H_
#define _SCROLL_H_

#define SCROLL_WIDTH 16
#define DEFAULT_SIZE 10

#define UP 0
#define DOWN 1
#define TOP 2
#define BOTTOM 3
#define OTHER 4
#define FIRST 5
#define LAST 6
#define NEXT 7
#define PREVIOUS 8
#define CURRENT 9
#define CURSOR 10

typedef struct {
	Window	frame;		/* Main window */
	Window	scroll;		/* Le curseur */
	unsigned long fg, bg;	/* Les couleurs */
	int width;		/* Largeur totale */
	int f_height;		/* geometrie frame et scroll */
	int s_height;
	int fx, fy, sx, sy;	
	XFontStruct *font;	/* La fonte utilisee pour le texte a scroller */
	int line;		/* Infos sur le nb de lignes a scroller */
	int linepage;  		
	int line_to_scroll;
	double	delta;		/* Pour les calculs */
	int y_min, y_max;    
	int last_dir;		/* Derniere direction pour la scrollbar */
	int last_y;     		  	
	double	rest;		/* La fraction de ligne restant a scroller */
	char	*text;		/* Vers les infos sur le texte */
} SWin;

/*
 *	Public
 */
extern void	InitScroll ( /* display */ );
extern SWin 	*MakeScroll ( /* display, parent, x, y */ );
extern void	SetScrollLine ( /*swin, n */ );
extern void	RefreshScroll ( /* display, swin, width, height, n*/ );
extern void	ShowScrollFrame ( /* display, swin */ );
extern int 	ButtonPressInScroll ( /* swin, w, y */ );
extern void	HandleScrollBar ( /* display, swin, f */ );
extern int		MoveScrollBar ( /* display, swin, flag, dy */ );
extern void	RefreshScrollBar ( /* display, s */ );
extern void	SetScrollBarSize ( /* display, swin */ );
extern void	RefreshScrollFrame ( /* display, s */ );
extern void	DeleteScroll ( /* display, swin */ );
extern int	ExposeInScroll ( /* display, w, s */ );

#define SetScrollFont(s, f)	(s -> font = f)
#define SetScrollLinePage(s, n)	(s -> linepage = n)
#define GetScrollWidth()	(SCROLL_WIDTH)

#endif /* _SCROLL_H_ */

