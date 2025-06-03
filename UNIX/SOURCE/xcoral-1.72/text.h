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

#ifndef  _TEXT_H_
#define  _TEXT_H_

#include "buffer.h"
#include "scroll.h"
#include "buttons.h"

/*
 *	Le ressources pour les fenetres Texte.
 */
typedef struct {
	GC	cgc;	/* Contexte graphique standard */
	GC	igc;	/* Contexte graphique inverse */
	unsigned long fg, bg, top_sh, bot_sh; /* Les couleurs */
	XFontStruct *font;	/* La fonte */
} TextResources;

/*
 *	Toutes les infos sur le texte.
 */
typedef struct {
	Window window;		/* La fenetre de texte */
	Window w_parent;	/* La fenetre parent */
	int width, height;	/* Les dimensions */
	int lines_in_buf;	/* Nombre de lignes dans le buffer */
	int no_current_line;	/* Numero de la ligne courante */
	int lines_in_page;	/* Nombre de ligne dans la page */
	int n1, n2;		/* Nb de ligne avant/apres le curseur */
	XFontStruct *font;	/* La fonte utilisee */
	int char_width_ave;	/* Largeur moyenne d'un caractere */
	int font_height;	/* Hauteur de la fonte en pixels */
	GC Cgc;			/* Le context graphique courant */
	GC Igc;			/* Le context graphique pour l'inverse */
	unsigned long fg, bg;	/* Le devant et le fond */
	unsigned long   top_sh, bot_sh; /* Shadow */
	int x_or, y_or;	/* Origine	 */
	int x_pos, y_pos;	/* Position courante dans la fenetre */
	int cursor_height;	/* Hauteur du curseur */
	int cursor_width;	/* Largeur moyenne du curseur */
	int cursor_stat;	/* Etat du curseur */
	int mouse_in;		/* Mouse in the window */
	int sl;			/* Horizontal Scroll */

	char filename [256];
	char current_dir [256];
	int modif;
	int stat;
	int visible;
	
	Buf *buf;	/* Le Buffer */
	SWin *swin;	/* Le scroll */
	MWin *mwin; 	/* La fenetre des messages et bouttons */
	int width_relief;

	struct {
	       int      wline [256];	/* La largeur des lignes */
	       char     *sline [256];	/* La table des lignes */
	} page;

	int markline;	/* Le numero de la ligne */
	int markpos;	/* La position dans la ligne */
	
	int mode;
	char indent[8];
	int tab_width;
} Text;

#define GetTextWindow(text) 	( text -> window )
#define GetModif(text)		( text -> modif )
#define GetFileName(text)	( text -> filename )
#define SetLineInPage(text,n) 	( text -> lines_in_page = n )
#define GetMode(text)		( text -> mode )
#define GetCursorStat(text)	( text -> cursor_stat )
#define GetScrollStat(text)	( text -> sl )
#define GetNbLinesInBuf(text)	( text -> lines_in_buf )

/*
 *	Public
 */
extern void 	InitTextRes ( /* display, font, fg, bg, ts, bs */ );
extern Text	*MakeTextWindow ( /* display, parent, x, y */ );
extern void	DeleteText ( /* display, text */ );
extern int	GetVisibility ( /* display, text */ );
extern void	KillText ( /* display, text */ );
extern void 	SetFontText ( /*display, text, font */ );
extern void 	ChangeTextFont ( /* display, text, f */ );
extern void 	ShowWindowText ( /* display, text,width, height */ );
extern void	ExposeTextWindow ( /* display,text */ );

extern void	TextCursorOn ( /* text */ );
extern void	TextCursorOff ( /* text */ );
extern void	FreeseTextCursor ( /* text */ );
extern void	UnFreeseTextCursor ( /* text */ );

extern int	MoveToXYinTextWindow ( /* text, x, y */ );
extern int 	SetTextVisibility ( /* text, state */ );
extern int 	GetLineInPage ( /* text, font */ );
extern int 	TextInBuf ( /* text */ );
extern void	MouseIn ( /* text */ );
extern void	MouseOut ( /* text */ );
extern void	FirstPage ( /* text */ );
extern void	ClearPage ( /* text */ );
extern void	ClipOn ( /* text, line */ );
extern void	ClipOff ( /* text */ );
extern void	ExposePage ( /* r, text */ );
extern void	SetLinesTable ( /* text */ );
extern void	SetCurrentLine ( /* text */ );
extern void	RefreshPage ( /* text */ );
extern void	ClearLine ( /* text, i */ );
extern void	LastPage ( /* text */ );
extern void	PreviousPage ( /* text */ );
extern void 	NextPage ( /* text */ );
extern void	GotoEnd ( /* text */ );
extern void	CurrentLineToTop ( /* text */ );
extern void	GotoLineNumber ( /* text, n */ );
extern void	ScrollNLine ( /* n, text */ );
extern void	CurrentLineToMiddle ( /* text */ );
extern void	SetAndDisplayPage ( /* text */ );
extern void	ScrollDownCont ( /* text */ );
extern void	ScrollUpCont ( /* text */ );
extern void	SetMark ( /* text */ );
extern void	ExchangePointMark ( /* text */ );
extern void	KillRegion ( /* text */ );
extern void	CopyRegion ( /* text */ );
extern void	PasteRegion ( /* text */ );
extern void	ForwardSearch ( /* text */ );
extern void	BackwardSearch ( /* text */ );
extern void	QueryReplace ( /* text */ );
extern void	GlobalReplace ( /* text */ );
extern void	GotoPos ( /* text, start, newpos, dir */ );
extern void	ResetSearchString ();

extern void	MoveToBline ( /* text */ );
extern void	MoveToEline ( /* text */ );
extern void	DownCursor ( /* text */ );
extern void	UpCursor ( /* text */ );
extern int	ForwardChar ( /* text */ );
extern void	BackwardChar ( /* text */ );
extern char	*HscrollString ( /* s, n, tabwidth */ );
extern void	GotoLeft ( /* text */ );

extern void	KillCurrentBuffer ( /* text */ );
extern int	LoadFile ( /* text, s, type */ );
extern int	SaveCurrentBuffer ( /* text, from */ );
extern void	SetDirAndFilename ( /* text, name */ );
extern void	ChangeDir ( /* text */ );

extern void	f_impc ( /* text, c */ );
extern void	f_return ( /* text */ );
extern void	f_tab ( /* text */ );
extern void	f_delete ( /* text */ );
extern void	Control_D ( /* text */ );
extern void	Control_K ( /* text, n */ );
extern void	Control_Y ( /* text, i */ );
extern void	GetBytesFromCutBuffer ( /* text */ );
extern void	StoreBytesInCutBuffer ( /* text, x, y */ );
extern void	DeleteBytesFromCutBuffer ( /* text */ );
extern void	SetTextMode ( /* text */ );
extern void	SetCMode ( /* text */ );
extern void	SetCCMode ( /* text */ );

#define TAB_WIDTH	8
#define INDENT_WIDTH	3
#define	OFF		0
#define ON              1

#define	MARGE	10
#define W_SPACE 5

#define W_RELIEF	2
#define FREESE		2
#define UNFREESE	3
#define MSET(x,y)	( x |= y )
#define MCLEAR(x,y)	( x &= (~y) )
#define MESSAGE		( 1 << 0 )
#define FIRSTPAGE	( 1 << 1 )
#define LASTPAGE	( 1 << 2 )

#define END_MESS "This is the end... my friend the end"

#define STD_C	1
#define CPLUS	2
#define TEXT	3	/* default */
#define LATEX	4

extern int default_mode;

#define F_KEY	0
#define F_MENU	1

#define FILESELECT	0	
#define KILLBUF		1
#define OPENFILES	2

#endif /* _TEXT_H_ */





