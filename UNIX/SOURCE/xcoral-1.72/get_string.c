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
#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>

#include "options.h"
#include "flist.h"

#define D_WIDTH		10
#define D_HEIGHT	10
#define D_SHADOW	6
#define D_WARNING	0
#define D_DIALOG	1
#define D_MESSAGE	2
#define DS_OK		"  Ok...  "
#define DS_DIALOG	"Dialog Box"
#define DS_CANCEL	"Cancel"

typedef struct {
	Window		frame, shadow, title, main, ok, cancel;
	unsigned long 	fg, bg, ts, bs;
	GC		gc;
	XFontStruct 	*font;
	int		t_height, b_width;
	int		width, height;
} Dialog;

Dialog dial;

extern Display	*dpy;
extern char 	*getenv ();
char 		*GetStringFromWindow ();
char		*ExpandTildeName ();


/*
**	Function name : InitDialogWindow
**
**	Description : Initialisation de la fenetre de dialog,
**		la fonte, les couleurs etc...
**	Input : 
**	Ouput :
*/
void InitDialogWindow ()
{
	XWindowAttributes att;
    	Window root;
    	int screen;
	unsigned long black, white;
	XGCValues	 gcv;
	XSizeHints sizehints;

	black = BlackPixel ( dpy, DefaultScreen ( dpy ));
	white = WhitePixel ( dpy, DefaultScreen ( dpy ));

	dial.fg = (DefaultDepth ( dpy, DefaultScreen ( dpy )) == 1) ? 
		black : GetOpColor ( OP_MENU_FG );
	dial.bg = (DefaultDepth ( dpy, DefaultScreen ( dpy )) == 1) ?
		white : GetOpColor ( OP_MENU_BG );
	dial.ts = GetOpColor ( OP_MENU_TS );
	dial.bs = GetOpColor ( OP_MENU_BS );
	dial.font = GetOpFont ( OP_TEXT_FONT );

	dial.gc = XCreateGC ( dpy, DefaultRootWindow ( dpy ), 0,  &gcv );
 	XCopyGC ( dpy, DefaultGC (dpy, DefaultScreen ( dpy )), (~0), dial.gc );

	XSetFont ( dpy, dial.gc, dial.font -> fid );
	XSetForeground ( dpy, dial.gc, dial.fg );
	XSetBackground ( dpy, dial.gc, dial.bg );

	screen = DefaultScreen ( dpy );
	root = RootWindow ( dpy, screen );
	XGetWindowAttributes ( dpy, root, &att );
	dial.width = att.width / 2;
	dial.height = att.height / 5 ;

	dial.frame = XCreateSimpleWindow ( dpy, root, 
		((att.width/2) - (dial.width/2)),((att.height/2) - (dial.height/2)),
		dial.width, dial.height, 0, black, black );

	dial.t_height = dial.font -> ascent + dial.font -> descent + 10;
	dial.title = XCreateSimpleWindow ( dpy,	dial.frame, 0, 0,
		dial.width, dial.t_height, 0, dial.fg, dial.bg);

	dial.main = XCreateSimpleWindow ( dpy, dial.frame, 0, dial.t_height,
		dial.width, dial.height - dial.t_height, 0, dial.fg, dial.bg);

	dial.b_width = XTextWidth ( dial.font, DS_OK,  strlen (DS_OK)) + 10;

	dial.ok = XCreateSimpleWindow ( dpy, dial.main, (dial.width/2) - (2*dial.b_width),
		dial.height/3, dial.b_width, dial.t_height, 0, dial.fg, dial.bg );
	
	dial.cancel = XCreateSimpleWindow ( dpy, dial.main, (dial.width/2) + dial.b_width,
		dial.height/3, dial.b_width, dial.t_height, 0, dial.fg, dial.bg );

	sizehints.flags = PPosition | PSize | USPosition | USSize ;
	sizehints.height = dial.height;
	sizehints.width = dial.width;
	sizehints.x = ((att.width/2) - (dial.width/2));
	sizehints.y = ((att.height/2) - (dial.height/2));

	XSetWMProperties ( dpy, dial.frame, 0, 0, 0, 0, &sizehints, 0, 0 );
	
	XSelectInput ( dpy, dial.main, ExposureMask | KeyPressMask );
	XSelectInput ( dpy, dial.title, ExposureMask );
	XSelectInput ( dpy, dial.cancel, ButtonPressMask |ButtonReleaseMask | ExposureMask);
	XSelectInput ( dpy, dial.ok, ButtonPressMask |ButtonReleaseMask | ExposureMask );
}


/*
**	Function name : GetStringFromDialogBox
**
**	Description : Ouvre une fenetre de dialog (grabbee),
**		et manage les differentes actions possibles
**		pour l'utilisaterur. Clavier et bouttons.
**
**	Input : La fenetre parent, le prompt.
**	Ouput : La chaine.
*/
char *GetStringFromDialogBox ( w, s )
	Window	w;
	char 	*s;
{
	register int x;
	XEvent event;
	register char *str;
	register int type;

	if ( strncmp ( s, "Warning", 7 ) == 0 )
		type = D_WARNING;
	else if ( strncmp ( s, "Message", 7 ) == 0 )
		type = D_MESSAGE;
	else
		type = D_DIALOG;
	
	XMapSubwindows (dpy, dial.main );
	XMapWindow ( dpy, dial.main );
	XMapSubwindows ( dpy, dial.frame);
	XMapRaised ( dpy, dial.frame);
	XWindowEvent ( dpy, dial.main, ExposureMask, &event );

	if ( (type != D_WARNING) && (type != D_MESSAGE) ) 
		XWindowEvent ( dpy, dial.cancel, ExposureMask, &event );
	else 
		XUnmapWindow ( dpy, dial.cancel );

	Display3D ( dpy, dial.title, dial.ts, dial.bs, 1, 0 );
	Display3D ( dpy, dial.main, dial.ts, dial.bs, 1, 0 );
	Display3D ( dpy, dial.ok, dial.ts, dial.bs, 1, 0 );
	if ( (type != D_WARNING) && (type != D_MESSAGE) )
		Display3D ( dpy, dial.cancel, dial.ts, dial.bs, 1, 0 );

	x = XTextWidth ( dial.font, DS_DIALOG, strlen (DS_DIALOG));
	XDrawString ( dpy, dial.title, dial.gc, (dial.width - x)/2,
		dial.font -> ascent + 5, DS_DIALOG, strlen (DS_DIALOG) );
	x = XTextWidth ( dial.font, DS_OK, strlen(DS_OK));
	XDrawString ( dpy, dial.ok, dial.gc, (dial.b_width - x)/2,
		dial.font -> ascent + 5, DS_OK, strlen(DS_OK));
	x = XTextWidth ( dial.font, DS_CANCEL, strlen (DS_CANCEL));
	if ( (type != D_WARNING) && (type != D_MESSAGE) )
		XDrawString ( dpy, dial.cancel, dial.gc,
		(dial.b_width - x)/2, dial.font -> ascent + 5,
		DS_CANCEL, strlen (DS_CANCEL));

 	str = GetStringFromWindow ( dial.main, dial.gc, dial.font,
		s, 20, dial.font -> ascent + 20 , (char *) 0, 1 );

	XUnmapSubwindows ( dpy, dial.main );
	XUnmapWindow ( dpy, dial.main );
	XUnmapSubwindows ( dpy, dial.frame );
	XUnmapWindow ( dpy, dial.frame );

	return str;
}


/*
**	Function name : GetStrinFromWindow
**
**	Description : Assure toute la gestion concernant
**		l'entree d'une chaine de caracteres.
**	Input : La fenetre, le contexte graphique, la fonte,
**		le prompt, la position de depart, le type de
**		reponse ( chaine complete, ou un seul caractere )
**		et un flag pour savoir si on doit tenir compte 
**		des 'ButtonPress'.
**		
**	Ouput : La chaine.
*/
char *GetStringFromWindow ( w, gc, font, s, x, y, reply, flag )
	Window		w;
	GC		gc;
	XFontStruct	*font;
	register char 	*s;
	register int 	x, y;
	register char 	*reply;
	register int 	flag;
{
	XEvent 		event;
	register int 	length, cur_x, cur_x_old;
	XComposeStatus 	compose;
	KeySym		keysym;
	register int 	start_x, max, font_h;
	XWindowAttributes att;
	register int type;
    	Window root, parent, **child;
    	unsigned int nchild;
    
#define MAXLENGTH 	256
#define VOLUME		10
	static char string [MAXLENGTH];
	static char buffer [MAXLENGTH];

	if ( strncmp ( s, "Warning", 7 ) == 0 )
		type = D_WARNING;
	else if ( strncmp ( s, "Message", 7 ) == 0 )
		type = D_MESSAGE;
	else
		type = D_DIALOG;
	XGetWindowAttributes ( dpy, w, &att );

	XQueryTree ( dpy, w, &root, &parent, (Window **) &child, &nchild );

	if ( nchild == 0 ) 
		XGrabPointer ( dpy, w, True, ButtonPress, GrabModeAsync,
			GrabModeAsync, None, None, CurrentTime ); 
	else 
		XGrabPointer ( dpy, w, True, ButtonPress, GrabModeAsync,
			GrabModeAsync, w, None, CurrentTime );

	if ( child != 0 )
		XFree ( (char *) child );

	XFlush (dpy );
	max = att.width - 40;
	font_h = font -> ascent + font -> descent;
	if ( type == D_MESSAGE ) s += strlen ( "Message : " );
	XDrawImageString ( dpy, w, gc, x, y, s, strlen (s) );
	if ( type == D_DIALOG ) {
		start_x = x + XTextWidth ( font, s, strlen (s) );
		XFillRectangle ( dpy, w, gc, start_x, y - font -> ascent,  font_h/2, font_h );
	}
	cur_x = start_x;
	string [0] = buffer [0] = buffer [1] = '\0';
	for (;;) {
		XNextEvent ( dpy, &event );
		switch ( event.type ) {
		case ButtonPress :
			if ( flag == 0 ) continue;
			if ( event.xbutton.window == dial.ok ) {
				Display3D ( dpy, dial.ok, dial.ts, dial.bs, 1, 1 );
				WaitButtonRelease ( event.xbutton.button );
				if ( strlen(string) )
					if ( string [0] != '~' )
						return string;
					else
						return ( (char *) ExpandTildeName ( string ));
				else
					return 0;
			}
			if ( event.xbutton.window == dial.cancel ) {
				Display3D ( dpy, dial.cancel, dial.ts, dial.bs, 1, 1 );
				WaitButtonRelease ( event.xbutton.button );
				string [0] = '\007';
				string [1] = '\0';
				return string;
			}
			break;
		case KeyPress:
			if ( (type == D_WARNING) || (type == D_MESSAGE) )
				continue;
			XLookupString ( (XKeyEvent *) &event.xkey, buffer,MAXLENGTH, 
					&keysym, &compose );
			 /* Cas return, linefeed ou enter */
			if ( (keysym == XK_Return) || (keysym == XK_KP_Enter)
				|| (keysym == XK_Linefeed) ) {
				XClearArea( dpy, w, x,
					   y - font -> ascent,
					   XTextWidth ( font, string,
						       strlen ( string )) +
					   XTextWidth ( font, s, strlen (s)) + font_h/2, font_h, False );
				XUngrabPointer ( dpy, CurrentTime );
				if ( reply != 0 ) {
					(void) strncat ( string, "\n", 1 );
					return string;
				}
				if ( strlen ( string ))
					if ( string [0] != '~' )
						return string;
					else
						return ( (char *) ExpandTildeName ( string ));
				else 
					return 0;
			}
			/* Cas du Controle G */
			else if ( (keysym == XK_Control_L) ||
				  (keysym == XK_Control_R)) {
				  continue;
				  /* on ne fait rien */
			}
			else if ( keysym == XK_g  && event.xkey.state == ControlMask ) { 
				XUngrabPointer ( dpy, CurrentTime ); 
				string [0] = '\007';
				string [1] = '\0';
				return string;
			}
			/* Cas des caracteres imprimables */
			else if ( ( (keysym >= XK_KP_Space)
					&& (keysym <= XK_KP_9) )
				  || ( (keysym >= XK_space)
				     	&& (keysym <= XK_asciitilde) ) ) {
				if ( reply != 0 ) {
					XUngrabPointer ( dpy, CurrentTime );
       					(void) strncat ( string, buffer, 1 );
       					string [1] = '\0';
       					return string;
				}
				if ( (strlen(string) + strlen(buffer))
					>= MAXLENGTH 
					|| XTextWidth ( font, s, strlen (s))
						+ XTextWidth ( font, string,
						strlen (string)) > max ) {
							XBell ( dpy, VOLUME );
							continue;
						      }
				else {
					(void) strncat ( string, buffer, 1 );
				      }
			}
			/* Cas shift */
			else if ( (keysym >= XK_Shift_L )
				  && (keysym <= XK_Shift_R) ) {
				  /* On fait rien */
			        continue;
			}
			/* Cas des fonctions speciales */
			else if ( (keysym >= XK_F1) && (keysym <= XK_F35) ) {
				if ( buffer == 0 )
					(void) fprintf ( stderr, "Umapped function key\n" );
				/* on ne fait rien */
				else if ( (strlen(string) + strlen(buffer)) >= MAXLENGTH ) 
					XBell ( dpy, VOLUME );
				else {
					XBell ( dpy, VOLUME );
					continue;
				}
			}

			/* Cas delete et backspace. */
			else if ( (keysym == XK_BackSpace) || (keysym ==XK_Delete) ) {
				if ( (length = strlen(string)) > 0 ) {
					string [ length - 1 ] = 0;
					cur_x_old = cur_x;
					cur_x = start_x + 
						XTextWidth ( font, string, strlen (string));
					XClearArea( dpy, w, cur_x, y - font -> ascent,
						   (font_h / 2) + (cur_x_old - cur_x),
						   font_h, False );

					XFillRectangle ( dpy, w, gc, cur_x, y - font -> ascent, 
						font_h/2, font_h );
				}
				else
					XBell ( dpy, VOLUME );
				continue;
			}
			else {
				XBell ( dpy, VOLUME );
				continue;
			}

			XDrawImageString ( dpy, w, gc, cur_x, y, buffer, 1 );
			cur_x_old = cur_x;
			cur_x += XTextWidth ( font, buffer, 1 );
			XFillRectangle ( dpy, w, gc, cur_x, y - font -> ascent,  font_h/2, font_h );
			break;
		}
	}
}


/*
**	Function name : ExpandTildeName
**
**	Description : Recherche le path de l'expression ~[name]
**	Input : ~/, ~name 
**	Ouput : Le path
*/
char *ExpandTildeName ( name )
	register char *name;
{
	register struct passwd *pwd;
	register char *home;
	register char *p = name + 1;
	static char  buf [256];

	bzero ( buf, 128 );
	if ( name [1] == '/' ) {	/* c'est moi */
		if ( (home = (char *) getenv ( "HOME" )) == 0 )
			return name;
		(void) strcpy ( buf, home );
		(void) strcat ( buf, name + 1 );
	}
	else { /* c'est un autre */
		while ( *p && *p != '/' ) p++;
		*p = 0;
		pwd = getpwnam ( name + 1 );
		if (  pwd ) {
			(void) strcpy ( buf, pwd ->  pw_dir);
			*p = '/';
			(void) strcat ( buf, p );
		}
		else
			return name;
	}
	return buf;
}
