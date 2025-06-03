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
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#ifndef apollo
#include <malloc.h>
#endif
#include "text.h"
#include "browser.h"

static TextResources	tr;
extern char *getenv ();
extern void Display3D ();

/*
**	Function name : InitTextRes
**
**	Description : Initialisations de resources communes
**		aux fenetres de texte : la fonte et les couleurs.
**
**	Input : Le display, la fonte, les couleur du devant, du fond,
**		du top_shadow et du buttom_shadow.
**	Ouput :
*/
void InitTextRes ( display, font, fg, bg, ts, bs )
	Display		*display;
	XFontStruct	*font;
	unsigned long 	fg, bg, ts, bs;
{
	XGCValues	gcv;
	GC		gc;
	unsigned long 	gcm;

	gc = DefaultGC ( display, DefaultScreen ( display ) );

	tr.top_sh = ts;
	tr.bot_sh = bs;
	tr.cgc = XCreateGC ( display, 	DefaultRootWindow ( display ), 0, &gcv );
	tr.igc = XCreateGC ( display,	DefaultRootWindow ( display ), 0, &gcv );
	tr.font = font;
	tr.fg = fg;
	tr.bg = bg;
			
	XCopyGC ( display, gc, (~0), tr.cgc );
	XCopyGC ( display, gc, (~0), tr.igc );
	
	gcm = 0;
	gcm |= GCForeground;	gcv.foreground = fg;
	gcm |= GCBackground;	gcv.background = bg;
	gcm |= GCFont;		gcv.font = font -> fid;

	XChangeGC ( display, tr.cgc, gcm, &gcv );
	
	gcm = 0;
	gcm |= GCFunction;	gcv.function = GXxor;
	gcm |= GCPlaneMask;	gcv.plane_mask = fg ^ bg;
	gcm |= GCForeground;	gcv.foreground = fg ^ bg;
	gcm |= GCBackground;	gcv.background = bg;

	XChangeGC ( display, tr.igc, gcm, &gcv );
}


/*
**	Function name : MakeTextWindow
**
**	Description : Creation d'un fenetre de texte.
**		
**	Input : Le display, la fenetre parent, la position par rapport
**		a celle-ci.
**	Ouput : La structure Text.
*/
Text *MakeTextWindow ( display, parent, x, y )
	Display	*display;
	Window	parent;
	register int x, y;
{
	Text		*text;
	XGCValues	gcv;
	register char*	indent;
	register int 	tw;

	text = ( Text * ) malloc ( sizeof ( Text ));

	text -> width_relief = W_RELIEF;
	text -> w_parent = parent;

	if ( DefaultDepth ( display, DefaultScreen ( display )) == 1 ) 
		text -> width_relief = 1;	       
	
	text -> window = XCreateSimpleWindow (display, parent, x,
		y, DEFAULT_SIZE, DEFAULT_SIZE, 0, tr.fg, tr.bg );

	XSelectInput ( display, text -> window,
		ExposureMask | KeyPressMask | VisibilityChangeMask |
		      KeyReleaseMask | ButtonPress | ButtonRelease );

	text -> Cgc = XCreateGC ( display, DefaultRootWindow ( display ), 0, &gcv );
	XCopyGC ( display, tr.cgc, (~0), text -> Cgc );

	text -> Igc = tr.igc;
	text -> top_sh = tr.top_sh;
	text -> bot_sh = tr.bot_sh;
	text -> lines_in_buf = 1;
	text -> no_current_line = 1;
	text -> n1 = 0;

	text -> fg = tr.fg;
	text -> bg = tr.bg;

	text -> x_or = text -> y_or = MARGE;
	text -> x_pos = text -> y_pos = MARGE;
	text -> visible = 0;

	SetFontText ( display, text, tr.font );

	text -> cursor_stat = OFF;
	text -> cursor_width = text -> char_width_ave;
	text -> cursor_height = text -> font_height;
	text -> sl = 0;
	*text -> filename = 0;
   	text -> stat = 0;
	text -> modif = False;
	text -> mouse_in = False;
	
	text -> mode = default_mode;

	indent = (char *) getenv ( "XCORAL_INDENT_WIDTH" );
	(void) strcpy ( text -> indent, "       " ); /* 8 blancs */
	if ( indent == 0 )
		text -> indent [3] = 0;
	else {
		tw = atoi (indent);
		if ( tw == -1 )
			strcpy(text->indent,"\t");
		else if ( tw <= 0 || tw > 7 )
			text -> indent [3] = 0;
		else
			text -> indent [tw] = 0;
	}

	indent = (char *) getenv ( "XCORAL_TAB_WIDTH" );
	if ( indent == 0 ) 
		text -> tab_width = TAB_WIDTH;
	else {
		tw = atoi (indent); 
		text -> tab_width = ( tw <= 0 || tw > 7 ) ? TAB_WIDTH : tw; 
	}
	
	(void) bzero ( (char *) text -> page.wline, 256 );
	(void) bzero ( (char *) text -> page.sline, 256 );

	text -> markline = text -> markpos = 0;
	return text;
}


/*
**	Function name : DeleteText
**
**	Description : Destruction d'une fenetre de texte.
**
**	Input : Le Display , la structure text.
**	Ouput :
*/
void DeleteText ( display, text )
	Display *display;
	Text *text;
{
	XFreeGC ( display, text -> Cgc );
	XDestroyWindow ( display, text -> window );
   	if ( text != 0 ) 
		(void) free ( (char *) text );
}


/*
**	Function name : SetTextVisibility
**
**	Description : Positionne l'etat de visibilite.
**
**	Input : Le text courant, l'etat.
**	Ouput :
*/
int SetTextVisibility ( w, text, state )
	Window w;
	Text *text;
	register int state;
{
   	if ( w != text -> window )
   		return False;
#ifdef DEBUG
	fprintf ( stderr, "Text  window stat = %d\n", state ); 
#endif
	text -> visible = state;
	return True;
}


/*
**	Function name : GetVisibility
**
**	Description : Retourne l'etat de la visibilite.
**
**	Input : Le Display, le text courant.
**	Ouput : Vrai ou Faux
*/
int GetVisibility ( display, text )
	Display *display;
	Text *text;
{
    	if ( text -> visible != VisibilityUnobscured ) {
		XRaiseWindow ( display, text -> w_parent );
		return False;
	}
	return True;
}


/*
**	Function name : KillText
**
**	Description : Vire le texte d'une fenetre texte et met
**		a jours les infos.
**
**	Input : Le display, le text courant.
**	Ouput :
*/
void KillText ( display, text )
	Display *display;
	Text *text;
{
	/*
	 * Affiche la premiere page pour positionner 
	 * certaines variables.
	 * Reset le buffer
	 */
   	FirstPage ( text );
	ClearBuffer ( text -> buf );

	/* 
	 * Mis a jour des infos.
	 */
	text -> lines_in_buf = 1; 
   	text -> modif = False;
   	text -> no_current_line = 1;
	(void) bzero ( (char *) text -> page.wline, 256 );
	(void) bzero ( (char *) text -> page.sline, 256 );

	/* 
	 * On nettoie bien
	 */
	SetScrollLine ( text -> swin, 1 );
	XClearWindow ( display, text -> window );
	Display3D ( display, text -> window, text -> top_sh, text ->bot_sh, 2, 1 ); 
}


/*
**	Function name : TextInBuf
**
**	Description : Comme son nom l'indique
**	Input : Le text courant
**	Ouput : Vrai ou faux
*/
int TextInBuf ( text )
	Text *text;
{
	if ( (! strcmp ( text -> filename, "NoName" )) && (text -> modif == False)) 
		return False;
	else 
		return True;
}


/*
**	Function name : SetFontText
**
**	Description : Positionne la fonte pour le texte courant.
**
**	Input :  Le display, le text courant, la fonte.
**	Ouput :
*/
void SetFontText ( display, text, font )
	Display *display;
	Text *text;
	XFontStruct *font;
{
	text -> font = font;
	text -> font_height = (font -> max_bounds.ascent) + (font ->max_bounds.descent);
	text -> cursor_height = text -> font_height;

	text -> cursor_width = ( font -> min_bounds.width + font -> max_bounds.width ) / 2;
	text -> char_width_ave = ( font -> min_bounds.width + font -> max_bounds.width ) / 2;

	XSetFont ( display, text -> Cgc, font -> fid );
}


/*
**	Function name : LoadFont
**
**	Description : Comme son nom l'indique.
**
**	Input : Le display, le nom de la fonte
**	Ouput : la structure
*/
XFontStruct *LoadFont ( dpy, str )
	Display *dpy;
	char    *str;
{
	XFontStruct     *font;
	extern void exit ();

   	if ((font = XLoadQueryFont ( dpy, str )) == 0 ) {
		(void) fprintf ( stderr, "Fontname error : %s\n", str );
		if (( font = XLoadQueryFont ( dpy, "fixed" )) == 0 ) {
			(void) fprintf ( stderr, "Can't load font : fixed\n" );
			(void) exit (1);
		}
		( void ) fprintf ( stderr, "Use font : fixed\n" );
        }
        return font;
}


/*
**	Function name : ChangeTextFont
**
**	Description : Change la fonte courante.
**
**	Input : Le display, le text courant, le nom de la nouvelle fonte.
**	Ouput :
*/
void ChangeTextFont ( dpy, text, f )
	Display *dpy;
	Text *text;
	register char *f;
{
	XFontStruct *font;
	register int i;

	TextCursorOff ( text );
	font = LoadFont ( dpy, f );
	i = GetLineInPage ( text, font );

	SetFontText ( dpy, text, font );

	SetScrollFont ( text -> swin, font );
	SetScrollLinePage ( text -> swin, i ); 
	SetLineInPage ( text, i );
	SetScrollBarSize ( dpy, text -> swin );

	ClearPage ( text );
	text -> n1 = 0;
	text -> n2 = text -> lines_in_page - 1;
       
	SetLinesTable ( text );
	ClipOn ( text, 0 );
	RefreshPage ( text );
	ClipOff ( text );

#ifdef DEBUG
       fprintf ( stderr, "no_current = %d\n", text -> no_current_line );
#endif
       
	(void) MoveScrollBar ( dpy,  text -> swin, CURRENT, text -> no_current_line - 1 );
	TextCursorOn ( text );
}


/*
**	Function name : GetLineInPage
**
**	Description : Retourne le nombre de lignes dans la page
**		courante pour une fonte donnee
**
**	Input : Le text courant, la fonte.
**	Ouput : le nombre de lignes.
*/
int GetLineInPage ( text, font )
	Text *text;
	XFontStruct *font;
{
	return ( (text -> height - (2*MARGE)) / 
		(font -> max_bounds.ascent + font -> max_bounds.descent));
}


/*
**	Function name : MouseIn
**
**	Description : La pointeur est dans le text courant.
**
**	Input : le text courant
**	Ouput :
*/
void MouseIn ( text )
	Text *text;
{
#ifdef DEBUG
	(void) fprintf ( stderr, "Mouse in\n" );
#endif
	text -> mouse_in = True;
}


/*
**	Function name : MouseOut
**
**	Description : Le pointeur n'est plus dans le text courant
**
**	Input : Le text courant.
**	Ouput :
*/
void MouseOut ( text )
	Text *text;
{
#ifdef DEBUG
	(void) fprintf ( stderr, "Mouse out\n" );
#endif
	text -> mouse_in = False;
}


/*
**	Function name : ShowWindowText
**
**	Description : Affichage de la fenetre de texte.
**
**	Input : Le display, le text courant, la geometrie.
**	Ouput :
*/
void ShowWindowText ( display, text,width, height )
	Display	*display;
	Text	*text;
	int	width, height;
{
	register int x;
#ifdef DEBUG
	(void) fprintf ( stderr, "ShowWindowText width = %d height = %d\n",
		 width, height );
#endif
	x = height - ( 2 * MARGE );
	text -> lines_in_page = (x / text -> font_height);
	text -> n2 = ( text -> lines_in_page - 1 ) - text -> n1;
	text -> width = width;
	text -> height = height;

#ifdef DEBUG
	(void) fprintf ( stderr, "ShowWindowText text-height = %d\n",
		 text -> height );
	(void) fprintf ( stderr, "lineinpage = %d\n", text -> lines_in_page );
#endif
	XResizeWindow ( display, text -> window, text -> width, text -> height );
	XMapWindow ( display, text -> window );
}


/*
**	Function name : SetTextSave
**
**	Description : Mise a jour des infos apres une sauvegarde
**		du buffer courant,
**	Input : Le text
**	Ouput :
*/
SetTextSave ( text )
	Text *text;
{
	text -> modif = False;
      	text -> mwin -> stat = False;
	RefreshWindowStatBuf ( text -> mwin );
}


/*
**	Function name : SetTextModif
**
**	Description : Mise a jour des infos apres une modification
**		du buffer courant.
**	Input : Le text
**	Ouput :
*/
SetTextModif ( text) 
	Text *text;
{
	text -> modif = True;
         	text -> mwin -> stat = True;
	RefreshWindowStatBuf ( text -> mwin );
}


/*
**	Function name : SetTextMode
**
**	Description :
**	Input : 
**	Ouput :
*/
void SetTextMode ( text )
	Text *text;
{
	text -> mode = TEXT; 
   	text -> mwin -> mode = TEXT;
	SetBrowserMode ( TEXT );
/*	RefreshBrowserControl (); */
	UnmapBrowser ();
	RefreshWindowMode ( text -> mwin );
}


/*
**	Function name : SetCMode
**
**	Description : 
**	Input : 
**	Ouput :
*/
void SetCMode ( text )
	Text *text;
{
	text -> mode = STD_C;
	text -> mwin -> mode = STD_C;   
	SetBrowserMode ( STD_C );
   	RefreshBrowserControl ();
	RefreshWindowMode ( text -> mwin );
}


/*
**	Function name : SetCCMode
**
**	Description : 
**	Input : 
**	Ouput :
*/
void SetCCMode ( text )
	Text *text; 
{
	text -> mode = CPLUS;
	text -> mwin -> mode = CPLUS;
	SetBrowserMode ( CPLUS );
   	RefreshBrowserControl ();
	RefreshWindowMode ( text -> mwin );
}


/*
**	Function name : ChangeDir
**
**	Description : Change de directorie courante.
**	Input : Le text courant.
**	Ouput :
*/
void ChangeDir ( text )
	Text *text;
{
	register char *tmp;
   	char buf [128];
	register int len;
   
	(void) chdir ( text -> current_dir );
   	
   	/* 
   	 * Affichage du filename ou de la directorie.
   	 */
	if ( text -> filename == 0 )
		return;
   	len = strlen ( text -> filename );
	if ( strcmp ( text -> filename + ( len - strlen ( "NoName" )), "NoName" ) == 0 ) {
      		len = strlen ( text -> current_dir );
		if ( len > 20 ) {
			(void) sprintf ( buf, "Dir : ...%s\n", 
				(char *) text -> current_dir + ( len - 20) );
		}
		else
			(void) sprintf ( buf, "Dir : %s\n", text -> current_dir );
		DisplayMessage ( text -> mwin, buf  );
		return;
	}
	tmp = strrchr ( text -> filename, '/' );
	if ( tmp != 0 )
		DisplayMessage ( text -> mwin, tmp + 1 );
}


/*
**	Function name : ExposeTextWindow
**
**	Description : Traitement d'un expose event dans une 
**		fenetre de texte.
**	Input : Le display, le text courant, la fenetre exposee.
**	Ouput :
*/
void ExposeTextWindow ( dpy, text, ev )
	Display *dpy;
	Text *text;
	XEvent *ev;
{
	XRectangle rec [2];
   	Region region;
   	XEvent tmp;

	Display3D ( dpy, text -> window,
		text -> top_sh,
		text -> bot_sh,
		text -> width_relief, DOWN );

	region = XCreateRegion ();

	rec [0].x = ((XExposeEvent *) ev) -> x;
	rec [0].y = ((XExposeEvent *) ev) -> y;
	rec [0].width = ((XExposeEvent *) ev) -> width;
	rec [0].height = ((XExposeEvent *) ev) -> height;

	XUnionRectWithRegion ( rec, region, region );
	while ( XCheckTypedWindowEvent ( dpy, text -> window, Expose, &tmp )) {
		rec [0].x = tmp.xexpose.x;
		rec [0].y = tmp.xexpose.y;
		rec [0].width = tmp.xexpose.width;
		rec [0].height = tmp.xexpose.height;
		XUnionRectWithRegion ( rec, region, region );
	}
	XSetRegion ( dpy, text -> Cgc, region );
	ExposePage ( region, text );
	XSetClipMask ( dpy, text -> Cgc, None );
	XDestroyRegion ( region );
	SetCurrentLine ( text );
	if ( text -> mouse_in == True ) 
		TextCursorOn ( text );
	else
	       FreeseTextCursor ( text ); 
	return;
}
