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
#include <string.h>

#include "options.h"
#include "xcoral.h"
#include "browser.h"
#include "logo.bm"
#include "flist.h"

extern Trans st_initial;

static void SetProp ();
static int add_window ();

/*
**	Function name : CreateWindow
**
**	Description : Fabrication d'une fenetre d'edition.
**	Input : 
**	Ouput : La structure associee.
*/
EdWin *CreateWindow ()
{
	EdWin		*edtmp;
	Window		root;
	int 		screen;
	char	 	*malloc ();
	XYMenu 		*MakeMenus (); 
	Text		*MakeTextWindow ();
	MWin		*MakeControlPanel ();
	static 		Atom wm_delete_window;

	screen = DefaultScreen ( dpy );
	root = RootWindow ( dpy, screen );
	
	edtmp = ( EdWin * ) malloc ( sizeof ( EdWin ));

	if ( add_window ( edtmp ) < 0 ) {
      		if ( edtmp != 0 )
			(void) free ( (char *) edtmp );
		return 0;
	}
	
	edtmp -> w_frame = XCreateSimpleWindow (dpy, root,
		GetOpGeo ( OP_X ), GetOpGeo ( OP_Y ),
		GetOpGeo ( OP_WIDTH ), GetOpGeo( OP_HEIGHT ), GetOpBW (),
		GetOpBD (), GetOpColor ( OP_MENU_BG ));

	(void) SetProp ( edtmp -> w_frame ); 

	XSelectInput ( dpy, edtmp->w_frame , ExposureMask |
		ButtonPressMask | ButtonReleaseMask | EnterWindowMask
		| LeaveWindowMask | KeyPressMask | StructureNotifyMask );

	edtmp -> menu = MakeMenus ( dpy, edtmp->w_frame, 
		menu_names, item_names, func_names );

	edtmp -> mwin = MakeControlPanel ( edtmp->w_frame );

	edtmp -> scroll = MakeScroll ( dpy, edtmp->w_frame,
		GetOpGeo ( OP_WIDTH ) - GetScrollWidth () - W_SPACE,
		HeightMenuBar ( edtmp -> menu -> font ) + 1 + W_SPACE );

	SetScrollFont ( edtmp -> scroll, GetOpFont ( OP_TEXT_FONT) );

	edtmp -> text = MakeTextWindow ( dpy, edtmp -> w_frame, 
	        W_SPACE, HeightMenuBar ( edtmp -> menu -> font ) + 1 + W_SPACE );

	edtmp -> buf = GetBuffer ( (unsigned) SIZEOF_BUFFER );

	edtmp -> text -> buf = edtmp -> buf;
	edtmp -> text -> swin = edtmp -> scroll;
	edtmp -> text -> mwin = edtmp -> mwin;
	edtmp -> scroll -> text = ( char * ) edtmp -> text;

	SetHiddenWindow ( edtmp -> menu, edtmp -> text -> window, edtmp -> text -> Cgc );

	if ( XSaveContext ( dpy, edtmp->w_frame, EdContext, (caddr_t) edtmp ) != 0 )
		( void ) printf ("create_env XSaveContext Error\n" );

	edtmp -> stat = (ST *) &st_initial;
	edtmp -> width = edtmp -> height = 0;
	(void) strcpy ( edtmp -> text -> filename, "NoName" ); 

	wm_delete_window = XInternAtom( dpy, "WM_DELETE_WINDOW", False);
	(void) XSetWMProtocols ( dpy, edtmp -> w_frame, &wm_delete_window, 1);

	return edtmp;
}

/*
**	Function name : ConfigWindow
**
**	Description : Positionne la geometrie des fenetres.
**	Input : Le contexte d'edition, largeur er hauteur.
**	Ouput :
*/
ConfigWindow ( e, width, height )
	EdWin   *e;
	register width, height;
{
       register int hbar, hmess, wscroll, i, x;

       e -> width = width;
       e -> height = height;
       hbar = HeightMenuBar ( e -> menu -> font );
       hmess = HeightOfMess ();
       wscroll = GetScrollWidth ();

#ifdef DEBUG
       fprintf ( stderr, "ConfigWindow width = %d, height = %d\n",
		width, height );
       fprintf ( stderr, "ConfigWindow hbar = %d, hmess = %d, wscroll = %d\n",
		hbar, hmess, wscroll );
#endif

       x = height - ( hbar + hmess + 2 ) - ( 2 * W_SPACE );
       x -= ( 2 * MARGE );
	i = x / e -> text -> font_height;

       SetScrollLinePage ( e -> scroll, i );
       SetMenuBarWidth ( e -> menu, width );
       ShowControlPanel ( e -> mwin, width, height );
       ShowWindowText ( dpy, e -> text, width - wscroll - 1 - ( 2 * W_SPACE ) ,
		       height - hbar - hmess - 2 - ( 2 * W_SPACE) );

       	if ( DoesSaveUnders ( DefaultScreenOfDisplay ( dpy ) ) != True )
		SetMenuPixmap ( dpy, e -> menu, width - wscroll - 1 );

       i = e -> text -> no_current_line - e -> text -> n1 - 1;
#ifdef DEBUG
       fprintf ( stderr, "no_current = %d n1 = %d\n", 
		e -> text -> no_current_line, e -> text -> n1 );
#endif
       RefreshScroll ( dpy, e -> scroll, width - W_SPACE,
		height - hbar - hmess - 2 - ( 2 * W_SPACE) , i );
}


/*
**	Function name : SetProp
**
**	Description : Initialisation des proprietes pour une fenetre
**		d'edition.
**	Input : La fenetre
**	Ouput :
*/
static void SetProp ( w )
	Window 	w;
{
	XSizeHints sizehints;
	XWMHints wm_hints;
	
	wm_hints.icon_pixmap = XCreatePixmapFromBitmapData ( dpy, w, icon_bits,
			icon_width, icon_height,
			WhitePixel ( dpy, DefaultScreen ( dpy )),
			BlackPixel ( dpy, DefaultScreen ( dpy )),
			DefaultDepth ( dpy, DefaultScreen ( dpy )));
	
/*	sizehints.flags = PMinSize | PMaxSize | PPosition | PSize | USPosition;  */

	sizehints.flags = PMinSize | PMaxSize;

	sizehints.height = GetOpGeo ( OP_HEIGHT );
	sizehints.width = GetOpGeo ( OP_WIDTH );
	sizehints.x = GetOpGeo ( OP_X );
	sizehints.y = GetOpGeo ( OP_Y );

	if ( sizehints.x != 0 ||sizehints.y != 0 ) 
		sizehints.flags |=  USPosition;
#ifdef DEBUG
	(void) fprintf ( stderr, "x = %d	y = %d\n", sizehints.x, sizehints.y );
#endif
	sizehints.min_width = (DisplayWidth ( dpy,DefaultScreen ( dpy )) *2 ) / 5; 
	sizehints.min_height = (DisplayHeight ( dpy,DefaultScreen ( dpy )) * 2 ) / 5; 
	sizehints.max_width = (DisplayWidth (dpy,DefaultScreen (dpy)) * 5 ) / 6;
	sizehints.max_height = (DisplayHeight (dpy,DefaultScreen (dpy)) * 5 ) / 6; 

	wm_hints.flags = InputHint | StateHint | IconPixmapHint;
	wm_hints.input = True;
	wm_hints.initial_state = NormalState;

	XSetWMProperties ( dpy, w, 0, 0, 0, 0, &sizehints, &wm_hints, 0 );
}


/*
**	Function name : add_window
**
**	Description : Ajoute une fenetre d'edition dans
**		la table principale.
**	Input : Le contexte d'edition.
**	Ouput : 0 si OK -1 sinon.
*/
static int add_window ( e )
	EdWin	*e;
{
	register EdWin	**t;

	for ( t = TWin; t < TWin + MAXWIN; t++ ) {
		if ( *t == 0 ) {
			 *t  = e;
			 break;
		}
	}
	if ( t == TWin + MAXWIN )
		return (- 1);
	else
		return 0;
}


/*
**	Function name : DisplayOpenFiles
**
**	Description : Affiche la liste des fichiers ouverts.
**	Input : Le text courant.
**	Ouput :
*/
void DisplayOpenFiles ( text )
	Text *text;
{
	register EdWin	**t;
	register char *str, *tmp;
	register int n;
 
   	if ( GetVisibility ( dpy, text ) == False )
		return;

	tmp = (char *) GetStringFromSelect ( text -> window, OPENFILES );
	if ( tmp != 0 ) {
		/* str [2] = 0; */
		str = strrchr ( tmp, '#' );
		str++;
		str [2] = 0;
		if ( (n = atoi (str)) == 0 ) {
			DisplayMessage ( text -> mwin, "Abort" );
			return;
		}
		t = TWin; 
		t += (n-1);
		XMapRaised ( dpy,  (*t) -> w_frame );
      		if ( tmp != 0 )
	      		(void) free ( tmp ); 
          	}
	else
		DisplayMessage ( text -> mwin, "Abort" );
}


/*
**	Function name : LoadFileNames
**
**	Description : Construit la liste de fichiers ouverts
**		en parcourant la table principale.
**	Input : Un buffer pour stocker la liste.
**	Ouput :
*/
void LoadFileNames ( buf )
	Buf *buf;
{
	register EdWin	**t;
	register int i = 1;
	register int j = 1;
	char tmp [256];
	register char *name;

	for ( t = TWin; t < TWin + MAXWIN; t++ ) {
		if ( *t != 0 ) {
			bzero ( tmp, 256 );
			if ( (name = strrchr ( (*t) -> text -> filename, '/' )) != 0 )
	         			if ( (*t) -> text -> modif == True ) 
					(void) sprintf ( tmp,
						" %d  : buffer #%d  **\t%s\n", i, j,(char *) (name+1) );
				else
					(void) sprintf ( tmp,
						" %d  : buffer #%d  \t%s\n", i, j,(char *) (name+1) );
			else 
				(void) sprintf ( tmp,
					" %d  : buffer #%d ...\n", i, j );
			InsertNchar ( buf, tmp, strlen ( tmp ));
			i++;
		}
		j++;
	}
}


/*
**	Function name : IsAlreadyLoad
**
**	Description : Pour eviter les ouvertures multiples
**		de fichiers
**	Input : Le nom du fichier.
**	Ouput : Le nombre de buffer ouvert sur le fichier.
*/
int IsAlreadyLoad ( s, text )
	register char *s;
	Text **text;
{
	register EdWin	**t;
   	register int n = 0;

	for ( t = TWin; t < TWin + MAXWIN; t++ ) {
		if ( *t == 0 )
			 continue;
		if ( strcmp ( s, (*t) -> text -> filename ) == 0 ) {
			n++;
         		   	*text = (*t) -> text;
		}
	}
   	return n;
}


/*
**	Function name : NewWindow
**
**	Description : Construction d'une fenetre d'edition en
**		tenant compte des proprietes de la fenetre parent.
**	Input : Le text courant.
**	Ouput :
*/
void NewWindow ( text )
	Text *text;
{
	EdWin 	*ew;
	ew = CreateWindow (); 

	if ( ew == 0 ) {
		(void) fprintf ( stderr, "Too many open window\n" );
		return;
	}
	ew -> text -> mode = text -> mode;
	ew -> text -> mwin -> mode = text -> mode;
	SetBrowserMode ( text -> mode );
	
	(void) strcpy ( ew -> text -> current_dir,
			text -> current_dir );
}


/*
**	Function name : DeleteWindow
**
**	Description : Detruit une fenetre d'edition.
**	Input : Le text courant.
**	Ouput : 0 si OK -1 sinon.
*/
int DeleteWindow ( text )
Text *text;
{
	register char *s;
	char c = '\007';
	register EdWin	**t, *tmp;
/*   
   	if ( GetVisibility ( dpy, text ) == False )
		return -1;
*/
	if ( GetModif ( text ) == True ) {
      		if ( SaveCurrentBuffer ( text, F_MENU ) != True )
      			return -1;
	}
	if ( IsLastWindow ( 1 ) == True ) {
		s = ( char * ) GetStringFromDialogBox ( text -> window, 
			"Last window. Quit [y/n] : " );
		if ( (s == 0) || (strncmp(s,"y",1) == 0) ) {
			DisplayMessage ( text -> mwin, "Quit" );
		}
		else if ( (strncmp(s,"n",1 ) == 0) || (strncmp(s,&c,1)==0) ) {
			DisplayMessage ( text -> mwin, "Abort" );
			return -1;
		}
	}

	for ( t = TWin; t < TWin + MAXWIN; t++ ) {
		if ( *t == 0 ) continue;
		if ( text == ( *t ) -> text ) {
			tmp = *t;
			break;
		}
	}

   	XUnmapWindow ( dpy, tmp -> w_frame ); 
 	DeleteMenu ( dpy, tmp -> menu );
	DeleteBuffer ( tmp -> buf );
	DeleteText ( dpy, tmp -> text );
	DeleteControlPanel ( tmp -> mwin );
	DeleteScroll ( dpy, tmp -> scroll );
	XSync ( dpy, False );

	for ( t = TWin; t < TWin + MAXWIN; t++ ) {
		if ( *t == 0 ) continue;
		if ( *t == tmp ) {
         			if ( tmp != 0 )
				(void) free ( (char *) tmp ); 
			*t = 0;
			break;
		}
	}
	return 0;
}


/*
**	Function name : IsLastWindow
**
**	Description : Comme son l'indique.
**	Input : Le nombre de fenetre.
**	Ouput : Vrai ou faux.
*/
int IsLastWindow ( nb )
	register int nb;
{
	register EdWin	**t;
	register int 	n;
	
	n = 0;
	for ( t = TWin; t < TWin + MAXWIN; t++ ) {
		if ( *t != 0 )
			n++;
	}
	if ( n != 0 ) {
		if ( nb == n )
			return True;
		else
			return False;
	}
	else
		return True;
}


/*
**	Function name : Version
**
**	Description : Affiche la version courante de xcoral.
**	Input : Le text courant
**	Ouput :
*/
void Version ( text )
Text *text;
{
   	char tmp [64];
/*   
	if ( GetVisibility ( dpy, text ) != True )
		return;
*/   
   	(void) sprintf ( tmp, "Message : %s", CURRENT_VERSION );
	(void) GetStringFromDialogBox ( text -> window,  tmp );
}


/*
**	Function name : Help
**
**	Description : En attendant d'avoir un help serieux.
**	Input : Le text courant. 
**	Ouput :
*/
void Help ( text )
Text *text;
{
/*
	if ( GetVisibility ( dpy, text ) != True )
		return;
*/
	(void) GetStringFromDialogBox ( text -> window, 
		"Message :  Help yourself" );
}





