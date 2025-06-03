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

#ifndef _BUTTONS_H
#define _BUTTONS_H

typedef struct {
	Window 	w;		/* Fenetre associee */
	void	(* f) (); 	/* Callback */
}ButtonWindow;

typedef struct {
	Pixmap		head_pix;
	Pixmap		queue_pix;
	Pixmap		up_pix;
	Pixmap		down_pix;
	Pixmap		np_pix;
	Pixmap		pp_pix;
	GC		mess_gc;
	XFontStruct	*font;
	unsigned long 	fg, bg;
	unsigned long   top_shadow, bot_shadow;
} ResourcesButton;

/*
 * La structure de controle en bas de la fenetre d'edition.
 */
typedef struct {
	Window		frame;
	Window		w_stat;
	int		stat;
	Window		mess;
	Window		w_mode;
	int		mode;
	int             twidth, theight;
	Window		up, down, np, pp;
	Window		head, q;
	char 		s [256];
	GC		mess_gc;
	XFontStruct	*font;
	unsigned long 	fg, bg, red, green;
	unsigned long   top_sh, bot_sh;
} MWin;

/*
 * Public
 */
#define	BUTTON_W	24
#define MW_SPACE	5
#define NB_BUTTONS	6

extern void 	InitControlRes ( /* fg, bg, ts, bs */ );
extern MWin 	*MakeControlPanel ( /* w */ );
extern void	DeleteControlPanel ( /* mwin */ );
extern void	RefreshWindowStatBuf ( /* mwin */ );
extern void	RefreshWindowMode ( /* mwin */ );
extern char 	*GetString ( /* text, prompt, reply */ );
extern void	ShowControlPanel ( /* mwin, width, height */ );
extern void	DisplayMessage ( /* mwin, s */ );
extern void	ClearMessageWindow ( /* mwin */ );
extern void 	ButtonPressInControl ( /* ev, text, i */ );
extern int 	IsButtonInControl ( /* w */ );
extern void 	SetButton ( /* mwin */ );
extern int	ExposeInControlPanel ( /* w, mwin */ );

#define HeightOfMess() ( BUTTON_W + 4 );
#define ExecButtonFunction(text,i) ( bw [i].f ) ( text );

#endif /* _BUTTONS_H_ */
