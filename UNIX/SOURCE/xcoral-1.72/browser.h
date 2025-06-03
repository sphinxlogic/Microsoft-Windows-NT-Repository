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

#ifndef _BROWSER_H_
#define _BROWSER_H_

#include "text.h"
#include "buffer.h"
#include "scroll.h"

#define B_FONT	"-adobe-times-medium-r-normal--18-180-75-75-p-94-iso8859-1"
#define B_MAP		1
#define B_UNMAP		2
#define B_ADD		5
#define B_DEL		6

#define DEC_MODE	0

#define B_BUTTON_WIDTH 	60
#define B_INTER		10
#define B_SPACE		5

#define W_CLASS		0
#define W_PARENT	1
#define W_CHILD		2
#define W_METHOD	3
#define W_PROC		4
#define W_FILES		5
#define W_VISIT		6

#define C_FIRST_WIN	4
#define B_NB_WIN	7

#define W_CHAP		7
#define W_SECTION	8
#define W_SUBSECTION	9

#define RECURSIVE	11
#define NO_RECURSIVE	12

#define B_BUFFER_SIZE	5000

/*
 * Les Infos des fenetres de texte.
 */
typedef struct {
	Window	frame, title, main;	/* Les fenetres principales */
	int 	f_width, f_height;	/* Les dimensions */
	int	t_width, t_height;
	int	m_width, m_height;
	int 	x, y;	/* La position */
	Text	*text;	/* Les infos sur le texte */
	Buf	*buf;	/* Le buffer */
	SWin	*scroll;/* Les Infos sur le scroll */
	int 	select;	/* Ligne selectionnee */
}BWin;

/*
 * Le browser.
 */
typedef struct {
	int		width, height;	/* Larguer hauteur du browser */
	unsigned long 	fg, bg, text_fg, text_bg;/* Les couleurs */
	Window		frame, title, main;	/* Les fenetres principales */
	char 		*title_name;	/* L'objet selectionne */
	Window		dec,add, del;	/* Les bouttons */
	Window		edit;		/* La fenetre de visite */
	BWin		tbw [7];	/* Les fenetres de texte */
	Buf 		*filebuf;	/* Pour les noms de fichiers */
	GC		gc;		/* Le contexte graphique */
	XFontStruct 	*font;		/* La fonte */
	int		title_height;	/* La hauteur du titre */
	int		button_height;	/* la hauteur des bouttons */
	unsigned long	ts, bs;		/* Les couleurs pour le 3D */
	int 		dec_imp_flag;	/* Toggle flag pour les methodes */
	char 		**methods_save;	/* Sauvegarde des divers pointeurs */
	char		**class_save;
	char		**files_save;
	int 		stat;		/* Browser mappe ou non */
	int		visible;	/* Visibilite */
	int		mode;		/* Le mode */
   	char 		dir [256];	/* La directorie courante */
	int		visit_raise;
	Time		click_time;
	Time		old_click;
} Browser;

/*
 * Public
 */
extern void 	InitBrowser	();
extern void 	ExtractInfos 	( /* char **name, int type */ );
extern void 	ConfigBrowser	( /* int width, int height */ );
extern void 	ConfigVisitWindow ( /* int width, int height */ );
extern void 	DisplayBrowser ();
extern void 	UnmapBrowser ();
extern void 	SetBrowserMode ( /* int mode */ );
extern void 	ExposeBrowser ( /* XEvent *ev );
extern int	ButtonBrowser ( /* XButtonEvent *ev */ );
extern int	KeyPressInBrowser ( /* XKeyEvent *ev */ );
extern void	ConfigScroll ( /* Bwin */ );
extern void	LoadDir ( /* dirname */ );
extern void	SetInfos ();
extern void	RefreshBrowserControl ();
extern void	SetBrowserDir ( /* dirname */ );
extern int	SetBrowserVisibility ( /* event */ );
extern int	GoodSuffix ( /* s */ );
extern void	RefreshBrowserInfos ();

#define GetBrowserFrame()	( br.frame )
#define GetBrowserVisit()	( br.tbw[W_VISIT].frame )

/*
 * Engine
 */
#include "result_types.h"
#include "browser_eng.h"

#endif /* _BROWSER_H_ */
