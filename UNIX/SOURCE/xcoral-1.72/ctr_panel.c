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

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifndef apollo
#include <malloc.h>
#endif
#include <string.h>

#include "text.h"
#include "buttons.h"
#include "button.bm"
#include "flist.h"

static ButtonWindow	bw [NB_BUTTONS];
static ResourcesButton rs;
extern unsigned long 	PixelValue ();
extern Display *dpy;
static void MakeButtons ();
int	default_mode	= TEXT;

/*
**	Function name : InitControlRes
**
**	Description : Les ressources pour le panel de controle.
**		Creation des Pixmap pour les boutons et l'icone.
**		Creation du contexte graphique pour la fenetre
**		des messages.
**	Input : 
**	Ouput :
*/
void InitControlRes ( fg, bg, ts, bs )
	unsigned long 	fg, bg, ts, bs;
{
	XGCValues 	gcv;
	Window		root;
	int 		depth;

	rs.fg = fg; 
	rs.bg = bg;

	root = DefaultRootWindow ( dpy );
	depth = DefaultDepth ( dpy, DefaultScreen ( dpy ));

	rs.mess_gc = XCreateGC ( dpy, root, 0, &gcv );

	XSetState ( dpy, rs.mess_gc, fg, bg, GXcopy, AllPlanes );
	rs.font = XLoadQueryFont ( dpy,
		"-adobe-courier-bold-r-normal--14-140-75-75-m-90-iso8859-1" );
	if ( rs.font != 0 ) {
		XSetFont ( dpy, rs.mess_gc, rs.font -> fid );
	}
	else {
		(void) fprintf ( stderr, "Font error, used fixed" );
			rs.font = XLoadQueryFont ( dpy, "fixed" );
		XSetFont ( dpy, rs.mess_gc, rs.font -> fid );
	}
	rs.up_pix = XCreatePixmapFromBitmapData ( dpy, root, up_bits,
		BUTTON_W, BUTTON_W, fg, bg, depth );
	rs.down_pix = XCreatePixmapFromBitmapData ( dpy, root, down_bits,
		BUTTON_W, BUTTON_W, fg, bg, depth );
	rs.np_pix = XCreatePixmapFromBitmapData ( dpy, root, np_bits,
		BUTTON_W, BUTTON_W, fg, bg, depth );
	rs.pp_pix = XCreatePixmapFromBitmapData ( dpy, root, pp_bits,
		BUTTON_W, BUTTON_W, fg, bg, depth );
	rs.queue_pix = XCreatePixmapFromBitmapData ( dpy, root, q_bits,
		BUTTON_W, BUTTON_W, fg, bg, depth );
	rs.head_pix = XCreatePixmapFromBitmapData ( dpy, root, head_bits,
		BUTTON_W, BUTTON_W, fg, bg ,depth );

	rs.top_shadow = ts;
	rs.bot_shadow = bs;
}


/*
**	Function name : MakeControlPanel
**
**	Description : Construction de la fenetre des messages
**		et des boutons de scroll up, down, np, pp, debut et fin.
**	Input : La fenetre parent.
**	Ouput :
*/
MWin *MakeControlPanel ( w )
	Window		w;
{
	MWin	*mwin;
	int result;

	mwin = ( MWin * ) malloc ( sizeof ( MWin ));	

	mwin -> frame = XCreateSimpleWindow ( dpy, w, 0,
		0, DEFAULT_SIZE, DEFAULT_SIZE, 0, rs.fg, rs.bg );

	mwin -> w_stat = XCreateSimpleWindow ( dpy, mwin -> frame,
		1, 1, BUTTON_W, BUTTON_W, 0, rs.fg, rs.bg );
	XSelectInput ( dpy, mwin -> w_stat, ExposureMask );

	mwin -> mess = XCreateSimpleWindow ( dpy, mwin -> frame,
		BUTTON_W + 1, 1, DEFAULT_SIZE, DEFAULT_SIZE, 0, rs.fg, rs.bg ); 
	XSelectInput ( dpy, mwin -> mess, ExposureMask | KeyPressMask );

	mwin -> w_mode = XCreateSimpleWindow ( dpy, mwin -> frame, 0,
		1, ( 3 * BUTTON_W ), BUTTON_W, 0, rs.fg, rs.bg );
	XSelectInput ( dpy, mwin -> w_mode, ExposureMask );

	mwin -> mess_gc = rs.mess_gc;
	mwin -> fg = rs.fg;
	mwin -> bg = rs.bg;
	mwin -> font = rs.font;
	mwin -> top_sh = rs.top_shadow;
	mwin -> bot_sh = rs.bot_shadow;
   	mwin -> mode = default_mode;
   	mwin -> stat = False;

	if ( DefaultDepth ( dpy, DefaultScreen ( dpy )) == 1 ) {
		if ( mwin -> bg == BlackPixel ( dpy, DefaultScreen ( dpy ))) {
			mwin -> red = WhitePixel ( dpy, DefaultScreen ( dpy ));
			mwin -> green = BlackPixel ( dpy, DefaultScreen ( dpy ));
		}
		else {
			mwin -> red = BlackPixel ( dpy, DefaultScreen ( dpy ));
			mwin -> green = WhitePixel ( dpy, DefaultScreen ( dpy ));
		}
	}
	else {
		mwin -> red = PixelValue ( dpy, "red", &result ); 
		if ( result == False )
			mwin -> red = BlackPixel ( dpy, DefaultScreen ( dpy ));	
		mwin -> green = PixelValue ( dpy, "green", &result );
		if ( result == False )
			mwin -> green = WhitePixel ( dpy, DefaultScreen ( dpy ));
	}
   	(void) MakeButtons ( mwin, rs.fg, rs.bg ); 
	return ( mwin );
}


/*
**	Function name : RefreshWindowStatBuf
**
**	Description : Met a jour la fenetre qui indique l'etat
**		du buffer ( modifie ou pas ).
**	Input : Le controle panel courant.
**	Ouput :
*/
void RefreshWindowStatBuf ( mwin )
	MWin *mwin;
{
	Display3D ( dpy, mwin -> w_stat, mwin -> top_sh, mwin -> bot_sh, 1, 0 );
	Display3D ( dpy, mwin -> frame, mwin -> top_sh, mwin -> bot_sh, 1, 1 );

	if ( mwin -> stat == False )
		XSetForeground ( dpy, mwin -> mess_gc, mwin -> green );
	else
		XSetForeground ( dpy, mwin -> mess_gc, mwin -> red );

	XDrawString ( dpy, mwin -> w_stat, mwin -> mess_gc, 	8, 16 , "S", 1 ); 
	XSetForeground ( dpy, mwin -> mess_gc, mwin -> fg ); 
}

/*
**	Function name : RefreshWindowMode
**
**	Description : Met a jour la fenetre qui indique le mode
**		courant.
**	Input : Le controle panel courant.
**	Ouput :
*/
void RefreshWindowMode ( mwin )
	MWin *mwin;
{
	char tmp[16];
	register int len;

	bzero (tmp, 16 );	
	XClearWindow ( dpy, mwin -> w_mode );
	Display3D ( dpy, mwin -> w_mode,
		mwin -> top_sh, mwin -> bot_sh, 1, 0 );
	Display3D ( dpy, mwin -> frame,
		mwin -> top_sh, mwin -> bot_sh, 1, 1 );
	switch ( mwin -> mode ) {
		case TEXT:
			(void) strcpy ( tmp, "Text" );
			break;
		case STD_C:
			(void) strcpy ( tmp, "C" );
			break;
		case CPLUS:
			(void) strcpy ( tmp, "C++" );
			break;
		case LATEX:
			(void) strcpy ( tmp, "LaTeX" );
			break;
	}
	if ( (len = strlen ( tmp )) != 0 ) {
		XDrawString ( dpy, mwin -> w_mode, mwin -> mess_gc, 
			((( 3 * BUTTON_W ) - XTextWidth ( mwin -> font, tmp, len )) / 2),
			16 , tmp, len ); 
	}
}


/*
**	Function name : DeleteControlPanel
**
**	Description : Destruction.
**	Input : Le panel de controloe courant.
**	Ouput :
*/
void DeleteControlPanel ( mwin )
	MWin *mwin;
{
	XDestroyWindow ( dpy, mwin -> frame );
   	if ( mwin != 0 )
		free ( (char *) mwin );
}


/*
**	Function name : GetString
**
**	Description : 
**	Input : Le text courant, le prompt et le type de reponse.
**	Ouput : Le chaine
*/
char *GetString ( text, prompt, reply )
Text *text;
register char *prompt;
register char *reply;
{
	return ( (char * ) GetStringFromWindow ( text -> mwin -> mess, 
		text -> mwin -> mess_gc, 
		text -> mwin -> font, prompt, 10, 16, reply, 0 ));
}


/*
**	Function name : MakeButtons
**
**	Description : Fabrication des 6 boutons de scroll.
**	Input : Le panel de controle courant, les couleurs. 
**	Ouput :
*/
static void MakeButtons ( mwin, fg, bg )
	MWin		*mwin;
	unsigned long 	fg, bg;
{
	register int i;
	
	mwin -> np = XCreateSimpleWindow ( dpy, mwin -> frame,
		1, 1, BUTTON_W, BUTTON_W, 0, fg, bg );
	XSetWindowBackgroundPixmap ( dpy, mwin -> np, rs.np_pix );
	bw [0].w = mwin -> np;
	bw [0].f = NextPage;

	mwin -> pp = XCreateSimpleWindow ( dpy, mwin -> frame,
		1, 1, BUTTON_W, BUTTON_W, 0, fg, bg );
	XSetWindowBackgroundPixmap ( dpy, mwin -> pp, rs.pp_pix );
	bw [1].w = mwin -> pp;
	bw [1].f = PreviousPage;

	mwin -> down = XCreateSimpleWindow ( dpy, mwin -> frame,
		1, 1, BUTTON_W, BUTTON_W, 0, fg, bg );
	XSetWindowBackgroundPixmap ( dpy, mwin -> down, rs.down_pix );
	bw [2].w = mwin -> down ;
	bw [2].f = ScrollDownCont;

	mwin -> up = XCreateSimpleWindow ( dpy, mwin -> frame,
		1, 1, BUTTON_W, BUTTON_W, 0, fg, bg );
	XSetWindowBackgroundPixmap ( dpy, mwin -> up, rs.up_pix );
	bw [3].w = mwin -> up;
	bw [3].f = ScrollUpCont;
	
	mwin -> q = XCreateSimpleWindow ( dpy, mwin -> frame,
		1, 1, BUTTON_W, BUTTON_W, 0, fg, bg );
	XSetWindowBackgroundPixmap ( dpy, mwin -> q, rs.queue_pix );
	bw [4].w = mwin -> q;
	bw [4].f = LastPage;

	mwin -> head = XCreateSimpleWindow ( dpy, mwin -> frame,
		1, 1, BUTTON_W, BUTTON_W, 0, fg, bg );
	XSetWindowBackgroundPixmap ( dpy, mwin -> head, rs.head_pix );
	bw [5].w = mwin -> head;
	bw [5].f = FirstPage;
	
	for ( i=0; i<NB_BUTTONS; i++ ) 
		XSelectInput ( dpy, bw [i].w, ExposureMask
			| ButtonPressMask | ButtonReleaseMask );
}


/*
**	Function name : ShowControlPanel
**
**	Description : Affiche le controle panel.
**		fenetres : stat_buf, message, mode, bouttons.
**	Input : Le panel de controle courant.
**	Ouput : Largeur et hauteur du parent.
*/
void ShowControlPanel ( mwin, width, height )
	MWin	*mwin;
	int	width, height;
{
	register int width_mess;
   	register int width_frame = width - ( 2 * MW_SPACE );
	
	width_mess = width_frame - ( 10 * BUTTON_W ) - 2; 

	mwin -> twidth = width_mess;
	mwin -> theight = BUTTON_W;

	XMoveResizeWindow ( dpy, mwin -> frame, MW_SPACE ,
		height - BUTTON_W  - MW_SPACE,
		width_frame, BUTTON_W + 2 );

	XResizeWindow ( dpy, mwin -> mess,
		width_mess , BUTTON_W );

	XMoveWindow ( dpy, mwin -> w_mode, 
		width_frame - ( 9 * BUTTON_W ) - 1, 1 );

	XMoveWindow ( dpy, mwin -> head,
		width_frame- ( 6 * BUTTON_W ) - 1, 1 );

	XMoveWindow ( dpy, mwin -> q,
		width_frame - ( 5 * BUTTON_W )  - 1, 1 );

	XMoveWindow ( dpy, mwin -> up,
		width_frame - ( 4 * BUTTON_W ) - 1, 1 );

	XMoveWindow ( dpy, mwin -> down, 
		width_frame - ( 3 * BUTTON_W ) - 1, 1 );

	XMoveWindow ( dpy, mwin -> pp, 
		width_frame - ( 2 * BUTTON_W ) - 1, 1 );
	
	XMoveWindow ( dpy, mwin -> np, 
		width_frame - BUTTON_W - 1, 1 );

	XMapSubwindows ( dpy, mwin -> frame ); 
	XMapWindow ( dpy, mwin -> frame );
}


/*
**	Function name : DisplayMessage
**
**	Description : Affiche un message.
**	Input : Le panel de controle courant, le message.
**	Ouput :
*/
void DisplayMessage ( mwin, s ) 
	MWin *mwin;
	register char *s;
{
	if ( (s == 0) || (mwin == 0) )
		return;

#ifdef DEBUG
	(void) fprintf ( stderr, "window mess id = %d\n", mwin -> mess );
#endif

	XClearArea ( dpy, mwin -> mess, 2, 2, mwin -> twidth - 4,
		    mwin -> theight - 4, False );

	XDrawString ( dpy, mwin -> mess,
		mwin -> mess_gc, 10, 16, s, strlen (s) );

}


/*
**	Function name : ClearMessageWIndow
**
**	Description : Comme son nom l'indique.
**	Input : Le panel de controle courant.
**	Ouput :
*/
void ClearMessageWindow ( mwin )
	MWin *mwin;
{
	DisplayMessage ( mwin, " ");
}


/*
**	Function name : ButtonPressInControl
**
**	Description : Gere le look des bouttons sur un 'ButtonPress'
**	Input : L'evennement, le text courant, le no du boutton.
**	Ouput :
*/
void ButtonPressInControl ( ev, text, i )
	XEvent *ev;
	Text *text;
	register int i;
{

       	static unsigned int button_press;
	
	button_press = ev -> xbutton.button;
  
	Display3D ( dpy, ev -> xbutton.window,
		rs.top_shadow, rs.bot_shadow, 1, 1 );
   
	XGrabPointer ( dpy, ev -> xbutton.window, True,
		ButtonPressMask, GrabModeAsync,GrabModeAsync,
		ev -> xbutton.window, None, CurrentTime );

	ExecButtonFunction ( text, i );

	if ( (i==2) || (i==3) ) { 
		Display3D ( dpy, ev -> xbutton.window,
			rs.top_shadow, rs.bot_shadow, 1, 0 );
      		XUngrabPointer ( dpy, CurrentTime );
		return;
	}

	WaitButtonRelease ( button_press );
	Display3D ( dpy, ev -> xbutton.window,
		rs.top_shadow, rs.bot_shadow, 1, 0 );
}


/*
**	Function name : ButtonPressInControl
**
**	Description : Recherche quel bouton a ete enfonce.
**	Input : La fenetre associe
**	Ouput : Le numero du bouton ou -1 si problemes.
*/
int IsButtonInControl ( w )
	Window w;
{
	register int i;
	
	for ( i=0; i < NB_BUTTONS; i++ ) {
		if ( w == bw [i].w ) {
			return i;
		}
	}
	return ( -1 );
}


/*
**	Function name : SetButton
**
**	Description : Met a jour la table des boutons.
**	Input : Le panel de controle courant.
**	Ouput :
*/
void SetButton ( mwin )
	MWin *mwin;
{
	bw [0].w = mwin -> np;
	bw [1].w = mwin -> pp;
	bw [2].w = mwin -> down;
	bw [3].w = mwin -> up;
	bw [4].w = mwin -> q;
	bw [5].w = mwin -> head;
}


/*
**	Function name : ExposeControlePanel
**
**	Description : Traitement d'un expose event dans le panel.
**	Input : La fenetre exposee, le panel courant.
**	Output: Vrai si exposition sur le panel, faux sinon.
*/
int ExposeInControlePanel ( w, mwin )
	Window w;
	MWin *mwin;
{
   	XEvent ev;
   
	if ( w == mwin -> up
		|| w == mwin -> down
		|| w == mwin -> np
		|| w == mwin -> pp
		|| w == mwin -> q
		|| w == mwin -> head ) {

#ifdef DEBUG
	(void) fprintf ( stderr, "Button\n" );	    
#endif
		Display3D ( dpy, w, mwin -> top_sh, mwin -> bot_sh, 1, 0 );
		Display3D ( dpy, mwin -> frame, mwin -> top_sh, mwin -> bot_sh, 1, 1 );
		while ( XCheckWindowEvent ( dpy, w, ExposureMask, &ev ));
		return True;
	}
	if ( w == mwin -> w_stat ) {
		RefreshWindowStatBuf ( mwin );
		while ( XCheckWindowEvent ( dpy, w, ExposureMask, &ev ));
		return True;
	}
	if ( w == mwin -> w_mode ) {
		RefreshWindowMode ( mwin );
		while ( XCheckWindowEvent ( dpy, w, ExposureMask, &ev ));
		return True;
	}
	if ( w == mwin -> mess ) {
#ifdef DEBUG
     (void) fprintf ( stderr, "Refresh Message\n");
#endif
		Display3D ( dpy, w, mwin -> top_sh, mwin -> bot_sh, 1, 0 );
		Display3D ( dpy, mwin -> frame, mwin -> top_sh, mwin -> bot_sh, 1, 1 );
		ClearMessageWindow ( mwin );
		while ( XCheckWindowEvent ( dpy, w, ExposureMask, &ev ));
		return True;
	}
	return False;
}
